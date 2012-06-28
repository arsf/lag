/*
==================================
 FileUtils.h

  Created on: 19 Jun 2012
      Author: jaho

 This file contains various definitions and
 methods used by LoadWorker and SaveWorker.
==================================
*/

#ifndef FILEUTILS_H_
#define FILEUTILS_H_

#include <string>
#include "laslib/lasdefinitions.hpp"
#include "geoprojectionconverter.hpp"

class LASreader;

// Supported file types
typedef enum
{
	LAS_FILE,
	ASCII_FILE,
	UNKNOWN_FILE
}  filetype_t;

filetype_t test_filename(std::string const& filename);
bool has_waveform(LASreader const* reader);
bool is_latlong(LASreader const* reader);
char* convert_string(std::string const& str);


#endif /* FILEUTILS_H_ */
