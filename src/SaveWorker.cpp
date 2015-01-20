/*
===============================================================================

 SaveWorker.cpp

 Created on: 10 May 2012
 Authors: Jan Holownia, Berin Smaldon

 LIDAR Analysis GUI (LAG), viewer for LIDAR files in .LAS or ASCII format
 Copyright (C) 2009-2012 Plymouth Marine Laboratory (PML)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

===============================================================================
*/

#include "SaveWorker.h"
#include "LoadWorker.h"
#include "ui/FileSaver.h"
#include "laslib/lasreader.hpp"
#include "laslib/laswriter.hpp"
#include "LidarPoint.h"
#include "FileUtils.h"


/*
==================================
 SaveWorker::SaveWorker
==================================
*/
SaveWorker::SaveWorker(FileSaver* fs, std::string filename, std::string filein,
   		int flightline, std::string parse_string, bool use_latlong,
	   	bool use_default_scalefactor, double scale_factor[3],
         Glib::Mutex* pointbucket_mutex) :
		Worker(),
		filesaver               (fs),
		filename                (filename),
		source_filename         (filein),
		flightline_number       (flightline),
		parse_string            (parse_string),
		latlong_output          (use_latlong),
		latlong_input           (false),
		use_default_scalefactor	(use_default_scalefactor),
		reader                  (0),
		writer                  (0),
		reproject_quantizer     (0),
		saved_quantizer         (0),
      pointbucket_mutex       (pointbucket_mutex)
{
	if (!this->use_default_scalefactor)
	{
		this->scale_factor[0] = scale_factor[0];
		this->scale_factor[1] = scale_factor[1];
		this->scale_factor[2] = scale_factor[2];
	}
	else
	{
		this->scale_factor[0] = this->scale_factor[1] = this->scale_factor[2] = 0;
	}
}

/*
==================================
 SaveWorker::save_points

 Saves an array of LidarPoints to a file.
==================================
*/
void SaveWorker::save_points(int n, LidarPoint* points)
{
	LASpoint* point = new LASpoint();
	point->init(&reader->header, reader->header.point_data_format, reader->header.point_data_record_length, &reader->header);

	for (int k = 0; k < n; ++k)
	{
		++number_of_points;

		point->gps_time = points[k].getTime();
		point->classification = points[k].getClassification();

		// latlong check and convertions
		if (latlong_output)
		{
			double coords[3];
			coords[0] = points[k].getX();
			coords[1] = points[k].getY();
			coords[2] = points[k].getZ();
			gpc.to_target(coords);
			point->set_x(coords[0]);
			point->set_y(coords[1]);
			point->set_z(coords[2]);
		}
		else
		{
			point->set_x(points[k].getX());
			point->set_y(points[k].getY());
			point->set_z(points[k].getZ());
		}

		point->edge_of_flight_line = points[k].getScanEdge();
		point->intensity = points[k].getIntensity();
		point->number_of_returns = points[k].getNumberOfReturns();
		point->point_source_ID = points[k].getPointSourceId();
		point->return_number = points[k].getReturn();
		point->scan_angle_rank = points[k].getScanAngle();
		point->scan_direction_flag = points[k].getScanDirection();

		// reprojection
		if (reproject_quantizer && !latlong_input)
		{
			point->compute_coordinates();
			gpc.to_target(point->coordinates);
			point->compute_XYZ(reproject_quantizer);
		}

		writer->write_point(point);
		writer->update_inventory(point);
	}

	delete point;
}

/*
==================================
 SaveWorker::save_points_wf

 Saves an array of LidarPoints to a file.
 Attached 1.3 waveform attributes from PointData file.
==================================
*/
void SaveWorker::save_points_wf(int n, LidarPoint* points)
{
	LASpoint* point = new LASpoint();
	point->init(&reader->header, reader->header.point_data_format, reader->header.point_data_record_length, &reader->header);

	// waveform stuff
	string data_filename;
	FILE* datafile = 0;
	char* buffer = 0;

	// Get PointData file
	tr1::unordered_map<uint8_t, std::string>::iterator it = LoadWorker::point_data_paths.find(flightline_number);
	if (it != LoadWorker::point_data_paths.end())
	{
		data_filename = it->second;
	}
	else
	{
		throw OutOfBoundsException("SaveWorker: Flightline number does not exist.");
	}

	datafile = fopen(data_filename.c_str(), "rb");
	if (datafile == NULL)
		throw FileException("SaveWorker: Error opening data file.");

	buffer = new char[sizeof(PointData)];

	for (int k = 0; k < n; ++k)
	{
		++number_of_points;

		point->gps_time = points[k].getTime();
		point->classification = points[k].getClassification();

		// latlong check and convertions
		if (latlong_output)
		{
			double coords[3];
			coords[0] = points[k].getX();
			coords[1] = points[k].getY();
			coords[2] = points[k].getZ();
			gpc.to_target(coords);
			point->set_x(coords[0]);
			point->set_y(coords[1]);
			point->set_z(coords[2]);
		}
		else
		{
			point->set_x(points[k].getX());
			point->set_y(points[k].getY());
			point->set_z(points[k].getZ());
		}

		point->edge_of_flight_line = points[k].getScanEdge();
		point->intensity = points[k].getIntensity();
		point->number_of_returns = points[k].getNumberOfReturns();
		point->point_source_ID = points[k].getPointSourceId();
		point->return_number = points[k].getReturn();
		point->scan_angle_rank = points[k].getScanAngle();
		point->scan_direction_flag = points[k].getScanDirection();

		// waveform stuff
		int s = sizeof(PointData);
		PointData* temp;
		fseek(datafile, points[k].getDataindex() * s, SEEK_SET);
		fread(buffer, s, 1, datafile);
		temp = (PointData*) buffer;

		point->wavepacket.setIndex(temp->wfDescriptorIndex);
		point->wavepacket.setOffset(temp->wfOffset);
		point->wavepacket.setSize(temp->wfPacketSize);
		point->wavepacket.setLocation(temp->wfPointLocation);
		point->wavepacket.setXt(temp->xt);
		point->wavepacket.setYt(temp->yt);
		point->wavepacket.setZt(temp->zt);


		// reprojection
		if (reproject_quantizer && !latlong_input)
		{
			point->compute_coordinates();
			gpc.to_target(point->coordinates);
			point->compute_XYZ(reproject_quantizer);
		}

		writer->write_point(point);
		writer->update_inventory(point);
	}
	delete point;

	// close waveform temp file
	fclose(datafile);
	delete buffer;
}

/*
==================================
 SaveWorker::close

 Updates header of the output file.
 Updates waveform location information in 1.3 files and copies waveform data.
==================================
*/
void SaveWorker::close()
{
	writer->update_header(&reader->header, TRUE);

	// Check if input file contains waveform.
	// If it does update output file header and append the data.
	if (reader->header.version_minor >= 3)
	{
		// Let the GUI know we will be copying waveform data
		sig_waveform();

		FILE* fileout=0;
		FILE* filein=0;

		fileout = fopen(filename.c_str(), "rb+");
		if (fileout == NULL)
		{
			throw FileException("Error opening output file for writing waveform data.");
		}

		// Manually update start_of_waveform_data_record in the header which is set to 0 by laslib.
		// Otherwise we'd have to modify the library.
		fseek(fileout, 227, SEEK_SET);
		fwrite(&reader->header.start_of_waveform_data_packet_record, sizeof(I64), 1, fileout);

		// Copy waveform data
		std::cout << "Copying waveform data. Please be patient..." << std::endl;
		filein = fopen(source_filename.c_str(), "rb");
		if (filein == NULL)
		{
			throw FileException("Error opening input file for reading waveform data.");
		}

		// We need a file size to be able to track progress
		fseek(filein, 0L, SEEK_END);
		long int file_size = ftell(filein);

		// Now go back to the start of the waveform data
		fseek(filein, reader->header.start_of_waveform_data_packet_record, SEEK_SET);

		// And to the end of the output file
		fseek(fileout, 0, SEEK_END);

		// How much we've got to read
		long int progress_step = (file_size - ftell(filein)) / 100;

		// The copying reads a 100th of the waveform data at the time (usually around 5 - 20 Mb)
		// and then signals progress after writing it to an output file.
		size_t size;
		char* buf = new char[progress_step];

		while ((size = fread(buf, 1, progress_step, filein)) > 0 && !stopped)
		{
			fwrite(buf, 1, size, fileout);
			sig_waveform_progress();
		}

		delete buf;

		if (filein) fclose(filein);
		if (fileout) fclose(fileout);
	}
}

/*
==================================
 SaveWorker::run

 Runs the worker.
==================================
*/
void SaveWorker::run()
{
	if (filesaver->lidardata == NULL)
		return;
	
	LASreadOpener lasreadopener;
	LASwriteOpener laswriteopener;

	lasreadopener.set_file_name(source_filename.c_str());

	// ASCII?
	filetype_t source_filetype = test_filename(source_filename);
	if (source_filetype == ASCII_FILE)
	{
		lasreadopener.set_parse_string(parse_string.c_str());
		if (!use_default_scalefactor)
			lasreadopener.set_scale_factor(scale_factor);
	}

	reader = lasreadopener.open();

	if (reader == NULL)
	{
		std::cout << "SaveWorker: Error creating reader.\n";
		sig_done();
		return;
	}

	number_of_points = 0;

	// Conversions
	latlong_input = is_latlong(reader);

	std::cout << "latlong_input : " << latlong_input << ", latlong_output: " << latlong_output << std::endl;

	if (latlong_input || latlong_output)
		convert_projection();

	if (reproject_quantizer)
	{
		if (latlong_input && !latlong_output)
			reader->header = *reproject_quantizer;
		else
			reader->header = *saved_quantizer;
	}

	laswriteopener.set_file_name(filename.c_str());

	// Ascii check
	if (test_filename(filename) == ASCII_FILE)
	{
		laswriteopener.set_parse_string(parse_string.c_str());
	}

	writer = laswriteopener.open(&reader->header);

	if (writer == NULL)
	{
		std::cout << "SaveWorker: Error creating writer.\n";
		sig_done();
		return;
	}

   // cache points from the quadtree here before saving them
	LidarPoint *points = new LidarPoint[SAVEWORKER_CACHE_POINTS];
	int counter = 0;

   // describes data to be saved
	vector<PointBucket*> *buckets;
	Boundary *b = filesaver->lidardata->getBoundary();

	vector<double> Xs(4);
	vector<double> Ys(4);
	Xs[0] = Xs[1] = b->minX - 5;
	Xs[2] = Xs[3] = b->maxX + 5;
	Ys[0] = Ys[3] = b->minY - 5;
	Ys[1] = Ys[2] = b->maxY + 5;

	buckets = filesaver->lidardata->advSubset(Xs, Ys, 4);

	// track progress
	int progress_counter = 0;
	int progress_step = buckets->size() < 100 ? 1 : int(buckets->size() / 100);

	// use appropriate function to save points depending on the point format
	typedef void (SaveWorker::*save_points_fun_t)(int n, LidarPoint* points);
	save_points_fun_t save_points_fun;

	if(has_waveform(reader))
		save_points_fun = &SaveWorker::save_points_wf;
	else
		save_points_fun = &SaveWorker::save_points;

   // exclusive access to pointbuckets
   {
      PointBucket *current;
      Glib::Mutex::Lock lock (*pointbucket_mutex);

      // Go through the point buckets until you have enough point
      // then save them to the file, start again with the next point.
      for (unsigned int k = 0; k < buckets->size(); ++k)
      {
         current = buckets->at(k);

         for (int i = 0; i < current->getNumberOfPoints(0); ++i)
         {
            // only use points from the specified flightline
            if (current->getPoint(i, 0).getFlightline() == flightline_number)
            {
               points[counter] = current->getPoint(i, 0);
               counter++;
               if (counter == SAVEWORKER_CACHE_POINTS)
               {
                  (this->*save_points_fun)(counter, points);
                  counter = 0;
               }
            }
         }

         if (progress_step == ++progress_counter)
         {
            sig_progress();
            progress_counter = 0;
         }

         if (stopped)
            break;
      }
   }

	(this->*save_points_fun)(counter, points);
	close();

	delete buckets;
	delete b;
	delete[] points;

	if (reproject_quantizer)
	{
		delete reproject_quantizer;
		delete saved_quantizer;
	}

	sig_done();
}

/*
==================================
 SaveWorker::run

 Stops the worker.
==================================
*/
void SaveWorker::stop()
{
	Glib::Mutex::Lock lock (mutex);
	stopped = true;
}

/*
==================================
 convert_projection

 Converts between UTM and latlong projections.
==================================
*/
void SaveWorker::convert_projection()
{
	reproject_quantizer = 0;
	saved_quantizer = 0;

	char tmp[256];

	// If the input is in latlong we need two converters
	if (latlong_input)
	{
		GeoProjectionParametersUTM* utm = new GeoProjectionParametersUTM();
		std::stringstream zone;
		gpc.compute_utm_zone(
				(reader->header.min_y + reader->header.max_y) / 2,
				(reader->header.min_x + reader->header.max_x) / 2, utm);
		zone << utm->utm_zone_number << utm->utm_zone_letter;

		// A converter to convert from UTM data in quandtree to latlong
		gpc.set_utm_projection(const_cast<char*>(zone.str().c_str()), tmp,
				true);

		// A converter to convert from latlong data in the reader->header to UTM
		gpc_ll.set_longlat_projection(tmp, true);

		delete utm;
	}
	// If the input is in UTM we have to find the zone from the VLRs
	// Note that this will not work with ascii
	else
	{
		if (reader->header.vlr_geo_keys)
		{
			gpc.set_projection_from_geo_keys(
					reader->header.vlr_geo_keys[0].number_of_keys,
					(GeoProjectionGeoKeys*) reader->header.vlr_geo_key_entries,
					reader->header.vlr_geo_ascii_params,
					reader->header.vlr_geo_double_params);
		}
		else
		{
			std::cout
					<< "No projection information in the input file. Will output rubbish.";
		}
	}

	if (latlong_output)
	{
		gpc.set_longlat_projection(tmp, false);
		gpc_ll.set_target_utm_projection(tmp);
	}
	else
	{
		gpc.set_target_utm_projection(tmp);
	}

	// latlong to UTM
	if (latlong_input && !latlong_output)
	{
		reproject_quantizer = new LASquantizer();

		double point[3];
		point[0] = (reader->header.min_x + reader->header.max_x) / 2;
		point[1] = (reader->header.min_y + reader->header.max_y) / 2;
		point[2] = (reader->header.min_z + reader->header.max_z) / 2;
		gpc_ll.to_target(point);

		reproject_quantizer->x_scale_factor = gpc_ll.get_target_precision();
		reproject_quantizer->y_scale_factor = gpc_ll.get_target_precision();
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
				!gpc_ll.has_projection(false)))
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
	}
	// latlong to latlong
	else if (latlong_input && latlong_output)
	{
		reproject_quantizer = new LASquantizer();

		double point[3];
		point[0] = (reader->header.min_x + reader->header.max_x) / 2;
		point[1] = (reader->header.min_y + reader->header.max_y) / 2;
		point[2] = (reader->header.min_z + reader->header.max_z) / 2;
		gpc_ll.to_target(point);
		gpc.to_target(point);

		reproject_quantizer->x_scale_factor = gpc.get_target_precision();
		reproject_quantizer->y_scale_factor = gpc.get_target_precision();
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
	}
	// UTM to latlong
	else
	{
		reproject_quantizer = new LASquantizer();

		double point[3];
		point[0] = (reader->header.min_x + reader->header.max_x) / 2;
		point[1] = (reader->header.min_y + reader->header.max_y) / 2;
		point[2] = (reader->header.min_z + reader->header.max_z) / 2;
		gpc.to_target(point);

		reproject_quantizer->x_scale_factor = gpc.get_target_precision();
		reproject_quantizer->y_scale_factor = gpc.get_target_precision();
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
	}

	saved_quantizer = new LASquantizer();
	*saved_quantizer = reader->header;
}
