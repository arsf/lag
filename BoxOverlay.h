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
   void makebox(double rmaxz);//Make the box with one line possibly in a different colour..
   void on_start(double x,double y,double areawidth,double areaheight);//Defines the start point and, initially, the end point.
   void on_(double x,double y, double areawidth,double areaheight);//Defines the end point.
   bool on_key(GdkEventKey* event,double scrollspeed,bool fractionalshift);//Moves the box using keyboard input, with different methods depending on slantedness and directionality.
   void drawinfo();//Outputs coordinate information to the label.
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
   bool directional;//This is true if there is a minor colour and means that keyboard movement will be forward and backwards and so on respective to looking into the box from the line in the minor colour.
   bool orthogonalshape;//Determines whether or not to draw an orthogonal box.
   bool slantedshape;//Determines whether or not to draw a slanted box.
   Gtk::Label *label;//Label showing the distance, in various dimensions, covered by the ruler.
   double* majorcolour;//This is the colour of three of the four lines that make up the box.
   double* minorcolour;//This is the colour of the line that, for a profile, represents the near clipping plane  and indicates where the profile is used from.
   //These are returned to other objects to define the box.
   double* xs;//Store the boundaries of the profile:
   double* ys;//...
   int ps;//Stores the number of corners for the profile.
   //Position variables:
   double centrex,centrey;//These give the centre of the viewport in image terms, rather than screen terms.
   double slantwidth;//The width of the slanted box in world units.
   double startx, starty;//The start coordinates in world units.
   double endx, endy;//The end coordinates in world units.
   double ratio;//This stores the ratio for translating to and from screen scale and world scale. It should be set to be the same as in the parent Display area.
   double zoomlevel;//Like the ratio, it should be the same as in the parent Display area. It modifies the ratio.
};
#endif
