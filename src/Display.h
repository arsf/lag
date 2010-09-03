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
   Display(const Glib::RefPtr<const Gdk::GL::Config>& config,
           Quadtree* lidardata,int bucketlimit);
   ~Display();
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
   void coloursandshades(double maxz,
                         double minz,
                         int maxintensity,
                         int minintensity);
   //Getters:
   double getrmaxz(){
      return rmaxz;
   }

   double getrminz(){
      return rminz;
   }

   double getzoffset(){
      return zoffset;
   }

   double getzfloor(){
      return zfloor;
   }

   int getrmaxintensity(){
      return rmaxintensity;
   }

   int getrminintensity(){
      return rminintensity;
   }

   double getintensityoffset(){
      return intensityoffset;
   }

   double getintensityfloor(){
      return intensityfloor;
   }

   //Setters:
   void setlidardata(Quadtree* lidardata,int bucketlimit){
      this->lidardata=lidardata;
      this->bucketlimit=bucketlimit;
   }  

   void setintensitycolour(bool intensitycolour){
      this->intensitycolour=intensitycolour;
   }

   void setheightcolour(bool heightcolour){
      this->heightcolour=heightcolour;
   }

   void setlinecolour(bool linecolour){
      this->linecolour=linecolour;
   }

   void setclasscolour(bool classcolour){
      this->classcolour=classcolour;
   }
   
   void setreturncolour(bool returncolour){
      this->returncolour=returncolour;
   }

   void setintensitybrightness(bool intensitybrightness){
      this->intensitybrightness=intensitybrightness;
   }

   void setheightbrightness(bool heightbrightness){
      this->heightbrightness=heightbrightness;
   }

   void setpointwidth(double pointsize){
      this->pointsize=pointsize;glPointSize(pointsize);
   }

   void setmaindetail(double maindetailmod){
      this->maindetailmod=maindetailmod;
   }

   void setzoffset(double zoffset){
      this->zoffset = zoffset;
   }

   void setzfloor(double zfloor){
      this->zfloor = zfloor;
   }

   void setintensityoffset(double intensityoffset){
      this->intensityoffset = intensityoffset;
   }

   void setintensityfloor(double intensityfloor){
      this->intensityfloor = intensityfloor;
   }

protected:
   //Control:
   // Indicates that the window (and therefore this display area) is 
   // changing shape and/or size.
   bool configuring;
   // The zoomlevel's change is determined by a pow(a,b) call. This 
   // variable stores the power.
   double zoompower;

   //Point data and related stuff:
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
   // These are the values of height and intensity used for the last 
   // determination of colour and shade and used to allow the proper display 
   // of a legend.
   double cbmaxz,cbminz;
   int cbmaxintensity,cbminintensity;
   //True if want to colour by height.
   bool heightcolour;
   //True if want to shade by height.
   bool heightbrightness;
   //A minimum height brightness value that also scales higher values.
   double zoffset;
   //As above, but does not scale anything.
   double zfloor;
   //True if want to colour by intensity.
   bool intensitycolour;
   //True if want to shade by intensity.
   bool intensitybrightness;
   //A minimum intensity brightness value that also scales higher values.
   double intensityoffset;
   //As above, but does not scale anything.
   double intensityfloor;
   //Whether to colour by flightline.
   bool linecolour;
   //Whether to colour by classification.
   bool classcolour;
   //Whether to colour by return.
   bool returncolour;
   //The maximum and minimum heights, for colour by elevation etc..
   double rmaxz,rminz;
   //The maximum and minimum intensity, for brightness by intensity etc..
   int rmaxintensity,rminintensity;
   //Array containing colours for different heights.
   double* colourheightarray;
   //Array containing colours for different intensities.
   double* colourintensityarray;
   //Array containing shades for different heights.
   double* brightnessheightarray;
   //Array containing shades for different intensities.
   double* brightnessintensityarray;
   
   //Methods:

   //Convenience code:
   // Convenience method for getting a subset, catching any exception and 
   // determining whether there is useful data.
   bool advsubsetproc(vector<PointBucket*>*& pointvector,
                      double *xs,double *ys,int ps);
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
 
   //Colouring and shading:
   //Colours by a numeric variable.
   void colour_by(double value,double maxvalue,double minvalue,
                  double& col1,double& col2,double& col3);
   //Shades by a numeric variable.
   double brightness_by(double value,double maxvalue,double minvalue,
                        double offsetvalue,double floorvalue);
 
   //Positioning methods:
   // Determines what part of the image is displayed with orthographic 
   // projection.
   virtual void resetview() = 0;
   //Handles resizing of the window. Calls resetview().
   bool on_configure_event(GdkEventConfigure* event);
};
#endif
