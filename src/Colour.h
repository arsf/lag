/*
 ===============================================================================

 Colour.h

 Created on: June-July 2010
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

#ifndef _COLOUR_H_
#define _COLOUR_H_

#include <string>

/*
 ===============================================================================

 Colour - class to hold information about a colour. This will hold a red, green,
 and blue value.

 ===============================================================================
 */
class Colour
{
      
   public:
      Colour();
      Colour(float, float, float);
      Colour(const std::string&);
      ~Colour();

      // Comparison operators
      friend bool operator==(Colour& lhs, Colour& rhs);
      friend bool operator!=(Colour& lhs, Colour& rhs);

      // Multiply
      void multiply(Colour);
      void multiply(float);
      void multiply(float, float, float);

      // Add
      void add(Colour);
      void add(float);
      void add(float, float, float);

      // Subtract
      void subtract(Colour);
      void subtract(float);
      void subtract(float, float, float);

      // Getters
      float getR();
      float getG();
      float getB();
      float* getRGB();

      // Setters
      void setR(float);
      void setG(float);
      void setB(float);
      void setRGB(float, float, float);

   private:
      float colourValues[3];
};

#endif
