/*
 ==================================
 SaveWorker.h

 Created on: 10 May 2012
 Author: jaho
 ==================================
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

class SaveWorker: public Worker
{
public:
	SaveWorker(FileSaver* fs, std::string filename, std::string filein,
			int flightline, std::string parse_string, bool use_latlong,
			bool use_default_scalefactor, double scale_factor[3]);
	void stop();

	Glib::Dispatcher sig_progress;
	Glib::Dispatcher sig_waveform;
	Glib::Dispatcher sig_waveform_progress;

protected:
	void run();
	void save_points(int n, LidarPoint* points);
	void save_points_wf(int n, LidarPoint* points);
	void close();
	void convert_projection();

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
