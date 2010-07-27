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
 * File: Display.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: January - July 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "Quadtree.h"
#include "QuadtreeStructs.h"
#include "PointBucket.h"
#include <vector>
#ifndef DISPLAY_H
#define DISPLAY_H
class Display : public Gtk::GL::DrawingArea{
public:
   Display(const Glib::RefPtr<const Gdk::GL::Config>& config,Quadtree* lidardata,int bucketlimit);
   ~Display();
   virtual bool returntostart() = 0;//Sets the intial position and translation ratio from screen to world scale of the data.
   virtual void drawoverlays() = 0;//Draw any relevant overlays.
   virtual bool drawviewable(int imagetype) = 0;//Draw the viewable part of the data.
   void prepare_image();//Reads from subset of quadtree and prepares variables for colouring etc..
   void coloursandshades(double maxz,double minz,int maxintensity,int minintensity);//Prepare colour and brightness arrays.
   //Getters:
   double getrmaxz(){return rmaxz;}
   double getrminz(){return rminz;}
   double getzoffset(){return zoffset;}
   double getzfloor(){return zfloor;}
   int getrmaxintensity(){return rmaxintensity;}
   int getrminintensity(){return rminintensity;}
   double getintensityoffset(){return intensityoffset;}
   double getintensityfloor(){return intensityfloor;}
   //Setters:
   void setlidardata(Quadtree* lidardata,int bucketlimit){this->lidardata=lidardata;this->bucketlimit=bucketlimit;}
   void setintensitycolour(bool intensitycolour){this->intensitycolour=intensitycolour;}
   void setheightcolour(bool heightcolour){this->heightcolour=heightcolour;}
   void setlinecolour(bool linecolour){this->linecolour=linecolour;}
   void setclasscolour(bool classcolour){this->classcolour=classcolour;}
   void setreturncolour(bool returncolour){this->returncolour=returncolour;}
   void setintensitybrightness(bool intensitybrightness){this->intensitybrightness=intensitybrightness;}
   void setheightbrightness(bool heightbrightness){this->heightbrightness=heightbrightness;}
   void setpointwidth(double pointsize){this->pointsize=pointsize;glPointSize(pointsize);}
   void setmaindetail(double maindetailmod){this->maindetailmod=maindetailmod;}
   void setzoffset(double zoffset){this->zoffset = zoffset;}
   void setzfloor(double zfloor){this->zfloor = zfloor;}
   void setintensityoffset(double intensityoffset){this->intensityoffset = intensityoffset;}
   void setintensityfloor(double intensityfloor){this->intensityfloor = intensityfloor;}
protected:
   //Control:
   bool configuring;//Indicates that the window (and therefore this display area) is changing shape and/or size.
   double zoompower;//The zoomlevel's change is determined by a pow(a,b) call. This variable stores the power.

   //Point data and related stuff:
   Quadtree* lidardata;//The point data is stored here.
   int bucketlimit;//This is the maximum number of points a single bucket can contain.
   double maindetailmod;//This plays a part in determining which resolution level is drawn for each bucket. A lower value means a lower resolution.
   double pointsize;//The diameter of the points in pixels.

   //Position variables:
   double zoomlevel;//This is the level of zoom. It starts at 1, i.e. 100%. Together with ratio it determines the translation from screen scale to world scale and vice versa and is set so that when its value is 1 all of the quadtree must be just visible on the window.
   double ratio;//This determines, along with the zoomlevel, the scaling of the image relative to the screen. At zoomlevel 1, the image should just fit within the screen.
 
   //Colouring and shading variables:
   double cbmaxz,cbminz;//These are the values of height and intensity used for the last determination of colour and shade and used to allow the proper display of a legend.
   int cbmaxintensity,cbminintensity;//...
   bool heightcolour;//True if want to colour by height.
   bool heightbrightness;//True if want to shade by height.
   double zoffset;//A minimum height brightness value that also scales higher values.
   double zfloor;//As above, but does not scale anything.
   bool intensitycolour;//True if want to colour by intensity.
   bool intensitybrightness;//True if want to shade by intensity.
   double intensityoffset;//A minimum intensity brightness value that also scales higher values.
   double intensityfloor;//As above, but does not scale anything.
   bool linecolour;//Whether to colour by flightline.
   bool classcolour;//Whether to colour by classification.
   bool returncolour;//Whether to colour by return.
   double rmaxz,rminz;//The maximum and minimum heights, for colour by elevation etc..
   int rmaxintensity,rminintensity;//The maximum and minimum intensity, for brightness by intensity etc..
   double* colourheightarray;//Array containing colours for different heights.
   double* colourintensityarray;//Array containing colours for different intensities.
   double* brightnessheightarray;//Array containing shades for different heights.
   double* brightnessintensityarray;//Array containing shades for different intensities.
   
   //Methods:

   //Convenience code:
   bool advsubsetproc(vector<PointBucket*>*& pointvector,double *xs,double *ys,int ps);//Convenience method for getting a subset, catching any exception and determining whether there is useful data.
   bool clearscreen();//Convenience method for clearing the screen
   void guard_against_interaction_between_GL_areas();//Convenience method for making sure that the different areas using OpenGL do not bleed into each other and cause graphical glitches.

   //Drawing:
   void on_realize();//Realises drawing area and calls prepare_image().
   bool on_expose_event(GdkEventExpose* event);//Calls draw on an expose event.
 
   //Colouring and shading:
   void colour_by(double value,double maxvalue,double minvalue,double& col1,double& col2,double& col3);//Colours by a numeric variable.
   double brightness_by(double value,double maxvalue,double minvalue,double offsetvalue,double floorvalue);//Shades by a numeric variable.
 
   //Positioning methods:
   virtual void resetview() = 0;//Determines what part of the image is displayed with orthographic projection.
   bool on_configure_event(GdkEventConfigure* event);//Handles resizing of the window. Calls resetview().
};
#endif
