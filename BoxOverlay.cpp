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

BoxOverlay::BoxOverlay(Gtk::Label *label,double* majorcolour,double* minorcolour){
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
   for(int i = 0;i < 3;i++)if(majorcolour[i] != minorcolour[i])directional = true;
   startx = starty = endx = endy = centrex = centrey = 0;
}
BoxOverlay::~BoxOverlay(){
   if(xs!=NULL)delete[]xs;
   if(ys!=NULL)delete[]ys;
   if(majorcolour!=NULL)delete[]majorcolour;
   if(minorcolour!=NULL)delete[]minorcolour;
}

//At the beginning, defines the start point and, for the moment, the end point.
void BoxOverlay::on_start(double x,double y,double width,double height){
   startx = endx = centrex + (x - width/2)*ratio/zoomlevel;
   starty = endy = centrey - (y - height/2)*ratio/zoomlevel;
}
//Updates the end point and then gets the vertical and horisontal differences between the start and end points.
void BoxOverlay::on_(double x,double y, double width,double height){
   endx = centrex + (x - width/2)*ratio/zoomlevel;
   endy = centrey - (y - height/2)*ratio/zoomlevel;
   drawinfo();
}
bool BoxOverlay::on_key(GdkEventKey* event,double scrollspeed,bool fractionalshift){
   if(fractionalshift){
      if(slantedshape)scrollspeed *= slantwidth;
      else scrollspeed *= sqrt((xs[0] - xs[1])*(xs[0] - xs[1]) + (ys[0] - ys[1])*(ys[0] - ys[1]));
   }
   double sameaxis = scrollspeed,diffaxis = 0;
   if(directional && slantedshape){
      double breadth = endx - startx;
      double height = endy - starty;
      double length = sqrt(breadth*breadth+height*height);//Right triangle.
      sameaxis = scrollspeed*breadth/length;
      diffaxis = -scrollspeed*height/length;
   }
   switch(event->keyval){
      case GDK_W:startx += diffaxis;starty += sameaxis;endx += diffaxis;endy += sameaxis;break;
      case GDK_S:startx -= diffaxis;starty -= sameaxis;endx -= diffaxis;endy -= sameaxis;break;
      case GDK_A:starty += diffaxis;startx -= sameaxis;endy += diffaxis;endx -= sameaxis;break;
      case GDK_D:starty -= diffaxis;startx += sameaxis;endy -= diffaxis;endx += sameaxis;break;
      default:return false;break;
   }
   drawinfo();
   return true;
}
//Calculate the boundaries based on whether or not the box is orthogonal or slanted and the start and end points of the user's clicks and drags.
void BoxOverlay::makeboundaries(){
   ps = 4;
   if(xs!=NULL)delete[]xs;
   if(ys!=NULL)delete[]ys;
   xs = new double[4];
   ys = new double[4];
   if(slantedshape){
      double breadth = endx - startx;
      double height = endy - starty;
      double length = sqrt(breadth*breadth+height*height);//Right triangle.
      xs[0] = startx-(slantwidth/2)*height/length;
      xs[1] = startx+(slantwidth/2)*height/length;
      xs[2] = endx+(slantwidth/2)*height/length;
      xs[3] = endx-(slantwidth/2)*height/length;
      ys[0] = starty+(slantwidth/2)*breadth/length;
      ys[1] = starty-(slantwidth/2)*breadth/length;
      ys[2] = endy-(slantwidth/2)*breadth/length;
      ys[3] = endy+(slantwidth/2)*breadth/length;
   }
   else if(orthogonalshape){
      if(abs(startx - endx) > abs(starty - endy)){//If the width is greater than the height:
         xs[0] = startx;//Then place start and end points on the vertical (different in x) sides, so that the view is along the y axis if this is a profile.
         xs[1] = startx;//...
         xs[2] = endx;//...
         xs[3] = endx;//...
         ys[0] = starty;//...
         ys[1] = endy;//...
         ys[2] = endy;//...
         ys[3] = starty;//...
      }
      else{//Otherwise:
         xs[0] = startx;//Then place start and end points on the horizontal (different in y) sides, so that the view is along the x axis if this is a profile.
         xs[1] = endx;//...
         xs[2] = endx;//...
         xs[3] = startx;//...
         ys[0] = starty;//...
         ys[1] = starty;//...
         ys[2] = endy;//...
         ys[3] = endy;//...
      }
   }
}
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
      string text = "MinX: " + minX.str() + " MaxX: " + maxX.str() + "\nMinY: " + minY.str() + " MaxY: " + maxY.str() + "\n-----";//This is to ensure that the label's height never differs from three character lines, as otherwise it will sometimes change height which will cause the viewport to be updated and, therefore, the image to be cleared, which plays havoc with drawbuckets().
      label->set_text(text);
   }
   else if(slantedshape){
      ostringstream StartX,StartY,EndX,EndY,Width;
      StartX << startx;
      StartY << starty;
      EndX << endx;
      EndY << endy;
      Width << slantwidth;
      string text = "Start = (" + StartX.str() + "," + StartY.str() + ") \nEnd = (" + EndX.str() + "," + EndY.str() + ") \nWidth = " + Width.str();//This is to ensure that the label's height never differs from three character lines, as otherwise it will sometimes change height which will cause the viewport to be updated and, therefore, the image to be cleared, which plays havoc with drawbuckets().
      label->set_text(text);
   }
}
//This makes the box. It calculates the ratio between the length its x and y dimensions. It then draws the rectangle.
void BoxOverlay::makebox(double rmaxz){
   double altitude = rmaxz+1000;//This makes sure the ile box is drawn on top of the flightlines.
   if(slantedshape){
      double breadth = endx - startx;
      double height = endy - starty;
      double length = sqrt(breadth*breadth+height*height);//Right triangle.
      if(length==0)length=1;
      glBegin(GL_LINE_LOOP);
         glColor3dv(majorcolour);
         glVertex3d(startx-(slantwidth/2)*height/length-centrex,starty+(slantwidth/2)*breadth/length-centrey,altitude);
         glVertex3d(startx+(slantwidth/2)*height/length-centrex,starty-(slantwidth/2)*breadth/length-centrey,altitude);
         glColor3dv(minorcolour);
         glVertex3d(startx+(slantwidth/2)*height/length-centrex,starty-(slantwidth/2)*breadth/length-centrey,altitude);
         glVertex3d(endx+(slantwidth/2)*height/length-centrex,endy-(slantwidth/2)*breadth/length-centrey,altitude);
         glColor3dv(majorcolour);
         glVertex3d(endx+(slantwidth/2)*height/length-centrex,endy-(slantwidth/2)*breadth/length-centrey,altitude);
         glVertex3d(endx-(slantwidth/2)*height/length-centrex,endy+(slantwidth/2)*breadth/length-centrey,altitude);
      glEnd();
   }
   else if(orthogonalshape){
      if(abs(startx - endx) > abs(starty - endy)){//If the width is greater than the height of the ile:
         if((endy<starty&&startx<endx)||(endy>starty&&startx>endx)){
            glBegin(GL_LINE_LOOP);
               glColor3dv(majorcolour);
               glVertex3d(startx-centrex,starty-centrey,altitude);
               glVertex3d(startx-centrex,endy-centrey,altitude);
               glColor3dv(minorcolour);
               glVertex3d(startx-centrex,endy-centrey,altitude);
               glVertex3d(endx-centrex,endy-centrey,altitude);
               glColor3dv(majorcolour);
               glVertex3d(endx-centrex,endy-centrey,altitude);
               glVertex3d(endx-centrex,starty-centrey,altitude);
            glEnd();
         }
         else{
            glBegin(GL_LINE_LOOP);
               glColor3dv(majorcolour);
               glVertex3d(startx-centrex,endy-centrey,altitude);
               glVertex3d(startx-centrex,starty-centrey,altitude);
               glColor3dv(minorcolour);
               glVertex3d(startx-centrex,starty-centrey,altitude);
               glVertex3d(endx-centrex,starty-centrey,altitude);
               glColor3dv(majorcolour);
               glVertex3d(endx-centrex,starty-centrey,altitude);
               glVertex3d(endx-centrex,endy-centrey,altitude);
            glEnd();
         }
      }
      else{//Otherwise:
         if((endx<startx&&starty>endy)||(endx>startx&&starty<endy)){
            glBegin(GL_LINE_LOOP);
               glColor3dv(majorcolour);
               glVertex3d(startx-centrex,starty-centrey,altitude);
               glVertex3d(endx-centrex,starty-centrey,altitude);
               glColor3dv(minorcolour);
               glVertex3d(endx-centrex,starty-centrey,altitude);
               glVertex3d(endx-centrex,endy-centrey,altitude);
               glColor3dv(majorcolour);
               glVertex3d(endx-centrex,endy-centrey,altitude);
               glVertex3d(startx-centrex,endy-centrey,altitude);
            glEnd();
         }
         else{
            glBegin(GL_LINE_LOOP);
               glColor3dv(majorcolour);
               glVertex3d(endx-centrex,starty-centrey,altitude);
               glVertex3d(startx-centrex,starty-centrey,altitude);
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
