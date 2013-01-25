/*
===============================================================================

 BoxOverlay.cpp

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

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include "BoxOverlay.h"

using namespace std;


/*
==================================
 BoxOverlay::BoxOverlay
==================================
*/
BoxOverlay::BoxOverlay(Gtk::Label *label, Colour majorc, Colour minorc)
:
		directional 	(false),
		orthogonalshape	(false),
		slantedshape	(true),
		label			(label),
		majorcolour		(majorc),
		minorcolour		(minorc),
		slantwidth		(30),
		ratio			(1),
		zoomlevel		(1)

{
	if (majorcolour != minorcolour)
		directional = true;
}

/*
==================================
 BoxOverlay::~BoxOverlay
==================================
*/
BoxOverlay::~BoxOverlay()
{
}

/*
==================================
 BoxOverlay::on_start

 At the beginning, defines the start point and, for the moment, the end point.
==================================
*/
void BoxOverlay::on_start(Point startPoint, double areawidth, double areaheight) 
{ 
   // 0 is set to the centre before scaling and adding the centre coordinate.
   end.move(centre.getX() + ((startPoint.getX() - areawidth/2)*ratio/zoomlevel),
             centre.getY() - ((startPoint.getY() - areaheight/2)*ratio/zoomlevel), 0);

   start = end;
}

/*
==================================
 BoxOverlay::on_

 Updates the end point and then gets the vertical and horizontal differences
 between the start and end points.
==================================
*/
void BoxOverlay::on_(Point endPoint, double areawidth, double areaheight)
{
   // 0 is set to the centre before scaling and adding the centre coordinate.
   end.move(centre.getX() + ((endPoint.getX() - areawidth/2)*ratio/zoomlevel),
             centre.getY() - ((endPoint.getY() - areaheight/2)*ratio/zoomlevel), 0);
   drawinfo();
}

/*
==================================
 BoxOverlay::on_key

 Moves the box differently depending on slantedness and directionality
 using keyboard input.
==================================
*/
bool BoxOverlay::on_key(GdkEventKey* event,double scrollspeed,bool fractionalshift)
{
   // If the user wants to scroll by a constant amount based on the thickness 
   // of the overlay rather than a variable amount based on the zoomlevel:
   if(fractionalshift)
   {
      if(slantedshape)scrollspeed *= slantwidth;
      else scrollspeed *= theBox.getCorners()[0].distanceTo(theBox.getCorners()[1]);
   }

   double sameaxis = scrollspeed, diffaxis = 0;

   //If the overlays is slanted and there is a differently coloured line:
   if(directional && slantedshape)
   {
      double breadth = end.getX() - start.getX();
      double height = end.getY() - start.getY();

      //Right triangle.
      double length = start.distanceTo(end); 

      // Movement should be in terms of the differently coloured line, "up" 
      // meaning forwards towards the parralel line with the majority colour.
      sameaxis = scrollspeed*breadth/length;
      diffaxis = -scrollspeed*height/length;
   }
   switch(event->keyval)
   {
      case GDK_W:
         start.translate(diffaxis, sameaxis, 0);
         end.translate(diffaxis, sameaxis, 0);
         break; //Up or "forward".
      case GDK_S:
         start.translate(-diffaxis, -sameaxis, 0);
         end.translate(-diffaxis, -sameaxis, 0);
         break; //Down or "backward".
      case GDK_A:
         start.translate(-sameaxis, diffaxis, 0);
         end.translate(-sameaxis, diffaxis, 0);
         break; //Left or "left".
      case GDK_D:
         start.translate(sameaxis, -diffaxis, 0);
         end.translate(sameaxis, -diffaxis, 0);
         break; //Right or "right".
      default:
         return false;
         break;
   }

   drawinfo();

   return true;
}

/*
==================================
 BoxOverlay::directly_place

 Places the box as specified at once
==================================
*/
void BoxOverlay::directly_place( Point start, Point end,
                     double areawidth, double areaheight)
{
   this->on_start(start, areawidth, areaheight);
   this->on_(end, areawidth, areaheight);
}

/*
==================================
 BoxOverlay::makeboundaries

 Calculate the boundaries based on whether or not the box is orthogonal or
 slanted and the start and end points of the user's clicks and drags.
==================================
*/
void BoxOverlay::makeboundaries()
{
   if(slantedshape)
   {
      double breadth = end.getX() - start.getX();
      double height = end.getY() - start.getY();
      double length = start.distanceTo(end);

      // The slantwidth is multiplied by the "opposite" ratio (i.e. height for 
      // x etc.) because the slantwidth is really a perpendicular distance from
      // the (ends of the) line to the corners:
      // (where the differently coloured line is the near 
      // clipping plane if a profile).
      theBox = SelectionBox( start.getX()-(slantwidth/2)*height/length,
                              start.getY()+(slantwidth/2)*breadth/length, 0.0,
                              start.getX()+(slantwidth/2)*height/length,
                              start.getY()-(slantwidth/2)*breadth/length, 0.0,
                              end.getX()+(slantwidth/2)*height/length,
                              end.getY()-(slantwidth/2)*breadth/length, 0.0,
                              end.getX()-(slantwidth/2)*height/length,
                              end.getY()+(slantwidth/2)*breadth/length, 0.0);
               
   }
   else if(orthogonalshape)
   {
      //If the width is greater than the height:
      if(abs(start.getX() - end.getX()) > abs(start.getY() - end.getY()))
      {
         // Then place start and end points on the vertical (different in y) 
         // sides, so that the view is along the y axis if this is a profile.
         theBox = SelectionBox( start.getX(), start.getY(), 0,
                                 start.getX(), end.getY(), 0,
                                 end.getX(), end.getY(), 0,
                                 end.getX(), start.getY(), 0);
      }
      else
      {
         // Otherwise place start and end points on the horizontal (different 
         // in x) sides, so that the view is along the x axis if this is a 
         // profile.
         theBox = SelectionBox ( start.getX(), start.getY(), 0,
                                  end.getX(), start.getY(), 0,
                                  end.getX(), end.getY(), 0,
                                  start.getX(), end.getY(), 0);
      }
   }
}

/*
==================================
 BoxOverlay::drawinfo

 This prints information about the coordinates of the box to the associated label.
==================================
*/
void BoxOverlay::drawinfo()
{
   if (label == NULL)
   {
      return;
   }

   else if (orthogonalshape)
   {
      Point min = start, max = end;

      if(start.getX()>end.getX())
      {
         min.move(end.getX(), min.getY(), 0);
         max.move(start.getX(), max.getY(), 0);
      }
      if(start.getY()>end.getY())
      {
         min.move(min.getX(), end.getY(), 0);
         max.move(max.getX(), start.getY(), 0);
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

   else if (slantedshape)
   {
      ostringstream StartX,StartY,EndX,EndY,Width;
      StartX << start.getX();
      StartY << start.getY();
      EndX << end.getX();
      EndY << end.getY();
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

/*
==================================
 BoxOverlay::makebox

 This makes the box with one line of it possibly in a different colour to
 indicate, for example, the near clipping plane of a profile view.
==================================
*/
void BoxOverlay::makebox(double rmaxz)
{
   //This makes sure the overlay box is drawn on top of the flightlines.
   double altitude = rmaxz+1000;
   if(slantedshape)
   {
      double breadth = end.getX() - start.getX();
      double height = end.getY() - start.getY();
      double length = start.distanceTo(end);//sqrt(breadth*breadth+height*height);//Right triangle.
      
      //Prevent division by zero in case of careless user.
      if(length==0)
         length=1;
      
      glBegin(GL_LINE_LOOP);

      // The slantwidth is multiplied by the "opposite" ratio (i.e. height
      // for x etc.) because the slantwidth is really a perpendicular
      // distance from the (ends of the) line to the corners:

      glColor3fv(majorcolour.getRGB());

      // Far left (where the differently coloured line is the near clipping plane if a profile).
      glVertex3d(start.getX()-(slantwidth/2)*height/length-centre.getX(),
    		  start.getY()+(slantwidth/2)*breadth/length-centre.getY(),altitude);

      // Near left
      glVertex3d(start.getX()+(slantwidth/2)*height/length-centre.getX(),
    		  start.getY()-(slantwidth/2)*breadth/length-centre.getY(),altitude);

      // If this is a profile then this line segment indicates the near clipping plane of the profile view.
      glColor3fv(minorcolour.getRGB());

      // Near left
      glVertex3d(start.getX()+(slantwidth/2)*height/length-centre.getX(),
    		  start.getY()-(slantwidth/2)*breadth/length-centre.getY(),altitude);

      // Near right
      glVertex3d(end.getX()+(slantwidth/2)*height/length-centre.getX(),
    		  end.getY()-(slantwidth/2)*breadth/length-centre.getY(),altitude);

      glColor3fv(majorcolour.getRGB());

      // Near right
      glVertex3d(end.getX()+(slantwidth/2)*height/length-centre.getX(),
    		  end.getY()-(slantwidth/2)*breadth/length-centre.getY(),altitude);

      // Far right
      glVertex3d(end.getX()-(slantwidth/2)*height/length-centre.getX(),
    		  end.getY()+(slantwidth/2)*breadth/length-centre.getY(),altitude);

      glEnd();
   }

   else if(orthogonalshape)
   {
      //If the width is greater than the height of the overlay:
      if(abs(start.getX() - end.getX()) > abs(start.getY() - end.getY()))
      {
         //If the gradient from start to the end is negative:
         if((end.getY() < start.getY() && start.getX() < end.getX())||(end.getY() > start.getY() && start.getX() > end.getX()))
         {
            glBegin(GL_LINE_LOOP);
               glColor3fv(majorcolour.getRGB());
               glVertex3d(start.getX()-centre.getX(),start.getY()-centre.getY(),altitude);
               glVertex3d(start.getX()-centre.getX(),end.getY()-centre.getY(),altitude);
               //Draw the differently coloured line on the end.getY() side.
               glColor3fv(minorcolour.getRGB());
               glVertex3d(start.getX()-centre.getX(),end.getY()-centre.getY(),altitude);
               glVertex3d(end.getX()-centre.getX(),end.getY()-centre.getY(),altitude);
               glColor3fv(majorcolour.getRGB());
               glVertex3d(end.getX()-centre.getX(),end.getY()-centre.getY(),altitude);
               glVertex3d(end.getX()-centre.getX(),start.getY()-centre.getY(),altitude);
            glEnd();
         }
         else
         {
        	//If gradient positive
            glBegin(GL_LINE_LOOP);
               glColor3fv(majorcolour.getRGB());
               glVertex3d(start.getX()-centre.getX(),end.getY()-centre.getY(),altitude);
               glVertex3d(start.getX()-centre.getX(),start.getY()-centre.getY(),altitude);
               //Draw the diferently coloured line on the start.getY() side.
               glColor3fv(minorcolour.getRGB());
               glVertex3d(start.getX()-centre.getX(),start.getY()-centre.getY(),altitude);
               glVertex3d(end.getX()-centre.getX(),start.getY()-centre.getY(),altitude);
               glColor3fv(majorcolour.getRGB());
               glVertex3d(end.getX()-centre.getX(),start.getY()-centre.getY(),altitude);
               glVertex3d(end.getX()-centre.getX(),end.getY()-centre.getY(),altitude);
            glEnd();
         }
      }
      else
      {
    	 //If the gradient from the start to the end is positive:
         if((end.getX() < start.getX() && start.getY() > end.getY()) || (end.getX() > start.getX() && start.getY() < end.getY()))
         {
            glBegin(GL_LINE_LOOP);
               glColor3fv(majorcolour.getRGB());
               glVertex3d(start.getX()-centre.getX(),start.getY()-centre.getY(),altitude);
               glVertex3d(end.getX()-centre.getX(),start.getY()-centre.getY(),altitude);
               //Draw the differently coloured line on the end.getX() side.
               glColor3fv(minorcolour.getRGB());
               glVertex3d(end.getX()-centre.getX(),start.getY()-centre.getY(),altitude);
               glVertex3d(end.getX()-centre.getX(),end.getY()-centre.getY(),altitude);
               glColor3fv(majorcolour.getRGB());
               glVertex3d(end.getX()-centre.getX(),end.getY()-centre.getY(),altitude);
               glVertex3d(start.getX()-centre.getX(),end.getY()-centre.getY(),altitude);
            glEnd();
         }
         else
         {
        	//If gradient negative
            glBegin(GL_LINE_LOOP);
               glColor3fv(majorcolour.getRGB());
               glVertex3d(end.getX()-centre.getX(),start.getY()-centre.getY(),altitude);
               glVertex3d(start.getX()-centre.getX(),start.getY()-centre.getY(),altitude);
               //Draw the differently coloured line on the start.getX() side.
               glColor3fv(minorcolour.getRGB());
               glVertex3d(start.getX()-centre.getX(),start.getY()-centre.getY(),altitude);
               glVertex3d(start.getX()-centre.getX(),end.getY()-centre.getY(),altitude);
               glColor3fv(majorcolour.getRGB());
               glVertex3d(start.getX()-centre.getX(),end.getY()-centre.getY(),altitude);
               glVertex3d(end.getX()-centre.getX(),end.getY()-centre.getY(),altitude);
            glEnd();
         }
      }
   }
}
