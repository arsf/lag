/*
 ==================================
 LoadWorker.h

 Created on: 24 Apr 2012
 Author: jaho
 ==================================
 */

#ifndef LOADWORKER_H_
#define LOADWORKER_H_

#include "Worker.h"
#include "SelectionBox.h"
#include <stdint.h>
#include <tr1/unordered_map>
#include "geoprojectionconverter.hpp"
#include "laslib/lasdefinitions.hpp"
#include "PointFilter.h"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class FileOpener;
class Boundary;
class Quadtree;
class LASreader;

class LoadWorker: public Worker
{
public:
	LoadWorker(FileOpener* fo, int point_offset,
			std::vector<std::string> filenames, bool s, bool usearea,
			int resolutiondepth, int resolutionbase, int bucketlevels,
			int bucketlimit, int cachelimit, bool default_scale_factors,
			double scale_factor[3], std::string ascii_code, SelectionBox fence,
			PointFilter pf, fs::path cache_path);

	void stop();

	Glib::Dispatcher sig_message;
	Glib::Dispatcher sig_file_loaded;
	Glib::Dispatcher sig_fail;
	Glib::Dispatcher sig_progress;

	static std::tr1::unordered_map<uint8_t, fs::path> point_data_paths;
	static std::vector<int> point_number;

protected:
	void run();
	void send_message(std::string message);
	Boundary get_boundary();
	int load_points(Quadtree* qt);
	int load_points_wf(Quadtree* qt);

	void convert_projection();
	std::string get_utm_zone();

	FileOpener* fileopener;
	int point_offset;
	std::vector<std::string> filenames;
	bool create_new_quadtree;
	bool usearea;
	int resolutiondepth;
	int resolutionbase;
	int bucketLevels;
	int bucketlimit;
	int cachelimit;
	bool use_default_scale_factors;
	double scale_factor[3];
	std::string ascii_code;
	SelectionBox fence;
	PointFilter point_filter;
	fs::path cache_path;

	bool latlong;
	GeoProjectionConverter gpc;
	LASquantizer* reproject_quantizer;
	LASquantizer* saved_quantizer;
	LASreader* reader;
};

#endif /* LOADWORKER_H_ */
