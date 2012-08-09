/*
===============================================================================

 SaveWorker.h

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

#ifndef SAVEWORKER_H_
#define SAVEWORKER_H_

#include "Worker.h"
#include "geoprojectionconverter.hpp"
#include "laslib/lasdefinitions.hpp"

class FileSaver;
class LASreader;
class LASwriter;
class LidarPoint;


/*
===============================================================================

 SaveWorker - a worker class for saving files.

===============================================================================
*/
class SaveWorker: public Worker
{
public:
	SaveWorker(FileSaver* fs, std::string filename, std::string filein,
			int flightline, std::string parse_string, bool use_latlong,
			bool use_default_scalefactor, double scale_factor[3],
         Glib::Mutex* pointbucket_mutex);
	void stop();

	Glib::Dispatcher sig_progress;			// Signals that 1% of saving has completed
	Glib::Dispatcher sig_waveform;			// Signals that there is full waveform
	Glib::Dispatcher sig_waveform_progress; // Signals 1% of waveform data copied

protected:
	void run();
	void save_points(int n, LidarPoint* points);
	void save_points_wf(int n, LidarPoint* points);
	void close();
	void convert_projection();

   Glib::Mutex* pointbucket_mutex;

	FileSaver* filesaver;
	std::string filename;
	std::string source_filename;
	int flightline_number;
	std::string parse_string;
	bool latlong_output;
	bool latlong_input;
	bool use_default_scalefactor;
	double scale_factor[3];
	int number_of_points;

	LASreader* reader;
	LASwriter* writer;

	GeoProjectionConverter gpc;
	GeoProjectionConverter gpc_ll;
	LASquantizer* reproject_quantizer;
	LASquantizer* saved_quantizer;
};

#endif /* SAVEWORKER_H_ */
