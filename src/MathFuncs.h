/*
 ===============================================================================

 MathFuncs.h

 Created on: December 2009
 Authors: Haraldur Tristan Gunnarsson, Jan Holownia

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

#ifndef MATHFUNCS_H
#define MATHFUNCS_H

#include <vector>
#include <iterator>
#include <cmath>

#include "PointBucket.h"

using namespace std;

double percentilevalue(double* data, int datasize, double percentile, double minval, double maxval);

bool* vetpoints(PointBucket* points, std::vector<double> xs, std::vector<double> ys, int numberofcorners, bool profileNoisePoints);

bool* vetpoints_slice(PointBucket* points, std::vector<double> xs, std::vector<double> ys, int numberofcorners, bool profileNoisePoints, double minz,
                      double maxz);

#endif
