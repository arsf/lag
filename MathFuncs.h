/*
 * LIDAR Analysis GUI (LAG), viewer for LIDAR files in .LAS or ASCII format
 * Copyright (C) 2009-2010 Plymouth Marine Laboratory (PML)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * File: MathFuncs.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: December 2009 - July 2010
 *
 * */
#include <vector>
#include <iterator>
#include <cmath>
#include "QuadtreeStructs.h"
#include "PointBucket.h"
using namespace std;
#ifndef MATHFUNCS_H
#define MATHFUNCS_H

//Returns the value of the given percentile of a dataset. Makes a histogram of the dataset and goes along it until it gets a total count corresponding with the percentile. NOTE: not currently used by anything. Keep?
double percentilevalue(double* data,int datasize,double percentile,double minval,double maxval);

//Determines whether the points in the sent bucket fit within the profile area.
bool* vetpoints(PointBucket* points,double* xs,double* ys,int numberofcorners);

#endif
