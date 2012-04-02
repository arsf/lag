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
 * File: LagDisplay.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: January - July 2010
 *
 * */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <gtkmm.h>
#include <gtkglmm.h>
#include "Quadtree.h"
#include "PointBucket.h"
#include <vector>
#include "Colour.h"
#include <GL/gl.h>

// Constants for "fixed" font
const unsigned int FONT_CHAR_WIDTH = 7;
const unsigned int FONT_CHAR_HEIGHT = 12;

class LagDisplay : public Gtk::GL::DrawingArea
{
public:
   LagDisplay(string fontpath, const Glib::RefPtr<const Gdk::GL::Config>& config, int bucketlimit);

   ~LagDisplay();

   // Sets the intial position and translation ratio from screen to world 
   // scale of the data.
   virtual bool returntostart() = 0;

   //Draw any relevant overlays.
   virtual void drawoverlays() = 0;

   //Draw the viewable part of the data.
   virtual bool drawviewable(int imagetype) = 0;

   // Reads from subset of quadtree and prepares variables for 
   // colouring etc..
   void prepare_image();

   //Prepare colour and brightness arrays.
   void coloursandshades(double maxz, double minz, int maxintensity, int minintensity);

   // This is how the pixels in the drawing area are coloured.
   enum brightnessBy_t {brightnessByNone, 
                        brightnessByIntensity, 
                        brightnessByHeight};

   enum colourBy_t { colourByNone, 
                     colourByIntensity, 
                     colourByHeight, 
                     colourByFlightline, 
                     colourByClassification, 
                     colourByReturn};

   // An enum to describe what mode the window is in
   enum currentMode_t { panning,
                        scrolling,
                        profiling,
                        rulering,
                        fencing };

   //Getters:
   inline double getrmaxz()
   {
      return rmaxz;
   }

   inline double getrminz()
   {
      return rminz;
   }

   inline double getzoffset()
   {
      return zoffset;
   }

   inline double getzfloor()
   {
      return zfloor;
   }

   inline int getrmaxintensity()
   {
      return rmaxintensity;
   }

   inline int getrminintensity()
   {
      return rminintensity;
   }

   inline double getintensityoffset()
   {
      return intensityoffset;
   }

   inline double getintensityfloor()
   {
      return intensityfloor;
   }

   //Setters:
   inline void setlidardata(Quadtree* lidardata,int bucketlimit)
   {
      this->lidardata=lidardata;
      this->bucketlimit=bucketlimit;
   } 
   
   inline void setColourBy(colourBy_t colour)
   {
      colourBy = colour;
   }

   inline void setBrightnessBy(brightnessBy_t brightness)
   {
      brightnessBy = brightness;
   }

   inline void setpointwidth(double pointsize)
   {
      this->pointsize=pointsize;
      //glPointSize(pointsize);
   }

   inline void setmaindetail(double maindetailmod)
   {
      this->maindetailmod=maindetailmod;
   }

   inline void setzoffset(double zoffset)
   {
      this->zoffset = zoffset;
   }

   inline void setzfloor(double zfloor)
   {
      this->zfloor = zfloor;
   }

   inline void setintensityoffset(double intensityoffset)
   {
      this->intensityoffset = intensityoffset;
   }

   inline void setintensityfloor(double intensityfloor)
   {
      this->intensityfloor = intensityfloor;
   }

   void set_background_colour(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
   void update_background_colour();

protected:
   //Control:
   // The zoomlevel's change is determined by a pow(a,b) call. This 
   // variable stores the power.
   double zoompower;

   //Point data and related stuff:
   Point centre;
   Point panStart;

   //The point data is stored here.
   Quadtree* lidardata;

   //This is the maximum number of points a single bucket can contain.
   int bucketlimit;

   // This plays a part in determining which resolution level is drawn 
   // for each bucket. A lower value means a lower resolution.
   double maindetailmod;

   //The diameter of the points in pixels.
   double pointsize;

   //Position variables:
   // This is the level of zoom. It starts at 1, i.e. 100%. Together 
   // with ratio it determines the translation from screen scale to 
   // world scale and vice versa and is set so that when its value is 
   // 1 all of the quadtree must be just visible on the window.
   double zoomlevel;

   // This determines, along with the zoomlevel, the scaling of the image 
   // relative to the screen. At zoomlevel 1, the image should just fit 
   // within the screen.
   double ratio;
 
   //Colouring and shading variables:
   enum brightnessBy_t brightnessBy;
   enum colourBy_t colourBy;

   // Window mode:
   enum currentMode_t currentMode;

   // These are the values of height and intensity used for the last 
   // determination of colour and shade and used to allow the proper display 
   // of a legend.
   double cbmaxz,cbminz;
   int cbmaxintensity,cbminintensity;

   //A minimum height brightness value that also scales higher values.
   double zoffset;

   //As above, but does not scale anything.
   double zfloor;

   //A minimum intensity brightness value that also scales higher values.
   double intensityoffset;

   //As above, but does not scale anything.
   double intensityfloor;

   //The maximum and minimum heights, for colour by elevation etc..
   double rmaxz,rminz;
   //The maximum and minimum intensity, for brightness by intensity etc..
   int rmaxintensity,rminintensity;

   //Array containing colours for different heights.
   double* colourheightarray;
   //Array containing shades for different heights.
   double* brightnessheightarray;
   //Array containing shades for different intensities.
   double* brightnessintensityarray;
   
   // Background colour
   float red;
   float green;
   float blue;
   float alpha;

   //Methods:

   //Convenience code:

   // Convert pixels to image units
   double pixelsToImageUnits(double);

   // Convert image units to pixels
   double imageUnitsToPixels(double);

   // Convenience method for getting a subset, catching any exception and 
   // determining whether there is useful data.
   bool advsubsetproc(vector<PointBucket*>*& pointvector, double *xs,double *ys,int ps);

   //Convenience method for clearing the screen
   bool clearscreen();

   // Convenience method for making sure that the different areas using OpenGL 
   // do not bleed into each other and cause graphical glitches.
   void guard_against_interaction_between_GL_areas();

   //Drawing:
   //Realises drawing area and calls prepare_image().
   void on_realize();

   //Calls draw on an expose event.
   bool on_expose_event(GdkEventExpose* event);

   //Fonts
   //void initFont();

   // Printfs a raster string at a certain position
   void printString(double, double, double);
 
   //Colouring and shading:
   
   // Functions which return a colour depending on how the user wants the flightlines
   // Coloured.
   Colour getColourByFlightline(int);
   Colour getColourByReturn(int);
   Colour getColourByClassification(int);
   Colour getColourByHeight(float);
   Colour getColourByIntensity(int);

   //Colours by a numeric variable.
   void colour_by(double, double, double, Colour&);

   //Shades by a numeric variable.
   double brightness_by(double value,double maxvalue,double minvalue, double offsetvalue,double floorvalue);
 
   //Positioning methods:
   // Determines what part of the image is displayed with orthographic 
   // projection.
   virtual void resetview() = 0;

   //Handles resizing of the window. Calls resetview().
   bool on_configure_event(GdkEventConfigure* event);

};
#endif
