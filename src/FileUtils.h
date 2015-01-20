/*
 ===============================================================================

 FileUtils.h

 This file contains various definitions and
 methods used by LoadWorker and SaveWorker.

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

#ifndef FILEUTILS_H_
#define FILEUTILS_H_

#include <string>
#include "laslib/lasdefinitions.hpp"
#include "geoprojectionconverter.hpp"

class LASreader;

// Supported file types
typedef enum
{
   LAS_FILE, ASCII_FILE, UNKNOWN_FILE
} filetype_t;

filetype_t test_filename(std::string const& filename);
bool has_waveform(LASreader const* reader);
bool is_latlong(LASreader const* reader);
char* convert_string(std::string const& str);

#endif /* FILEUTILS_H_ */
