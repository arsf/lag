/*
 ===============================================================================

 SelectionBox.h

 Created on: December 2010
 Author: Haraldur Tristan Gunnarsson

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

#ifndef _SELECTIONBOX_H_
#define _SELECTIONBOX_H_

#include "Point.h"
#include <vector>

/*
 ===============================================================================

 SelectionBox - Class to hold info about a selection box.  This will be a rectangle
 holding	four Points - one for each corner.

 ===============================================================================
 */
class SelectionBox
{
      
   public:
      SelectionBox()
      {
      }
      
      SelectionBox(double, double, double, double, double, double, double, double, double, double, double, double);

      SelectionBox(Point, Point, Point, Point);

      ~SelectionBox();

      Point getCorner(int);
      Point* getCorners();

      std::vector<double> getXs();
      std::vector<double> getYs();

      double get_min_x();
      double get_max_x();
      double get_min_y();
      double get_max_y();

      // Mutators for the whole SelectionBox
      void move(double, double, double);
      void translate(double, double, double);

      // Mutators for the individual Points.
      void movePoint(int, double, double, double);
      void translatePoint(int, double, double, double);

      // Rotate one day?
      
   private:
      static const int NUMBER_OF_POINTS = 4;
      Point points[4];
};

#endif
