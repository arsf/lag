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
#include "BoxOverlay.h"
using namespace std;

BoxOverlay::
BoxOverlay(Gtk::Label *label, Colour majorcolour, Colour minorcolour){
   this->label = label;
   orthogonalshape = false;
   slantedshape = true;
   slantwidth=30;
   zoomlevel = 1;
   ratio = 1;
   _majorcolour = majorcolour;
   _minorcolour = minorcolour;
   directional = false;
   for(int i = 0;i < 3;i++)
      if(majorcolour.getRGB()[i] != minorcolour.getRGB()[i])
         // If there is a distinct minor colour then the box is presumably 
         // supposed to be directional.
         directional = true;
}

BoxOverlay::~BoxOverlay(){
}

// At the beginning, defines the start point and, for the moment, the end point.
void BoxOverlay::
on_start(Point startPoint, double areawidth, double areaheight) { 
   // 0 is set to the centre before scaling and adding the centre coordinate.
   _end.move(_centre.getX() + ((startPoint.getX() - areawidth/2)*ratio/zoomlevel),
             _centre.getY() - ((startPoint.getY() - areaheight/2)*ratio/zoomlevel),
             0);

   _start = _end;
}

// Updates the end point and then gets the vertical and horizontal differences 
// between the start and end points.
void BoxOverlay::
on_(Point endPoint, double areawidth,double areaheight){
   //0 is set to the centre before scaling and adding the centre coordinate.
   _end.move(_centre.getX() + ((endPoint.getX() - areawidth/2)*ratio/zoomlevel),
             _centre.getY() - ((endPoint.getY() - areaheight/2)*ratio/zoomlevel),
             0);
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
      else scrollspeed *= _theBox.getCorners()[0].distanceTo(_theBox.getCorners()[1]);
   }
   double sameaxis = scrollspeed,diffaxis = 0;
   //If the overlays is slanted and there is a differently coloured line:
   if(directional && slantedshape) {
      double breadth = _end.getX() - _start.getX();
      double height = _end.getY() - _start.getY();
      //Right triangle.
      double length = _start.distanceTo(_end); 
      // Movement should be in terms of the differently coloured line, "up" 
      // meaning forwards towards the parralel line with the majority colour.
      sameaxis = scrollspeed*breadth/length;
      diffaxis = -scrollspeed*height/length;
   }
   switch(event->keyval){
      case GDK_W:
         _start.translate(diffaxis, sameaxis, 0);
         _end.translate(diffaxis, sameaxis, 0);
         break;//Up or "forward".
      case GDK_S:
         _start.translate(-diffaxis, -sameaxis, 0);
         _end.translate(-diffaxis, -sameaxis, 0);
         break;//Down or "backward".
      case GDK_A:
         _start.translate(diffaxis, -sameaxis, 0);
         _end.translate(diffaxis, -sameaxis, 0);
         break;//Left or "left".
      case GDK_D:
         _start.translate(-diffaxis, sameaxis, 0);
         _end.translate(-diffaxis, sameaxis, 0);
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
   if(slantedshape){
      double breadth = _end.getX() - _start.getX();
      double height = _end.getY() - _start.getY();
      double length = _start.distanceTo(_end); 
      // The slantwidth is multiplied by the "opposite" ratio (i.e. height for 
      // x etc.) because the slantwidth is really a perpendicular distance from
      // the (ends of the) line to the _corners:
      // (where the differently coloured line is the near 
      // clipping plane if a profile).
      _theBox = SelectionBox( _start.getX()-(slantwidth/2)*height/length,
                              _start.getY()+(slantwidth/2)*breadth/length, 0.0,
                              _start.getX()+(slantwidth/2)*height/length,
                              _start.getY()-(slantwidth/2)*breadth/length, 0.0,
                              _end.getX()+(slantwidth/2)*height/length,
                              _end.getY()-(slantwidth/2)*breadth/length, 0.0,
                              _end.getX()-(slantwidth/2)*height/length,
                              _end.getY()+(slantwidth/2)*breadth/length, 0.0);
               
   }
   else if(orthogonalshape){
      //If the width is greater than the height:
      if(abs(_start.getX() - _end.getX()) > abs(_start.getY() - _end.getY())){
         // Then place start and end points on the vertical (different in y) 
         // sides, so that the view is along the y axis if this is a profile.
         //
         _theBox = SelectionBox( _start.getX(), _start.getY(), 0,
                                 _start.getX(), _end.getY(), 0,
                                 _end.getX(), _end.getY(), 0,
                                 _end.getX(), _start.getY(), 0);
      }
      else{
         // Otherwise place start and end points on the horizontal (different 
         // in x) sides, so that the view is along the x axis if this is a 
         // profile.
         _theBox = SelectionBox ( _start.getX(), _start.getY(), 0,
                                  _end.getX(), _start.getY(), 0,
                                  _end.getX(), _end.getY(), 0,
                                  _start.getX(), _end.getY(), 0);
      }
   }
}

//This prints information about the coordinates of the box to the associated label.
//
void BoxOverlay::drawinfo(){
   if(orthogonalshape){
      double minx = _start.getX(),maxx = _end.getX(),miny = _start.getY(),maxy = _end.getY();
      Point min = _start, max = _end;

      if(_start.getX()>_end.getX()){
         min.move(_end.getX(), min.getY(), 0);
         max.move(_start.getX(), max.getY(), 0);
      }
      if(_start.getY()>_end.getY()){
         min.move(min.getX(), _end.getY(), 0);
         max.move(max.getX(), _start.getY(), 0);
      }
      ostringstream minX,maxX,minY,maxY;
      minX << min.getX();
      maxX << max.getX();
      minY << min.getY();
      maxY << max.getY();
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
      StartX << _start.getX();
      StartY << _start.getY();
      EndX << _end.getX();
      EndY << _end.getY();
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
      double breadth = _end.getX() - _start.getX();
      double height = _end.getY() - _start.getY();
      double length = _start.distanceTo(_end);//sqrt(breadth*breadth+height*height);//Right triangle.
      
      //Prevent division by zero in case of careless user.
      if(length==0)
         length=1;
      
      glBegin(GL_LINE_LOOP);

         // The slantwidth is multiplied by the "opposite" ratio (i.e. height 
         // for x etc.) because the slantwidth is really a perpendicular 
         // distance from the (ends of the) line to the _corners:

         glColor3fv(_majorcolour.getRGB());
            // Far left (where the differently coloured line is the near 
            // clipping plane if a profile).
            glVertex3d(_start.getX()-(slantwidth/2)*height/length-_centre.getX(),
                       _start.getY()+(slantwidth/2)*breadth/length-_centre.getY(),altitude);
            // Near left
            glVertex3d(_start.getX()+(slantwidth/2)*height/length-_centre.getX(),
                       _start.getY()-(slantwidth/2)*breadth/length-_centre.getY(),altitude);
         // If this is a profile then this line segment indicates the near 
         // clipping plane of the profile view.
         glColor3fv(_minorcolour.getRGB());
            // Near left
            glVertex3d(_start.getX()+(slantwidth/2)*height/length-_centre.getX(),
                       _start.getY()-(slantwidth/2)*breadth/length-_centre.getY(),altitude);
            // Near right
            glVertex3d(_end.getX()+(slantwidth/2)*height/length-_centre.getX(),
                       _end.getY()-(slantwidth/2)*breadth/length-_centre.getY(),altitude);

         glColor3fv(_majorcolour.getRGB());
            // Near right
            glVertex3d(_end.getX()+(slantwidth/2)*height/length-_centre.getX(),
                       _end.getY()-(slantwidth/2)*breadth/length-_centre.getY(),altitude);
            // Far right
            glVertex3d(_end.getX()-(slantwidth/2)*height/length-_centre.getX(),
                       _end.getY()+(slantwidth/2)*breadth/length-_centre.getY(),altitude);
      glEnd();
   }

   else if(orthogonalshape){
      //If the width is greater than the height of the overlay:
      if(abs(_start.getX() - _end.getX()) > abs(_start.getY() - _end.getY())){
         //If the gradient from start to the end is negative:
         if((_end.getY() < _start.getY() && _start.getX() < _end.getX())||
            (_end.getY() > _start.getY() && _start.getX() > _end.getX())){
            glBegin(GL_LINE_LOOP);
               glColor3fv(_majorcolour.getRGB());
               glVertex3d(_start.getX()-_centre.getX(),_start.getY()-_centre.getY(),altitude);
               glVertex3d(_start.getX()-_centre.getX(),_end.getY()-_centre.getY(),altitude);
               //Draw the differently coloured line on the _end.getY() side.
               glColor3fv(_minorcolour.getRGB());
               glVertex3d(_start.getX()-_centre.getX(),_end.getY()-_centre.getY(),altitude);
               glVertex3d(_end.getX()-_centre.getX(),_end.getY()-_centre.getY(),altitude);
               glColor3fv(_majorcolour.getRGB());
               glVertex3d(_end.getX()-_centre.getX(),_end.getY()-_centre.getY(),altitude);
               glVertex3d(_end.getX()-_centre.getX(),_start.getY()-_centre.getY(),altitude);
            glEnd();
         }
         else{//If gradient positive, 0 or ∞:
            glBegin(GL_LINE_LOOP);
               glColor3fv(_majorcolour.getRGB());
               glVertex3d(_start.getX()-_centre.getX(),_end.getY()-_centre.getY(),altitude);
               glVertex3d(_start.getX()-_centre.getX(),_start.getY()-_centre.getY(),altitude);
               //Draw the diferently coloured line on the _start.getY() side.
               glColor3fv(_minorcolour.getRGB());
               glVertex3d(_start.getX()-_centre.getX(),_start.getY()-_centre.getY(),altitude);
               glVertex3d(_end.getX()-_centre.getX(),_start.getY()-_centre.getY(),altitude);
               glColor3fv(_majorcolour.getRGB());
               glVertex3d(_end.getX()-_centre.getX(),_start.getY()-_centre.getY(),altitude);
               glVertex3d(_end.getX()-_centre.getX(),_end.getY()-_centre.getY(),altitude);
            glEnd();
         }
      }
      else{
         //If the gradient from the start to the end is positive:
         if((_end.getX() < _start.getX() && _start.getY() > _end.getY()) ||
            (_end.getX() > _start.getX() && _start.getY() < _end.getY())) {
            glBegin(GL_LINE_LOOP);
               glColor3fv(_majorcolour.getRGB());
               glVertex3d(_start.getX()-_centre.getX(),_start.getY()-_centre.getY(),altitude);
               glVertex3d(_end.getX()-_centre.getX(),_start.getY()-_centre.getY(),altitude);
               //Draw the differently coloured line on the _end.getX() side.
               glColor3fv(_minorcolour.getRGB());
               glVertex3d(_end.getX()-_centre.getX(),_start.getY()-_centre.getY(),altitude);
               glVertex3d(_end.getX()-_centre.getX(),_end.getY()-_centre.getY(),altitude);
               glColor3fv(_majorcolour.getRGB());
               glVertex3d(_end.getX()-_centre.getX(),_end.getY()-_centre.getY(),altitude);
               glVertex3d(_start.getX()-_centre.getX(),_end.getY()-_centre.getY(),altitude);
            glEnd();
         }
         else{//If gradient negative, 0 or ∞:
            glBegin(GL_LINE_LOOP);
               glColor3fv(_majorcolour.getRGB());
               glVertex3d(_end.getX()-_centre.getX(),_start.getY()-_centre.getY(),altitude);
               glVertex3d(_start.getX()-_centre.getX(),_start.getY()-_centre.getY(),altitude);
               //Draw the differently coloured line on the _start.getX() side.
               glColor3fv(_minorcolour.getRGB());
               glVertex3d(_start.getX()-_centre.getX(),_start.getY()-_centre.getY(),altitude);
               glVertex3d(_start.getX()-_centre.getX(),_end.getY()-_centre.getY(),altitude);
               glColor3fv(_majorcolour.getRGB());
               glVertex3d(_start.getX()-_centre.getX(),_end.getY()-_centre.getY(),altitude);
               glVertex3d(_end.getX()-_centre.getX(),_end.getY()-_centre.getY(),altitude);
            glEnd();
         }
      }
   }
}
