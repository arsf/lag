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
 * File: BoxOverlay.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: June-July 2010
 *
 * */
#include <iostream>
#include <gtkmm.h>
#include <vector>
#ifndef BOXOVERLAY_H
#define BOXOVERLAY_H
class BoxOverlay{
public:
   BoxOverlay(Gtk::Label *label,double* majorcolour,double* minorcolour);
   ~BoxOverlay();
   void makeboundaries();//Determine the boundaries from the start and end points of the user's clicks and drags and whether to be orthogonal or slanted.
   void makebox(double rmaxz);//Make the box.
   void on_start(double x,double y,double width,double height);
   void on_(double x,double y, double width,double height);
   bool on_key(GdkEventKey* event,double scrollspeed,bool fractionalshift);
   void drawinfo();
   //Getters:
   void getboundaries(double*& xs,double*& ys,int& ps){//Get coordinates for boundaries.
      xs = this->xs;
      ys = this->ys;
      ps = this->ps;
   }
   //Setters:
   void setslantwidth(double slantwidth){ this->slantwidth = slantwidth;}//Set width of the slanted box.
   void setorthogonalshape(bool orthogonalshape){ this->orthogonalshape = orthogonalshape; }
   void setslantedshape(bool slantedshape){ this->slantedshape = slantedshape; }
   void setratio(double ratio){this->ratio = ratio;}
   void setzoomlevel(double zoomlevel){this->zoomlevel = zoomlevel;}
   void setcentre(double centrex,double centrey){this->centrex = centrex; this->centrey = centrey;}

protected:
   bool directional;
   bool orthogonalshape;//Determines whether or not to draw an orthogonal box.
   bool slantedshape;//Determines whether or not to draw a slanted box.
   double* xs;//Store the boundaries of the profile:
   double* ys;//...
   int ps;//Stores the number of corners for the profile.
   Gtk::Label *label;//Label showing the distance, in various dimensions, covered by the ruler.
   //Position variables:
   double centrex,centrey;//These give the centre of the viewport in image terms, rather than screen terms.
   double slantwidth;//The width of the slanted box in world units.
   double startx, starty;//The start coordinates in world units.
   double endx, endy;//The end coordinates in world units.
   double ratio;
   double zoomlevel;
   double* majorcolour;
   double* minorcolour;
};
#endif
