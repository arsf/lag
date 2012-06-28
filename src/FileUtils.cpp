/*
==================================
 FileUtils.cpp

  Created on: 19 Jun 2012
      Author: jaho
==================================
*/

#include <sstream>
#include <iostream>
#include "FileUtils.h"
#include "laslib/lasreader.hpp"

/*
==================================
 test_filename

 Determines type of the file based on its extension.
 Returns type of the file: LAS_FILE, ASCII_FILE or UNKNOWN_FILE.
==================================
*/
filetype_t test_filename(std::string const& filename)
{
	if (filename.find(".las", filename.length() - 4) != std::string::npos
		|| filename.find(".LAS", filename.length() - 4) != std::string::npos)
	{
		return LAS_FILE;
	}
	else if (filename.find(".txt", filename.length() - 4) != std::string::npos
			|| filename.find(".TXT", filename.length() - 4) != std::string::npos
			|| filename.find(".csv", filename.length() - 4) != std::string::npos
			|| filename.find(".CSV", filename.length() - 4) != std::string::npos)
	{
		return ASCII_FILE;
	}

	return UNKNOWN_FILE;
}

/*
==================================
 has_waveform

 Given a LASreader object checks if the associated file contains waveform information.
 Note it only checks point version and doesn't actually search for waveform data.
==================================
*/
bool has_waveform(LASreader const* reader)
{
	return (reader->header.version_minor >= 3);
}


/*
==================================
 is_latlong

 Given a LASreader object checks if the associated file
 is in latlong projection. Returns true/false.
==================================
*/
bool is_latlong(LASreader const* reader)
{
	return (-360 < reader->header.min_x  && -360 < reader->header.min_y && reader->header.max_x < 360 && reader->header.max_y < 360);
}

/*
==================================
 convert_string

 Converts a std::string to char*. Returns a copy of the string.
 The object has to be deleted by the caller.
==================================
*/
char* convert_string(std::string const& str)
{
	char* c = new char[str.size() + 1];
	strcpy(c, str.c_str());
	return c;
}

