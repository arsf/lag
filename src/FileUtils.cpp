/*
 ===============================================================================

 FileUtils.cpp

 Created on: 19 Jun 2012
 Authors: Jan Holownia

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
   if(filename.find(".las", filename.length() - 4) != std::string::npos || filename.find(".LAS", filename.length() - 4) != std::string::npos
      || filename.find(".laz", filename.length() - 4) != std::string::npos || filename.find(".LAZ", filename.length() - 4) != std::string::npos)
   {
      return LAS_FILE;
   } else if(filename.find(".txt", filename.length() - 4) != std::string::npos || filename.find(".TXT", filename.length() - 4) != std::string::npos
             || filename.find(".csv", filename.length() - 4) != std::string::npos || filename.find(".CSV", filename.length() - 4) != std::string::npos)
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
   return (-360 < reader->header.min_x && -360 < reader->header.min_y && reader->header.max_x < 360 && reader->header.max_y < 360);
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

