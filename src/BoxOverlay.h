/*
===============================================================================

 BoxOverlay.h

 Created on: June-July 2010
 Authors: Haraldur Tristan Gunnarsson

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

#ifndef BOXOVERLAY_H
#define BOXOVERLAY_H
#include <iostream>

#include <gtkmm.h>
#include <vector>
#include "Colour.h"
#include "Point.h"
#include "SelectionBox.h"

/*
===============================================================================

 BoxOverlay - represents a rectangle on the screen.

===============================================================================
*/
class BoxOverlay
{
public:
   BoxOverlay(Gtk::Label *label, Colour majorcolour, Colour minorcolour);

   ~BoxOverlay();

   // Determine the boundaries from the start and end points of the user's 
   // clicks and drags and whether to be orthogonal or slanted.
   void makeboundaries();

   //Make the box with one line possibly in a different colour..
   void makebox(double rmaxz);

   //Defines the start point and, initially, the end point.
   void on_start(Point,double areawidth,double areaheight);

   //Defines the end point.
   void on_(Point, double areawidth,double areaheight);

   // Moves the box using keyboard input, with different methods depending 
   // on slantedness and directionality.
   bool on_key(GdkEventKey* event,double scrollspeed,bool fractionalshift);

   // Directly defines both start and end
   void directly_place(Point, Point, double areawidth, double areaheight);

   //Outputs coordinate information to the label.
   void drawinfo();
   
   //Getters:

   inline SelectionBox getSelectionBox()
   {
      return theBox;
   }

   //Setters:

   //Set width of the slanted box.
   inline void setslantwidth(double slantwidth)
   {
      this->slantwidth = slantwidth;
   }

   inline void setorthogonalshape(bool orthogonalshape)
   {
      this->orthogonalshape = orthogonalshape; 
   }

   inline void setslantedshape(bool slantedshape)
   {
      this->slantedshape = slantedshape; 
   }

   inline void setratio(double ratio)
   {
      this->ratio = ratio;
   }

   inline void setzoomlevel(double zoomlevel)
   {
      this->zoomlevel = zoomlevel;
   }

   inline void setcentre(Point c)
   {
      centre = c;
   }

private:
   // This is true if there is a minor colour and means that keyboard movement 
   // will be forward and backwards and so on respective to looking into the 
   // box from the line in the minor colour.
   bool directional;

   //Determines whether or not to draw an orthogonal box.
   bool orthogonalshape;

   //Determines whether or not to draw a slanted box.
   bool slantedshape;

   //Label showing the distance, in various dimensions, covered by 
   //the ruler.
   Gtk::Label *label;

   //This is the colour of three of the four lines that make up the box.
   Colour majorcolour;

   // This is the colour of the line that, for a profile, represents the 
   // near clipping plane  and indicates where the profile is used from.
   Colour minorcolour;

   //These are returned to other objects to define the box.
   //Store the boundaries of the profile:
   SelectionBox theBox;

   // Stores the number of corners for the profile.
   // Position variables:
   // These give the centre of the viewport in image terms, rather than 
   // screen terms.
   Point centre;

   //The width of the slanted box in world units.
   double slantwidth;

   //The start coordinates in world units.
   Point start;

   //The end coordinates in world units.
   Point end;

   // This stores the ratio for translating to and from screen scale and 
   // world scale. It should be set to be the same as in the parent Display 
   // area.
   double ratio;

   // Like the ratio, it should be the same as in the parent Display area. 
   // It modifies the ratio.
   double zoomlevel;
};
#endif
