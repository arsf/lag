/*
 * File:   Point.cpp
 * Author: jaho
 *
 * Created on 10 March 2012
 *
 * ----------------------------------------------------------------
 *
 * This file is part of lidarquadtree, a library providing a data
 * structure for storing and indexing LiDAR points.
 *
 * Copyright (C) 2008-2012 Plymouth Marin Laboratory
 *    arsfinternal@pml.ac.uk
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * The GNU General Public License is contained in the file COPYING. *
 */

#include "Point.h"
#include <math.h>

//Default
Point::Point()
{
}

// 1D Point
Point::Point(double x)
:
	x_	(x),
	y_	(0),
	z_	(0)
{
}

// 2D Point
Point::Point(double x, double y)
:
	x_	(x),
	y_	(y),
	z_	(0)
{
}

// 3D Point
Point::Point(double x, double y, double z)
:
	x_	(x),
	y_	(y),
	z_	(z)
{
}

//  Euclidian distance
double Point::distanceTo(const Point& other)
{
   return sqrt((x_-other.x_)*(x_-other.x_) +
               (y_-other.y_)*(y_-other.y_) +
               (z_-other.z_)*(z_-other.z_));
}

bool operator==(Point& lhs, Point& rhs)
{
	return lhs.x_ == rhs.x_ && lhs.y_ == rhs.y_ && lhs.z_ == rhs.z_;
}

bool operator!=(Point& lhs, Point& rhs)
{
	if (lhs == rhs)
		return false;
	else
		return true;
}

// Mutator methods for the Point
void Point::translate(double x, double y , double z)
{
   x_ += x;
   y_ += y;
   z_ += z;
}

void Point::move(double x, double y, double z)
{
   x_ = x;
   y_ = y;
   z_ = z;
}

const Point Point::operator+(Point& other) const
{
   return Point(
      x_ + other.getX(),
      y_ + other.getY(),
      z_ + other.getZ() );
}

const Point Point::operator-(Point& other) const
{
   return Point(
      x_ - other.getX(),
      y_ - other.getY(),
      z_ - other.getZ() );
}
