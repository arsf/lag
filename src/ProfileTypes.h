/*
 ===============================================================================

 ProfileTypes.h

 Created on: 31 August 2012
 Authors: Berin Smaldon

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

#ifndef PROFILETYPES_H_
#define PROFILETYPES_H_
#include <utility>
#include "Point.h"

/*
 * Mostly just a quick convenience & clarity header, shared between Profile and
 * any of the Profile's Worker classes that need it
 *
 */

/*
 ================================================================================
 FenceType

 Defines a box (or fence) from two points
 ================================================================================
 */
typedef std::pair<Point, Point> FenceType;

/*
 ================================================================================
 ClassificationJob

 ClassifyWorker(s) takes jobs from a respective Profile as and when ready.
 ClassificationJob is a type which bundles all the necessary data to specify a
 job into one object, handy for things like forming lists
 ================================================================================
 */
typedef std::pair<FenceType, uint8_t> ClassificationJob;

#endif /* #ifdef PROFILETYPES_H_ */
