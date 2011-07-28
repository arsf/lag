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
 * File: MathFuncs.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: December 2009 - July 2010
 *
 * */
#include "MathFuncs.h"

// Returns the value of the given percentile of a dataset. Makes a histogram 
// of the dataset and goes along it until it gets a total count corresponding 
// with the percentile.
double percentilevalue(double* data, int datasize, double percentile, 
                       double minval, double maxval){
   unsigned long int* histogram = new unsigned long int [
                                 (unsigned long int) floor(maxval - minval) +
                                  1];
   //Fill histogram with zeroes.                                                        
   for (unsigned int i = 0; 
        i < (unsigned long int) floor(maxval - minval); 
        i++)
      histogram[i] = 0;

   //Increment histogram indeices to give it "shape".
   for (int i = 0; i < datasize; i++)
      histogram[(unsigned long int) floor(data[i] - minval)]++;

   int temp = 0, index = 0;
   do{//Determine value of percentile.
      temp += histogram[index];
      index++;
   } while(temp < (percentile * (double) datasize) / 100.0);

   double percval = index + minval;
   delete[] histogram;
   return percval;
}

//Determines whether the points in the sent bucket fit within the profile area.
//
//If "hide noise points in profile" is ticket in advanced options, these are not sent to the
//profile
bool* vetpoints(PointBucket* points, double* xs, double* ys, 
                int numberofcorners, bool hideNoise){
   bool* correctpoints = new bool[points->getNumberOfPoints(0)];
   //Determines whether the point is within the boundary.
   bool pointinboundary;
   //These define the edge being considered.
   int lastcorner,currentcorner;
   //For every point:
   for(int i=0; i < points->getNumberOfPoints(0); i++) {
      // Zero is an even number, so if the point is to the right of an edge 
      // of the boundary zero times, it cannot be within it.
      pointinboundary = false;
      //Initially the last corner is looped back.
      lastcorner = numberofcorners - 1;
      //For every edge:
      for(currentcorner = 0;currentcorner < numberofcorners; currentcorner++){
         // This segments the line to the length of the segment that 
         // helps define the boundary.
         if((ys[currentcorner] < points->getPoint(i,0).getY() && 
             ys[lastcorner] >= points->getPoint(i,0).getY()) ||
            (ys[lastcorner] < points->getPoint(i,0).getY() && 
             ys[currentcorner] >= points->getPoint(i,0).getY())){
            // If the point is to the right of the line defined by the corners 
            // (and segmented by the above if statement), i.e. the edge:
            if(xs[currentcorner] + 
               ((points->getPoint(i,0).getY() - ys[currentcorner]) /
               (ys[lastcorner] - ys[currentcorner])) * 
               (xs[lastcorner] - xs[currentcorner]) < 
               points->getPoint(i,0).getX()) {
               // If done an odd number of times, the point must be within the 
               // shape, otherwise without.
               pointinboundary = !pointinboundary;
            }
         }
         lastcorner = currentcorner;
     }
     if (hideNoise == true && points->getPoint(i, 0).getClassification() == 7)
        pointinboundary = false;
     correctpoints[i] = pointinboundary;
   }
   return correctpoints;
}
