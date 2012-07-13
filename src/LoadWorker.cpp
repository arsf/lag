/*
 ==================================
 LoadWorker.cpp

 Created on: 24 Apr 2012
 Author: jaho
 ==================================
 */

#include "LoadWorker.h"
#include "FileUtils.h"
#include "CollisionDetection.h"
#include "ui/FileOpener.h"
#include "laslib/lasreader.hpp"
#include "PointData.h"
#include <cstdio>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace std;
namespace fs = boost::filesystem;

std::vector<int> LoadWorker::point_number;
tr1::unordered_map<uint8_t, fs::path> LoadWorker::point_data_paths;


/*
 ==================================
 LoadWorker::LoadWorker
 ==================================
 */
LoadWorker::LoadWorker(FileOpener* fo, int point_offset,
		std::vector<std::string> filenames, bool create_new, bool usearea,
		int resolutiondepth, int resolutionbase, int bucketlevels,
		int bucketlimit, int cachelimit, bool default_scale_factors,
		double scale_factor[3], std::string ascii_code, SelectionBox fence,
		PointFilter pf, fs::path cache_path) :
		Worker(),
		fileopener			(fo),
		point_offset		(point_offset),
		filenames			(filenames),
		create_new_quadtree	(create_new),
		usearea				(usearea),
		resolutiondepth		(resolutiondepth),
		resolutionbase		(resolutionbase),
		bucketLevels		(bucketlevels),
		bucketlimit			(bucketlimit),
		cachelimit			(cachelimit),
		use_default_scale_factors	(default_scale_factors),
		ascii_code			(ascii_code),
		fence				(fence),
		point_filter		(pf),
		cache_path			(cache_path)
{
	if (!use_default_scale_factors)
	{
		this->scale_factor[0] = scale_factor[0];
		this->scale_factor[1] = scale_factor[1];
		this->scale_factor[2] = scale_factor[2];
	}
	else
	{
		this->scale_factor[0] = this->scale_factor[1] = this->scale_factor[2] =
				0;
	}
	latlong = false;
	reproject_quantizer = 0;
	saved_quantizer = 0;
}

/*
 ==================================
 LoadWorker::send_message

 Passes a message to the main GUI thread.
 set_thread_message() method has to be thread safe.
 ==================================
 */
void LoadWorker::send_message(std::string message)
{
	fileopener->set_thread_message(message);
	sig_message();
}

/*
 ==================================
 LoadWorker::get_boundary

 Returns a boundary object with min-max x,y values provided a LASreader.
 ==================================
 */
Boundary LoadWorker::get_boundary()
{
	Boundary newboundary;

	if (latlong)
	{
		double min[3];
		double max[3];
		min[0] = reader->header.min_x;
		min[1] = reader->header.min_y;
		min[2] = reader->header.min_z;
		max[0] = reader->header.max_x;
		max[1] = reader->header.max_y;
		max[2] = reader->header.max_z;
		gpc.to_target(min);
		gpc.to_target(max);

		newboundary.minX = min[0] - 1;
		newboundary.minY = min[1] - 1;
		newboundary.maxX = max[0] + 1;
		newboundary.maxY = max[1] + 1;
	}
	else
	{
		newboundary.minX = reader->header.min_x;
		newboundary.minY = reader->header.min_y;
		newboundary.maxX = reader->header.max_x;
		newboundary.maxY = reader->header.max_y;
	}

	return newboundary;
}

/*
 ==================================
 LoadWorker::load_points

 Loads points from the file and inserts them into the quadtree.
 ==================================
 */
int LoadWorker::load_points(Quadtree* qt)
{
	int points_loaded = 0;
	int points_to_load = reader->header.number_of_point_records
			/ (point_offset + 1);
	int flightline_number = qt->getFlightlinesNumber();

	LidarPoint temp_point;

	int skip_counter = 0;
	int progress_counter = 0;
	int progress_step = int((points_to_load + 1) / 100);

	try
	{
		while (!stopped && points_loaded < points_to_load
				&& reader->read_point())
		{
			if (skip_counter != point_offset)
			{
				++skip_counter;
				continue;
			}

			// Track progress
			if (++progress_counter == progress_step)
			{
				sig_progress();
				progress_counter = 0;
			}

			if (latlong)
			{
				reader->header = *saved_quantizer;
				reader->point.compute_coordinates();
				gpc.to_target(reader->point.coordinates);
				reader->point.compute_xyz(reproject_quantizer);
				reader->header = *reproject_quantizer;
			}

			if (usearea)
			{
				if (!vectorTest(fence.getXs(), fence.getYs(), 4,
						reader->point.get_x(), reader->point.get_y()))
				{
					skip_counter = 0;
					continue;
				}

			}

			temp_point = reader->point;
			temp_point.setFlightlineNumber(flightline_number);

			++points_loaded;
			skip_counter = 0;

			qt->insert(temp_point);
		}
	} catch (DescriptiveException e)
	{
		std::cout << "An exception happened in LoadWorker::load_points(): "
				<< e.why() << std::endl;
	}

	return points_loaded;
}

/*
 ==================================
 LoadWorker::load_points_wf

 Loads points from the file and inserts them into the quadtree.
 Creates a temporary file to store additional, waveform related,
 attributes from 1.3 files.
 ==================================
 */
int LoadWorker::load_points_wf(Quadtree* qt)
{
	int points_loaded = 0;
	int points_to_load = reader->header.number_of_point_records
			/ (point_offset + 1);
	int flightline_number = qt->getFlightlinesNumber();

	LidarPoint temp_point;
	PointData temp_data;
	PointData* point_data_array = NULL;
   fs::path point_data_path;
	FILE* point_data_file = NULL;

	int skip_counter = 0;
	int progress_counter = 0;
	int progress_step = int((points_to_load + 1) / 100);

	try
	{
		// Prepare storage for waveform data
		point_data_array = new PointData[points_to_load];  // Fix this! 1048576

		// Create temporary file for PointData or use one created already
		tr1::unordered_map<uint8_t, fs::path>::iterator it =
				point_data_paths.find(flightline_number);
		if (it == point_data_paths.end())
		{
         point_data_path = fs::path(cache_path) /
                           fs::unique_path("lag-pointdata.%%%%%%");

			point_data_paths.insert(
					make_pair(flightline_number, point_data_path));
			point_number.resize(flightline_number + 1);
			point_number.at(flightline_number) = 0;
			cout  << "Creating PointData file at: " << point_data_path.c_str()
			   	<< std::endl;
		}
		else
		{
			point_data_path = it->second;
		}

		point_data_file = fopen(point_data_path.c_str(), "ab");
		if (point_data_file == NULL)
		{
			delete[] point_data_array;
			throw FileException(
					"Error opening output file for writing waveform data.");
		}


		while (!stopped && points_loaded < points_to_load
				&& reader->read_point())
		{
			if (skip_counter != point_offset)
			{
				++skip_counter;
				continue;
			}

			// Track progress
			if (++progress_counter == progress_step)
			{
				sig_progress();
				progress_counter = 0;
			}

			if (latlong)
			{
				reader->header = *saved_quantizer;
				reader->point.compute_coordinates();
				gpc.to_target(reader->point.coordinates);
				reader->point.compute_xyz(reproject_quantizer);
				reader->header = *reproject_quantizer;
			}

			if (usearea)
			{
				if (!vectorTest(fence.getXs(), fence.getYs(), 4,
						reader->point.get_x(), reader->point.get_y()))
				{
					skip_counter = 0;
					continue;
				}

			}

			// Create temp_point
			temp_point = reader->point;

			// 1.3 stuff
			temp_data = reader->point.wavepacket;
			temp_point.setDataindex(point_number.at(flightline_number));
			++point_number.at(flightline_number);
			point_data_array[points_loaded] = temp_data;

			++points_loaded;
			skip_counter = 0;

			qt->insert(temp_point);
		}

		// Save 1.3 data
		fseek(point_data_file, 0, SEEK_END);
		fwrite(point_data_array, points_to_load * sizeof(PointData), 1,
				point_data_file);
		fclose(point_data_file);
		delete[] point_data_array;

	} catch (DescriptiveException& e)
	{
		std::cout << "An exception happend in load_points: " << e.why()
				<< std::endl;
	}

	return points_loaded;
}

/*
 ==================================
 LoadWorker::run()

 Starts the worker.
 ==================================
 */
void LoadWorker::run()
{
	int num_files = filenames.size();

	// Has a new quadtree been created?
	bool newQuadtree = false;

	try
	{
		// For each file
		for (int i = 0; i < num_files; ++i)
		{
			std::string filename = filenames[i];
			send_message(filename);

			if (filename != "")
			{
				filetype_t file_type = test_filename(filename);

				if (file_type == UNKNOWN_FILE)
				{
					send_message(
							"Unrecognised file type. Supported formats: las, txt, csv.");
					sig_fail();
					return;
				}

				LASreadOpener lasreadopener;

				lasreadopener.set_file_name(filename.c_str());

				// ASCII files
				if (file_type == ASCII_FILE)
				{
					lasreadopener.set_parse_string(ascii_code.c_str());
					if (!use_default_scale_factors)
					{
						lasreadopener.set_scale_factor(scale_factor);
					}
				}

				// Point filter
				if (point_filter.argc != 0)
				{
					std::vector<char*> argv;
					std::transform(point_filter.args.begin(), point_filter.args.end(), std::back_inserter(argv), convert_string);
					argv.push_back(0);

					if (!lasreadopener.parse(point_filter.argc, &argv[0]))
					{
						for (size_t i = 0; i < argv.size(); ++i)
							delete[] argv[i];

						send_message("Error parsing filter parameters.");
						sig_fail();
						return;
					}

					for (size_t i = 0; i < argv.size(); ++i)
						delete[] argv[i];
				}

				reader = lasreadopener.open();

				if (reader == NULL)
				{
					send_message("Error opening the file.");
					sig_fail();
					return;
				}

				// Check if we have a latlong file
				latlong = is_latlong(reader);

				// If we have latlong convert to UTM
				if (latlong)
					convert_projection();

				// Get file boundary
				Boundary boundary = get_boundary();

				// std::cout << "Boundary: " << boundary.minX << " - " << boundary.maxX << ", " << boundary.minY << " - " << boundary.maxY << std::endl;

				fileopener->set_utm_zone(get_utm_zone());

				// If refreshing
				if ((i == 0
						&& (create_new_quadtree
								|| !fileopener->get_loadedanyfiles()))
						|| fileopener->get_lidardata() == NULL)
				{
					// Delete old quadtree
					fileopener->delete_lidardata();

					// Create new quadree
					Quadtree* qt;

					if (!usearea)
						qt = new Quadtree(&boundary, bucketlimit, cachelimit,
								bucketLevels, resolutionbase, resolutiondepth, cache_path);
					else
					{
						qt = new Quadtree(fence.get_min_x(), fence.get_min_y(),
								fence.get_max_x(), fence.get_max_y(),
								bucketlimit, cachelimit, bucketLevels,
								resolutionbase, resolutiondepth, cache_path);
					}

					// Load points
					int points_loaded = 0;
					if (has_waveform(reader))
						points_loaded = load_points_wf(qt);
					else
						points_loaded = load_points(qt);

					// Add flightline to quadtree's flight table
					qt->addFlightline(filename);

					if (points_loaded == 0)
					{
						std::cout
								<< "LoadWorker: "
								<< filename
								<< " - no points loaded from file. Possibly because of fence."
								<< std::endl;
					}

					// Set lidardata
					fileopener->set_lidardata(qt);

					newQuadtree = true;
					qt = NULL;
				}
				else
				{
					Quadtree* qt = fileopener->get_lidardata();

					// Adjust boundary
					if (!usearea)
						qt->adjustBoundary(boundary);

					// Load points
					int points_loaded = 0;
					if (has_waveform(reader))
						points_loaded = load_points_wf(qt);
					else
						points_loaded = load_points(qt);

					qt->addFlightline(filename);

					if (points_loaded == 0)
					{
						std::cout
								<< "LoadWorker: "
								<< filename
								<< " - no points loaded from file. Possibly because of fence."
								<< std::endl;
					}

					qt = NULL;
				}

				if (reader->header.min_z < fileopener->get_minZ() || i == 0)
					fileopener->set_minZ(reader->header.min_z);
				if (reader->header.max_z > fileopener->get_maxZ() || i == 0)
					fileopener->set_maxZ(reader->header.max_z);
			}
			sig_file_loaded();
			if (stopped)
				break;
		}
	} catch (DescriptiveException& e)
	{
		if (reproject_quantizer)
			delete reproject_quantizer;
		if (saved_quantizer)
			delete saved_quantizer;

		std::cout << "LoadWorker: Error loading file.\n";
		std::cout << e.what() << "\n" << e.why() << std::endl;
		send_message(e.why());

		fileopener->delete_lidardata();

		fileopener->set_loadedanyfiles(false);
		newQuadtree = false;

		sig_fail();

		return;
	}

	// Delete quantizers somewhere
	if (latlong)
	{
		delete reproject_quantizer;
		delete saved_quantizer;
	}

	fileopener->set_newQuadtree(newQuadtree);
	sig_done();
}

/*
 ==================================
 LoadWorker::stop

 Stops loading files.
 ==================================
 */
void LoadWorker::stop()
{
	Glib::Mutex::Lock lock(mutex);
	stopped = true;
}

/*
 ==================================
 LoadWorker::convert_projection

 Converts UTM to latlong.
 ==================================
 */
void LoadWorker::convert_projection()
{
	char tmp[256];

	// In geoprojectionconverter the x = longitude, y = latitude projection is called longlat
	gpc.set_longlat_projection(tmp, true);
	gpc.set_target_utm_projection(tmp);

	reproject_quantizer = 0;
	saved_quantizer = 0;

	reproject_quantizer = new LASquantizer();

	double point[3];
	point[0] = (reader->header.min_x + reader->header.max_x) / 2;
	point[1] = (reader->header.min_y + reader->header.max_y) / 2;
	point[2] = (reader->header.min_z + reader->header.max_z) / 2;
	gpc.to_target(point);

	reproject_quantizer->x_scale_factor = gpc.get_target_precision() * 0.01;
	reproject_quantizer->y_scale_factor = gpc.get_target_precision() * 0.01;
	reproject_quantizer->z_scale_factor = reader->header.z_scale_factor;
	reproject_quantizer->x_offset = ((I64) ((point[0]
			/ reproject_quantizer->x_scale_factor) / 10000000)) * 10000000
			* reproject_quantizer->x_scale_factor;
	reproject_quantizer->y_offset = ((I64) ((point[1]
			/ reproject_quantizer->y_scale_factor) / 10000000)) * 10000000
			* reproject_quantizer->y_scale_factor;
	reproject_quantizer->z_offset = ((I64) ((point[2]
			/ reproject_quantizer->z_scale_factor) / 10000000)) * 10000000
			* reproject_quantizer->z_scale_factor;

	int number_of_keys;
	GeoProjectionGeoKeys* geo_keys = 0;
	int num_geo_double_params;
	double* geo_double_params = 0;

	if (gpc.get_geo_keys_from_projection(number_of_keys, &geo_keys,
			num_geo_double_params, &geo_double_params,
			!gpc.has_projection(false)))
	{
		reader->header.set_geo_keys(number_of_keys,
				(LASvlr_key_entry*) geo_keys);
		free(geo_keys);
		if (geo_double_params)
		{
			reader->header.set_geo_double_params(num_geo_double_params,
					geo_double_params);
			free(geo_double_params);
		}
		else
		{
			reader->header.del_geo_double_params();
		}
		reader->header.del_geo_ascii_params();
	}

	saved_quantizer = new LASquantizer();
	*saved_quantizer = reader->header;
}

/*
 ==================================
 LoadWorker::get_utm_zone

 Tries to find out UTM zone from VLR geo keys. Returns a string with zone name.
 Note that zone information is in the wrong place in files produced by alspp.
 ==================================
 */
std::string LoadWorker::get_utm_zone()
{
	std::stringstream zone;

	if (latlong)
	{
		GeoProjectionParametersUTM* utm = new GeoProjectionParametersUTM();
		gpc.compute_utm_zone((reader->header.min_y + reader->header.max_y) / 2,
				(reader->header.min_x + reader->header.max_x) / 2, utm);
		zone << utm->utm_zone_number << utm->utm_zone_letter;
	}
	else
	{
		if (reader->header.vlr_geo_keys)
		{

			//GeoProjectionConverter gpc;
			gpc.set_projection_from_geo_keys(
					reader->header.vlr_geo_keys[0].number_of_keys,
					(GeoProjectionGeoKeys*) reader->header.vlr_geo_key_entries,
					reader->header.vlr_geo_ascii_params,
					reader->header.vlr_geo_double_params);

			zone << gpc.get_projection_name(true);

		}
		else
		{
			zone << "unknown";
		}
	}
	return zone.str();
}

