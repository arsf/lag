/*
 ===============================================================================

 Colour.cpp

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

#include "Colour.h"

/*
 ==================================
 Colour::Colour
 ==================================
 */
Colour::Colour()
{
   colourValues[0] = 0.0;
   colourValues[1] = 0.0;
   colourValues[2] = 0.0;
}

/*
 ==================================
 Colour::Colour
 ==================================
 */
Colour::Colour(float r, float g, float b)
{
   colourValues[0] = r;
   colourValues[1] = g;
   colourValues[2] = b;
}

/*
 ==================================
 Colour::Colour
 ==================================
 */
Colour::Colour(const std::string& colourName)
{
   if(colourName == "red")
   {
      colourValues[0] = 1.0f;
      colourValues[1] = 0.0f;
      colourValues[2] = 0.0f;
   } else if(colourName == "white")
   {
      colourValues[0] = 1.0f;
      colourValues[1] = 1.0f;
      colourValues[2] = 1.0f;
   } else if(colourName == "green")
   {
      colourValues[0] = 0.0f;
      colourValues[1] = 1.0f;
      colourValues[2] = 0.0f;
   } else if(colourName == "blue")
   {
      colourValues[0] = 0.0f;
      colourValues[1] = 0.0f;
      colourValues[2] = 1.0f;
   }
}

/*
 ==================================
 operator==
 ==================================
 */
bool operator==(Colour& lhs, Colour& rhs)
{
   for(int i = 0; i < 3; ++i)
   {
      if(lhs.getRGB()[i] != rhs.getRGB()[i])
         return false;
   }
   return true;
}

/*
 ==================================
 operator!=
 ==================================
 */
bool operator!=(Colour& lhs, Colour& rhs)
{
   if(lhs == rhs)
      return false;
   else
      return true;
}

/*
 ==================================
 Colour::multiply
 ==================================
 */
void Colour::multiply(Colour other)
{
   colourValues[0] *= other.getR();
   if(colourValues[0] > 1.0)
      colourValues[0] = 1.0;
   
   colourValues[1] *= other.getG();
   if(colourValues[1] > 1.0)
      colourValues[1] = 1.0;
   
   colourValues[2] *= other.getB();
   if(colourValues[2] > 1.0)
      colourValues[2] = 1.0;
}

/*
 ==================================
 Colour::multiply
 ==================================
 */
void Colour::multiply(float multiplicant)
{
   colourValues[0] *= multiplicant;
   if(colourValues[0] > 1.0)
      colourValues[0] = 1.0;
   
   colourValues[1] *= multiplicant;
   if(colourValues[1] > 1.0)
      colourValues[1] = 1.0;
   
   colourValues[2] *= multiplicant;
   if(colourValues[2] > 1.0)
      colourValues[2] = 1.0;
}

/*
 ==================================
 Colour::multiply
 ==================================
 */
void Colour::multiply(float r, float g, float b)
{
   colourValues[0] *= r;
   if(colourValues[0] > 1.0)
      colourValues[0] = 1.0;
   
   colourValues[1] *= g;
   if(colourValues[1] > 1.0)
      colourValues[1] = 1.0;
   
   colourValues[2] *= b;
   if(colourValues[2] > 1.0)
      colourValues[2] = 1.0;
}

/*
 ==================================
 Colour::add
 ==================================
 */
void Colour::add(Colour other)
{
   colourValues[0] += other.getR();
   if(colourValues[0] > 1.0)
      colourValues[0] = 1.0;
   
   colourValues[1] += other.getG();
   if(colourValues[1] > 1.0)
      colourValues[1] = 1.0;
   
   colourValues[2] += other.getB();
   if(colourValues[2] > 1.0)
      colourValues[2] = 1.0;
}

/*
 ==================================
 Colour::add
 ==================================
 */
void Colour::add(float numberToAdd)
{
   colourValues[0] += numberToAdd;
   if(colourValues[0] > 1.0)
      colourValues[0] = 1.0;
   
   colourValues[1] += numberToAdd;
   if(colourValues[1] > 1.0)
      colourValues[1] = 1.0;
   
   colourValues[2] += numberToAdd;
   if(colourValues[2] > 1.0)
      colourValues[2] = 1.0;
}

/*
 ==================================
 Colour::add
 ==================================
 */
void Colour::add(float r, float g, float b)
{
   colourValues[0] += r;
   if(colourValues[0] > 1.0)
      colourValues[0] = 1.0;
   
   colourValues[1] += g;
   if(colourValues[1] > 1.0)
      colourValues[1] = 1.0;
   
   colourValues[2] += b;
   if(colourValues[2] > 1.0)
      colourValues[2] = 1.0;
}

/*
 ==================================
 Colour::subtract
 ==================================
 */
void Colour::subtract(Colour other)
{
   colourValues[0] -= other.getR();
   if(colourValues[0] < 0.0)
      colourValues[0] = 0.0;
   
   colourValues[1] -= other.getG();
   if(colourValues[1] < 0.0)
      colourValues[1] = 0.0;
   
   colourValues[2] -= other.getB();
   if(colourValues[2] < 0.0)
      colourValues[2] = 0.0;
}

/*
 ==================================
 Colour::subtract
 ==================================
 */
void Colour::subtract(float numberToSubtract)
{
   colourValues[0] -= numberToSubtract;
   if(colourValues[0] < 0.0)
      colourValues[0] = 0.0;
   
   colourValues[1] -= numberToSubtract;
   if(colourValues[1] < 0.0)
      colourValues[1] = 0.0;
   
   colourValues[2] -= numberToSubtract;
   if(colourValues[2] < 0.0)
      colourValues[2] = 0.0;
}

/*
 ==================================
 Colour::subtract
 ==================================
 */
void Colour::subtract(float r, float g, float b)
{
   colourValues[0] -= r;
   if(colourValues[0] < 0.0)
      colourValues[0] = 0.0;
   
   colourValues[1] -= g;
   if(colourValues[1] < 0.0)
      colourValues[1] = 0.0;
   
   colourValues[2] -= b;
   if(colourValues[2] < 0.0)
      colourValues[2] = 0.0;
}

/*
 ==================================
 Colour::~Colour
 ==================================
 */
Colour::~Colour()
{
}

// Getters
float Colour::getR()
{
   return colourValues[0];
}
float Colour::getG()
{
   return colourValues[1];
}
float Colour::getB()
{
   return colourValues[2];
}
float* Colour::getRGB()
{
   return colourValues;
}

// Setters
void Colour::setR(float r)
{
   colourValues[0] = r;
}
void Colour::setG(float g)
{
   colourValues[1] = g;
}
void Colour::setB(float b)
{
   colourValues[2] = b;
}
void Colour::setRGB(float r, float g, float b)
{
   colourValues[0] = r;
   colourValues[1] = g;
   colourValues[2] = b;
}
