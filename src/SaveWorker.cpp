/*
==================================
 SaveWorker.cpp

  Created on: 10 May 2012
      Author: jaho
==================================
*/

#include "SaveWorker.h"
#include "ui/FileSaver.h"


SaveWorker::SaveWorker(FileSaver* fs, std::string filename, std::string filein, int flightline, std::string parse_string, bool use_latlong, bool use_default_scalefactor, double scale_factor[3]) : Worker(),
		filesaver			(fs),
		filename			(filename),
		source_filename		(filein),
		flightline_number	(flightline),
		parse_string		(parse_string),
		use_latlong			(use_latlong),
		use_default_scalefactor	(use_default_scalefactor)
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


void SaveWorker::run()
{
	if (filesaver->lidardata == NULL)
		return;
	
	LasSaver* saver = 0;

	try
	{
		if (filename.find(".txt", filename.length()-4) != std::string::npos || filename.find(".TXT", filename.length()-4) != std::string::npos)
		{
			if (use_default_scalefactor)
			{
				saver = new LasSaver(filename.c_str(), source_filename.c_str(), parse_string.c_str(), use_latlong);
			}
			else
			{
				saver = new LasSaver(filename.c_str(), source_filename.c_str(), parse_string.c_str(), scale_factor, use_latlong);
			}
		}
		else
		{
			saver = new LasSaver(filename.c_str(), source_filename.c_str(), use_latlong);
		}

		filesaver->lidardata->saveFlightLine(flightline_number, saver);

		saver->close();
		delete saver;
	}
	catch (DescriptiveException e)
	{
		std::cout << "SaveWorker: There has been an exception: " << "\n";
		std::cout << "What: " << e.what() << "\n";
		std::cout << "Why: " << e.why() << std::endl;

		if (saver != 0)
			delete saver;

		sig_done();

		return;
	}

	sig_done();
}
