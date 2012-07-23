/*
===============================================================================

 SelectionBox.cpp

 Created on: December 2010
 Author: Haraldur Tristan Gunnarsson, Jan Holownia

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

#include "SelectionBox.h"
#include <stdio.h>


/*
==================================
 SelectionBox::SelectionBox
==================================
*/
SelectionBox::SelectionBox(double firstX, double firstY, double firstZ,
                           double secondX, double secondY, double secondZ,
                           double thirdX, double thirdY, double thirdZ,
                           double fourthX, double fourthY, double fourthZ)
{
   points[0].move(firstX, firstY, firstZ);
   points[1].move(secondX, secondY, secondZ);
   points[2].move(thirdX, thirdY, thirdZ);
   points[3].move(fourthX, fourthY, fourthZ);
}

/*
==================================
 SelectionBox::SelectionBox
==================================
*/
SelectionBox::SelectionBox(Point first, Point second, 
                           Point third, Point fourth)
{
   points[0] = first;
   points[1] = second;
   points[2] = third;
   points[3] = fourth;
}

/*
==================================
 SelectionBox::~SelectionBox
==================================
*/
SelectionBox::~SelectionBox()
{
}

/*
==================================
 SelectionBox::move
==================================
*/
void SelectionBox::move(double moveX, double moveY, double moveZ)
{
   for (int i = 0; i < NUMBER_OF_POINTS; i++)
      points[i].move(moveX, moveY, moveZ);
}

/*
==================================
 SelectionBox::translate
==================================
*/
void SelectionBox::translate(double transX, double transY, double transZ)
{
   for (int i = 0; i < NUMBER_OF_POINTS; i++)
      points[i].translate(transX, transY, transZ);
}

/*
==================================
 SelectionBox::movePoint
==================================
*/
void SelectionBox::movePoint(int pointNumber, double moveX, double moveY, double moveZ)
{
   points[pointNumber].move(moveX, moveY, moveZ);
}

/*
==================================
 SelectionBox::translatePoint
==================================
*/
void SelectionBox::translatePoint(int pointNumber, double transX, double transY, double transZ)
{
   points[pointNumber].translate(transX, transY, transZ);
}

/*
==================================
 SelectionBox::getCorner
==================================
*/
Point SelectionBox::getCorner(int cornerNumber)
{
   return points[cornerNumber];
}

/*
==================================
 SelectionBox::getCorners
==================================
*/
Point* SelectionBox::getCorners()
{
   return points;
}

/*
==================================
 SelectionBox::getXs
==================================
*/
std::vector<double> SelectionBox::getXs()
{
   std::vector<double> returner(4);
   for (int i=0; i < 4; i++)
      returner[i] = points[i].getX();
   return returner;
}

/*
==================================
 SelectionBox::getYs
==================================
*/
std::vector<double> SelectionBox::getYs()
{
   std::vector<double> returner(4);
   for (int i=0; i < 4; i++) {
      returner[i] = points[i].getY();
   }
   return returner;
}

/*
==================================
 SelectionBox::get_min_x
==================================
*/
double SelectionBox::get_min_x()
{
	double x = points[0].getX();
	for (int i = 1; i < 4; ++i)
	{
		if (points[i].getX() < x)
			x = points[i].getX();
	}
	return x;
}

/*
==================================
 SelectionBox::get_max_x
==================================
*/
double SelectionBox::get_max_x()
{
	double x = points[0].getX();
	for (int i = 1; i < 4; ++i)
	{
		if (points[i].getX() > x)
			x = points[i].getX();
	}
	return x;
}

/*
==================================
 SelectionBox::get_min_y
==================================
*/
double SelectionBox::get_min_y()
{
	double y = points[0].getY();
	for (int i = 1; i < 4; ++i)
	{
		if (points[i].getY() < y)
			y = points[i].getY();
	}
	return y;
}

/*
==================================
 SelectionBox::get_max_y
==================================
*/
double SelectionBox::get_max_y()
{
	double y = points[0].getY();
	for (int i = 1; i < 4; ++i)
	{
		if (points[i].getY() > y)
			y = points[i].getY();
	}
	return y;
}
