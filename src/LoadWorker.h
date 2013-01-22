/*
===============================================================================

 LoadWorker.h

 Created on: 24 Apr 2012
 Author: Jan Holownia

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

#ifndef LOADWORKER_H_
#define LOADWORKER_H_

#include "Worker.h"
#include "SelectionBox.h"
#include <stdint.h>
#include <tr1/unordered_map>
#include "geoprojectionconverter.hpp"
#include "laslib/lasdefinitions.hpp"
#include "PointFilter.h"

class FileOpener;
class Boundary;
class Quadtree;
class LASreader;


/*
===============================================================================

 LoadWorker - a worker class for loading files. Also creates and populates the
 Quadtree.

===============================================================================
*/
class LoadWorker: public Worker
{
public:
	LoadWorker(FileOpener* fo, int point_offset,
			std::vector<std::string> filenames, bool s, bool usearea,
			int resolutiondepth, int resolutionbase, int bucketlevels,
			int bucketlimit, int cachelimit, bool default_scale_factors,
			double scale_factor[3], std::string ascii_code, SelectionBox fence,
			PointFilter pf, std::string cache_path);

	void stop();

	Glib::Dispatcher sig_message;		// Signals the GUI to display a message
	Glib::Dispatcher sig_file_loaded;// Signals that a file has been loaded
	Glib::Dispatcher sig_fail;			// Signals failure
	Glib::Dispatcher sig_progress;   // Signals when loading (1%, typically)
	Glib::Dispatcher sig_ascii;      // Signals when not possible to estimate sig_progress

	// This stores location of PointData files with 1.3 point attributes
	static std::tr1::unordered_map<uint8_t, std::string> point_data_paths;
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
	std::string cache_path;

	bool latlong;
	GeoProjectionConverter gpc;
	LASquantizer* reproject_quantizer;
	LASquantizer* saved_quantizer;
	LASreader* reader;
};

#endif /* LOADWORKER_H_ */
