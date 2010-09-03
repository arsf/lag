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
 * File: BoxOverlay.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: June-July 2010
 *
 * */
#include <gtkmm.h>
#include <vector>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "BoxOverlay.h"
using namespace std;

BoxOverlay::
BoxOverlay(Gtk::Label *label, double* majorcolour, double* minorcolour){
   this->label = label;
   orthogonalshape = false;
   slantedshape = true;
   slantwidth=30;
   ps = 0;
   xs = NULL;
   ys = NULL;
   zoomlevel = 1;
   ratio = 1;
   this->majorcolour = majorcolour;
   this->minorcolour = minorcolour;
   directional = false;
   for(int i = 0;i < 3;i++)
      if(majorcolour[i] != minorcolour[i])
         // If there is a distinct minor colour then the box is presumably 
         // supposed to be directional.
         directional = true;

   startx = starty = endx = endy = centrex = centrey = 0;
}

BoxOverlay::~BoxOverlay(){
   if(xs!=NULL)
      delete[]xs;
   if(ys!=NULL)
      delete[]ys;
   if(majorcolour!=NULL)
      delete[]majorcolour;
   if(minorcolour!=NULL)
      delete[]minorcolour;
}

// At the beginning, defines the start point and, for the moment, the end point.
void BoxOverlay::
on_start(double x, double y, double areawidth, double areaheight) { 
   // 0 is set to the centre before scaling and adding the centre coordinate.
   startx = endx = centrex + (x - areawidth/2)*ratio/zoomlevel;
   starty = endy = centrey - (y - areaheight/2)*ratio/zoomlevel;
}

// Updates the end point and then gets the vertical and horizontal differences 
// between the start and end points.
void BoxOverlay::
on_(double x,double y, double areawidth,double areaheight){
   //0 is set to the centre before scaling and adding the centre coordinate.
   endx = centrex + (x - areawidth/2)*ratio/zoomlevel;
   endy = centrey - (y - areaheight/2)*ratio/zoomlevel;
   drawinfo();
}

// Moves the box differently depending on slantedness and directionality 
// using keyboard input.
bool BoxOverlay::
on_key(GdkEventKey* event,double scrollspeed,bool fractionalshift){
   // If the user wants to scroll by a constant amount based on the thickness 
   // of the overlay rather than a variable amount based on the zoomlevel:
   if(fractionalshift){
      if(slantedshape)scrollspeed *= slantwidth;
      else scrollspeed *= sqrt((xs[0] - xs[1])*(xs[0] - xs[1]) + 
                               (ys[0] - ys[1])*(ys[0] - ys[1]));
   }
   double sameaxis = scrollspeed,diffaxis = 0;
   //If the overlays is slanted and there is a differently coloured line:
   if(directional && slantedshape) {
      double breadth = endx - startx;
      double height = endy - starty;
      //Right triangle.
      double length = sqrt(breadth*breadth+height*height);
      // Movement should be in terms of the differently coloured line, "up" 
      // meaning forwards towards the parralel line with the majority colour.
      sameaxis = scrollspeed*breadth/length;
      diffaxis = -scrollspeed*height/length;
   }
   switch(event->keyval){
      case GDK_W:
         startx += diffaxis;
         starty += sameaxis;
         endx += diffaxis;
         endy += sameaxis;
         break;//Up or "forward".
      case GDK_S:
         startx -= diffaxis;
         starty -= sameaxis;
         endx -= diffaxis;
         endy -= sameaxis;
         break;//Down or "backward".
      case GDK_A:
         starty += diffaxis;
         startx -= sameaxis;
         endy += diffaxis;
         endx -= sameaxis;
         break;//Left or "left".
      case GDK_D:
         starty -= diffaxis;
         startx += sameaxis;
         endy -= diffaxis;
         endx += sameaxis;
         break;//Right or "right".
      default:
         return false;
         break;
   }
   drawinfo();
   return true;
}
// Calculate the boundaries based on whether or not the box is orthogonal or 
// slanted and the start and end points of the user's clicks and drags.
void BoxOverlay::
makeboundaries(){
   //Four corners/points.
   ps = 4;
   if(xs!=NULL)delete[]xs;
   if(ys!=NULL)delete[]ys;
   xs = new double[4];
   ys = new double[4];

   if(slantedshape){
      double breadth = endx - startx;
      double height = endy - starty;
      double length = sqrt(breadth*breadth+height*height);//Right triangle.
      // The slantwidth is multiplied by the "opposite" ratio (i.e. height for 
      // x etc.) because the slantwidth is really a perpendicular distance from
      // the (ends of the) line to the corners:
      xs[0] = startx-(slantwidth/2)*height/length;//Far left 
      // (where the differently coloured line is the near 
      // clipping plane if a profile).
      xs[1] = startx+(slantwidth/2)*height/length;//Near left.
      xs[2] = endx+(slantwidth/2)*height/length;//Near right.
      xs[3] = endx-(slantwidth/2)*height/length;//Far right.
      ys[0] = starty+(slantwidth/2)*breadth/length;//Far left.
      ys[1] = starty-(slantwidth/2)*breadth/length;//Near left.
      ys[2] = endy-(slantwidth/2)*breadth/length;//Near right.
      ys[3] = endy+(slantwidth/2)*breadth/length;//Far right.
   }
   else if(orthogonalshape){
      //If the width is greater than the height:
      if(abs(startx - endx) > abs(starty - endy)){
         // Then place start and end points on the vertical (different in y) 
         // sides, so that the view is along the y axis if this is a profile.
         xs[0] = startx;
         xs[1] = startx;
         xs[2] = endx;
         xs[3] = endx;
         ys[0] = starty;
         ys[1] = endy;
         ys[2] = endy;
         ys[3] = starty;
      }
      else{
         // Otherwise place start and end points on the horizontal (different 
         // in x) sides, so that the view is along the x axis if this is a 
         // profile.
         xs[0] = startx;
         xs[1] = endx;
         xs[2] = endx;
         xs[3] = startx;
         ys[0] = starty;
         ys[1] = starty;
         ys[2] = endy;
         ys[3] = endy;
      }
   }
}

//This prints information about the coordinates of the box to the associated label.
void BoxOverlay::drawinfo(){
   if(orthogonalshape){
      double minx = startx,maxx = endx,miny = starty,maxy = endy;
      if(startx>endx){
         minx = endx;
         maxx = startx;
      }
      if(starty>endy){
         miny = endy;
         maxy = starty;
      }
      ostringstream minX,maxX,minY,maxY;
      minX << minx;
      maxX << maxx;
      minY << miny;
      maxY << maxy;
      // This is to ensure that the label's height never differs from three 
      // character lines, as otherwise it will sometimes change height which 
      // will cause the viewport to be updated and, therefore, the image to be 
      // cleared, which plays havoc with drawbuckets().
      string text = "MinX: " + minX.str() + " MaxX: " + maxX.str() + 
                  "\nMinY: " + minY.str() + " MaxY: " + maxY.str() + 
                  "\n-----";
      label->set_text(text);
   }
   else if(slantedshape){
      ostringstream StartX,StartY,EndX,EndY,Width;
      StartX << startx;
      StartY << starty;
      EndX << endx;
      EndY << endy;
      Width << slantwidth;
      // This is to ensure that the label's height never differs from three 
      // character lines, as otherwise it will sometimes change height which 
      // will cause the viewport to be updated and, therefore, the image to 
      // be cleared, which plays havoc with drawbuckets().
      string text = "Start = (" + StartX.str() + "," + StartY.str() + 
                    ") \nEnd = (" + EndX.str() + "," + EndY.str() + 
                    ") \nWidth = " + Width.str();
      label->set_text(text);
   }
}

// This makes the box with one line of it possibly in a different colour to 
// indicate, for example, the near clipping plane of a profile view.
void BoxOverlay::
makebox(double rmaxz){
   //This makes sure the overlay box is drawn on top of the flightlines.
   double altitude = rmaxz+1000;
   if(slantedshape){
      double breadth = endx - startx;
      double height = endy - starty;
      double length = sqrt(breadth*breadth+height*height);//Right triangle.
      
      //Prevent division by zero in case of careless user.
      if(length==0)
         length=1;
      
      glBegin(GL_LINE_LOOP);

         // The slantwidth is multiplied by the "opposite" ratio (i.e. height 
         // for x etc.) because the slantwidth is really a perpendicular 
         // distance from the (ends of the) line to the corners:

         glColor3dv(majorcolour);
            // Far left (where the differently coloured line is the near 
            // clipping plane if a profile).
            glVertex3d(startx-(slantwidth/2)*height/length-centrex,
                       starty+(slantwidth/2)*breadth/length-centrey,altitude);
            // Near left
            glVertex3d(startx+(slantwidth/2)*height/length-centrex,
                       starty-(slantwidth/2)*breadth/length-centrey,altitude);
         // If this is a profile then this line segment indicates the near 
         // clipping plane of the profile view.
         glColor3dv(minorcolour);
            // Near left
            glVertex3d(startx+(slantwidth/2)*height/length-centrex,
                       starty-(slantwidth/2)*breadth/length-centrey,altitude);
            // Near right
            glVertex3d(endx+(slantwidth/2)*height/length-centrex,
                       endy-(slantwidth/2)*breadth/length-centrey,altitude);

         glColor3dv(majorcolour);
            // Near right
            glVertex3d(endx+(slantwidth/2)*height/length-centrex,
                       endy-(slantwidth/2)*breadth/length-centrey,altitude);
            // Far right
            glVertex3d(endx-(slantwidth/2)*height/length-centrex,
                       endy+(slantwidth/2)*breadth/length-centrey,altitude);
      glEnd();
   }

   else if(orthogonalshape){
      //If the width is greater than the height of the overlay:
      if(abs(startx - endx) > abs(starty - endy)){
         //If the gradient from start to the end is negative:
         if((endy < starty && startx < endx)||
            (endy > starty && startx > endx)){
            glBegin(GL_LINE_LOOP);
               glColor3dv(majorcolour);
               glVertex3d(startx-centrex,starty-centrey,altitude);
               glVertex3d(startx-centrex,endy-centrey,altitude);
               //Draw the differently coloured line on the endy side.
               glColor3dv(minorcolour);
               glVertex3d(startx-centrex,endy-centrey,altitude);
               glVertex3d(endx-centrex,endy-centrey,altitude);
               glColor3dv(majorcolour);
               glVertex3d(endx-centrex,endy-centrey,altitude);
               glVertex3d(endx-centrex,starty-centrey,altitude);
            glEnd();
         }
         else{//If gradient positive, 0 or ∞:
            glBegin(GL_LINE_LOOP);
               glColor3dv(majorcolour);
               glVertex3d(startx-centrex,endy-centrey,altitude);
               glVertex3d(startx-centrex,starty-centrey,altitude);
               //Draw the diferently coloured line on the starty side.
               glColor3dv(minorcolour);
               glVertex3d(startx-centrex,starty-centrey,altitude);
               glVertex3d(endx-centrex,starty-centrey,altitude);
               glColor3dv(majorcolour);
               glVertex3d(endx-centrex,starty-centrey,altitude);
               glVertex3d(endx-centrex,endy-centrey,altitude);
            glEnd();
         }
      }
      else{
         //If the gradient from the start to the end is positive:
         if((endx < startx && starty > endy) ||
            (endx > startx && starty < endy)) {
            glBegin(GL_LINE_LOOP);
               glColor3dv(majorcolour);
               glVertex3d(startx-centrex,starty-centrey,altitude);
               glVertex3d(endx-centrex,starty-centrey,altitude);
               //Draw the differently coloured line on the endx side.
               glColor3dv(minorcolour);
               glVertex3d(endx-centrex,starty-centrey,altitude);
               glVertex3d(endx-centrex,endy-centrey,altitude);
               glColor3dv(majorcolour);
               glVertex3d(endx-centrex,endy-centrey,altitude);
               glVertex3d(startx-centrex,endy-centrey,altitude);
            glEnd();
         }
         else{//If gradient negative, 0 or ∞:
            glBegin(GL_LINE_LOOP);
               glColor3dv(majorcolour);
               glVertex3d(endx-centrex,starty-centrey,altitude);
               glVertex3d(startx-centrex,starty-centrey,altitude);
               //Draw the differently coloured line on the startx side.
               glColor3dv(minorcolour);
               glVertex3d(startx-centrex,starty-centrey,altitude);
               glVertex3d(startx-centrex,endy-centrey,altitude);
               glColor3dv(majorcolour);
               glVertex3d(startx-centrex,endy-centrey,altitude);
               glVertex3d(endx-centrex,endy-centrey,altitude);
            glEnd();
         }
      }
   }
}
