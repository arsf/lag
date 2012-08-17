/*
 * File:   Point.h
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

#ifndef _POINT_H_
#define _POINT_H_


class Point {

public:
   Point();
   // 1D Point
   Point(double);
   // 2D Point
   Point(double, double);
   // 3D Point
   Point(double, double, double);

   // Distance to another Point
   double distanceTo(const Point&);

   // Comparison operators
   friend bool operator== (Point& lhs, Point& rhs);
   friend bool operator!= (Point& lhs, Point& rhs);

   // Methods to move the Point
   void translate(double, double, double);
   void move(double, double, double);

   // Getter methods
   double getX();
   double getY();
   double getZ();

protected:
   double x_;
   double y_;
   double z_;
};

// Getters for coordinates
inline double Point::getX() { return x_; }
inline double Point::getY() { return y_; }
inline double Point::getZ() { return z_; }


#endif

