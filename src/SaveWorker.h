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

class FileSaver;

class SaveWorker : public Worker
{
public:
	SaveWorker(FileSaver* fs, std::string filename, std::string filein, int flightline, std::string parse_string, bool use_latlong, bool use_default_scalefactor, double scale_factor[3]);

protected:
	void run();

	FileSaver* filesaver;
	std::string filename;
	std::string source_filename;
	int flightline_number;
	std::string parse_string;
	bool use_latlong;
	bool use_default_scalefactor;
	double scale_factor[3];
};


#endif /* SAVEWORKER_H_ */
