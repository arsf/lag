/*
===============================================================================

 TwoDeeOverview.h

 Created on: Nov 2009
 Author: Haraldur Tristan Gunnarsson

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

#ifndef TWODEEOVERVIEW_H
#define TWODEEOVERVIEW_H

#include <gtkmm.h>
#include <gtkglmm.h>
#include <vector>
#include "Quadtree.h"
#include "PointBucket.h"
#include "LagDisplay.h"
#include "SelectionBox.h"
#include "BoxOverlay.h"


/*
===============================================================================

 TwoDeeOverview - represents the drawing area in TwoDeeOverview window.

===============================================================================
*/
class TwoDeeOverview : public LagDisplay
{
public:
   TwoDeeOverview(const Glib::RefPtr<const Gdk::GL::Config>& config,
                  int bucketlimit,
                  Gtk::Label *rulerlabelover);

   ~TwoDeeOverview();

   //Handles keyboard input for panning.
   bool on_pan_key(GdkEventKey* event,double scrollspeed);

   //Handles keyboard input for profiling.
   bool on_prof_key(GdkEventKey* event,double scrollspeed, bool fractionalshift);

   //Handles keyboard input for fencing.
   bool on_fence_key(GdkEventKey* event,double scrollspeed);

   //Handles keyboard input for zooming.
   bool on_zoom_key(GdkEventKey* event);

   //Method that waits until the drawing thread has paused.
   void waitforpause()
   {
      while(thread_running)
      {
#ifdef __WIN32
         // this is a compromise, ideally the code should behave in the
         // same way on all operating systems
         Sleep(1);
#else
         usleep(10);
#endif
      }
   }

   //Returns to the initial view.
   bool returntostart();

   //Draw the viewable part of the image.
   bool drawviewable(int imagetype);

   void toggleNoise(); 

   //Short, status changing methods:
   //Blocks pan signals and unblocks profile signals:
   void setupprofile(){
      sigpanstart.block();
      sigpan.block();
      sigpanend.block();
      sigprofstart.unblock();
      sigprof.unblock();
      sigprofend.unblock();
      Gdk::Cursor cursor(Gdk::CROSSHAIR);
      if(is_realized())get_window()->set_cursor(cursor);
      profiling=true;
   }
   //Blocks profile signals and unblocks pan signals:
   void unsetupprofile(){
      sigpanstart.unblock();
      sigpan.unblock();
      sigpanend.unblock();
      sigprofstart.block();
      sigprof.block();
      sigprofend.block();
      if(is_realized())get_window()->set_cursor();
      profiling=false;
   }
   //Blocks pan signals and unblocks fence signals:
   void setupfence(){
      sigpanstart.block();
      sigpan.block();
      sigpanend.block();
      sigfencestart.unblock();
      sigfence.unblock();
      sigfenceend.unblock();
      Gdk::Cursor cursor(Gdk::CROSSHAIR);
      if(is_realized())get_window()->set_cursor(cursor);
      fencing=true;
   }
   //Blocks fence signals and unblocks pan signals:
   void unsetupfence(){
      sigpanstart.unblock();
      sigpan.unblock();
      sigpanend.unblock();
      sigfencestart.block();
      sigfence.block();
      sigfenceend.block();
      if(is_realized())get_window()->set_cursor();
      fencing=false;
   }
   //Blocks pan signals and unblocks ruler signals:
   void setupruler(){
      sigpanstart.block();
      sigpan.block();
      sigpanend.block();
      sigrulerstart.unblock();
      sigruler.unblock();
      sigrulerend.unblock();
      Gdk::Cursor cursor(Gdk::CROSSHAIR);
      if(is_realized())get_window()->set_cursor(cursor);
      rulering=true;
   }
   //Blocks ruler signals and unblocks pan signals:
   void unsetupruler(){
      sigpanstart.unblock();
      sigpan.unblock();
      sigpanend.unblock();
      sigrulerstart.block();
      sigruler.block();
      sigrulerend.block();
      if(is_realized())get_window()->set_cursor();
      rulering=false;
   }

   //Getters:
   SelectionBox getProfile()
   {
      return profbox->getSelectionBox();
   }

   SelectionBox getFence()
   {
      return fencebox->getSelectionBox();
   }

   void getprofile(vector<double>& profxs, vector<double>& profys, int& profps)
   {
      profxs = profbox->getSelectionBox().getXs();
      profys = profbox->getSelectionBox().getYs();
      profps = 4;
   }

   bool getpausethread()
   {
      return pausethread; 
   }

   bool getthread_running()
   {
      return thread_running; 
   }

   BoxOverlay* getfencebox()
   {
      return fencebox; 
   }

   BoxOverlay* getprofbox()
   {
      return profbox; 
   }

   //Setters:
   void setshowdistancescale(double showdistancescale)
   {
      this->showdistancescale = showdistancescale;
   }

   void setshowlegend(double showlegend)
   {
      this->showlegend = showlegend;
   }

   void setpausethread(bool pausethread)
   {
      this->pausethread = pausethread; 
   }

   void setreversez(bool reversez)
   {
      this->reversez = reversez; 
   }

   void setraiseline(bool raiseline)
   {
      this->raiseline = raiseline; 
   }

   void setlinetoraise(int linetoraise)
   {
      this->linetoraise = linetoraise; 
   }

   void setresolutiondepth(int resolutiondepth)
   {
      this->resolutiondepth = resolutiondepth; 
   }

   void setresolutionbase(int resolutionbase)
   {
      this->resolutionbase = resolutionbase; 
   }

   void set_panning_refresh(int refreshType)
   {
	   this->panningRefresh = refreshType;
   }

   //Classification:
   // These set whether various classifications should be highlighted by 
   // making them be drawn above other points:
   void setheightenNonC(bool heightenNonC){ 
      this->heightenNonC = heightenNonC;
   }
   void setheightenGround(bool heightenGround){ 
      this->heightenGround = heightenGround; 
   }

   void setheightenLowVeg(bool heightenLowVeg){ 
      this->heightenLowVeg = heightenLowVeg; 
   } 

   void setheightenMedVeg(bool heightenMedVeg){ 
      this->heightenMedVeg = heightenMedVeg; 
   }

   void setheightenHighVeg(bool heightenHighVeg){ 
      this->heightenHighVeg = heightenHighVeg; 
   }

   void setheightenBuildings(bool heightenBuildings){ 
      this->heightenBuildings = heightenBuildings;
   }
 
   void setheightenNoise(bool heightenNoise){ 
      this->heightenNoise = heightenNoise; 
   }

   void setheightenMass(bool heightenMass){ 
      this->heightenMass = heightenMass; 
   }

   void setheightenWater(bool heightenWater){ 
      this->heightenWater = heightenWater; 
   }

   void setheightenOverlap(bool heightenOverlap){ 
      this->heightenOverlap = heightenOverlap; 
   }

   void setheightenUndefined(bool heightenUndefined){ 
      this->heightenUndefined = heightenUndefined; 
   }

   void set_superzoom(bool);

   void set_slicing(bool slice)
   {
	   this->slicing = slice;
   }

   bool get_slicing()
   {
	   return this->slicing;
   }

   void set_slice_minz(double minz)
   {
	   this->slice_minz = minz;
   }

   double get_slice_minz()
   {
	   return this->slice_minz;
   }

   void set_slice_maxz(double maxz)
   {
	   this->slice_maxz = maxz;
   }

   double get_slice_maxz()
   {
	   return this->slice_maxz;
   }

   bool get_latlong()
   {
	   return this->latlong;
   }

   void set_latlong(bool uselatlong)
   {
	   this->latlong = uselatlong;
   }

   void set_utm_zone(std::string zone)
   {
	   this->utm_zone = zone;
   }

   std::string get_utm_zone()
   {
	   return this->utm_zone;
   }

   void convert_to_latlong(double* point);

protected:

   //Drawing variables:
   // These define a rectangle that (very) roughly corresponds to how much of 
   // the flightline has been drawn so far. This is used for when panning, so 
   // that if there is not much already loaded then the bucket outlines are 
   // not completely covered by black.
   double drawnsofarminx,drawnsofarminy,drawnsofarmaxx,drawnsofarmaxy;

   //These store the resolution levels available in the quadtree indirectly.
   int resolutionbase,resolutiondepth;

   //The number of buckets in the last subset.
   int numbuckets;

   //Whether to raise a flightline above the others.
   bool raiseline;

   //Which flightline to raise.
   int linetoraise;

   // This stores whether the entire image has been drawn since initially 
   // being loaded from file.
   bool drawnsinceload;

   // This stores whether the heights have been reversed to allow the seeing 
   // of low points above high ones.
   bool reversez;

   //Determines whether the colour legend should be drawn, if applicable.
   bool showlegend;

   //Determines whether to draw the distance scale overlay.
   bool showdistancescale;

   // Stores whether all of the buckets were drawn the last time the drawing 
   // thread was invoked.
   bool drawneverything;

   //The number of points being drawn from the current bucket.
   int pointcount;

   //This contains the coordinates of all the points in the current bucket.
   float* vertices;

   // This contains the RGB components of the colours of all the points in the 
   // current bucket.
   float* colours;

   //Drawing thread interaction variables:
   // If true, this causes a huge amount of spam to spew from the program 
   // whenever the drawing thread is in use, for debugging purposes.
   bool threaddebug;

   // This indicates whether a drawing thread currently exists as recorded 
   // by the main thread.
   bool thread_existsmain;

   // This indicates whether a drawing thread currently exists as recored by 
   // the drawing thread.
   bool thread_existsthread;

   // This indicates that the thread is running, and so will be using the 
   // pointbucket::getpoint() method often.
   bool thread_running;

   // If this is true, the data manipulation thread will pause until it is set
   // to false. This pausing is done because pointbucket::getpoint() is 
   // not threadsafe.
   bool pausethread;

   //This indicates whether to interrupt the current drawing thread.
   bool interruptthread;

   // This indicates whether the main thread is/should/will be busy setting up 
   // OpenGL for drawing. If so, the drawing thread should pause.
   bool initialising_GL_draw;

   // This indicates whether the main thread is/should/will be busy drawing the 
   // contents of the arrays vertices and colours to the framebuffer. If so, 
   // the drawing thread should pause.
   bool drawing_to_GL;

   // This indicates whether the main thread is/should/will be busy flushing 
   // the contents of the framebuffer to the screen. If so, no new thread must 
   // be made before flushing is complete.
   bool flushing;

   // This indicates whether an extra draw signal, caused by a previous draw 
   // signal being blocked to avoid deadlock or race conditions, has been 
   // sent by the main thread to itself. If so, the main thread should not 
   // send any more until the current one is resolved.
   bool extraDrawing;

   //Signal dispatchers:

   // Signal dispatcher from the drawing thread to the main thread to set up 
   // OpenGL for drawing. 
   Glib::Dispatcher signal_InitGLDraw;

   // Signal dispatcher from the drawing thread to the main thread to clear up 
   // OpenGL after drawing is complete.
   Glib::Dispatcher signal_EndGLDraw;

   // Signal dispatcher from the drawing thread to the main thread to draw the 
   // contents of the vertices and colours arrays to the framebuffer.
   Glib::Dispatcher signal_DrawGLToCard;

   // Signal dispatcher from the drawing thread to the main thread to flush the 
   // contents of the framebuffer to the screen.
   Glib::Dispatcher signal_FlushGLToScreen;

   // Signal dispatcher from the main thread to itself to draw the points yet 
   // another time. This method is used as getting direct access to the 
   // variables affecting the main (GTK) thread would likely be very messy.
   Glib::Dispatcher signal_extraDraw;


   bool tdoDisplayNoise;
   //Position variables:

   // These give the centre of the viewport in image terms, rather than screen 
   // terms.
   Point centreSafe;

   // Label showing the distance, in various dimensions, covered by the ruler 
   // and also the coordinates of fences and profiles.
   Gtk::Label *rulerlabel;

   //Profiling:

   //Determines whether or not the profile should be drawn.
   bool profiling;

   //Whether to display the profile box when not modifying it. 
   bool showprofile;

   //SelectionBox profileSelectionBox;
   BoxOverlay* profbox;

   //Fencing:

   //Determines whether or not the fence should be drawn.
   bool fencing;

   //Whether to display the fence when not modifying it.
   bool showfence;

   //This represents the fence.
   BoxOverlay* fencebox;

   //Rulering:
   Point rulerEventStart;
   Point rulerStart;
   Point rulerEnd;
   double rulerwidth;
   bool rulering;
 
   //Classification heightening:
   bool heightenNonC;
   bool heightenGround;
   bool heightenLowVeg;
   bool heightenMedVeg;
   bool heightenHighVeg;
   bool heightenBuildings;
   bool heightenNoise;
   bool heightenMass;
   bool heightenWater;
   bool heightenOverlap;
   bool heightenUndefined;

   //Panning: 2 = no refresh, 1 = full refresh
   int panningRefresh;

   // Slicing
   bool slicing;
   double slice_minz;
   double slice_maxz;

   //Latlong
   bool latlong;
   std::string utm_zone;

   bool superzoom;

   //Signal handlers:
   //Panning:
   sigc::connection sigpanstart;
   sigc::connection sigpan;
   sigc::connection sigpanend;
   //Profiling:
   sigc::connection sigprofstart;
   sigc::connection sigprof;
   sigc::connection sigprofend;
   //Fencing:
   sigc::connection sigfencestart;
   sigc::connection sigfence;
   sigc::connection sigfenceend;
   //Rulering:
   sigc::connection sigrulerstart;
   sigc::connection sigruler;
   sigc::connection sigrulerend;
 
   //Methods:

   // This returns some information about a selected point. Does not yet
   // work properly.
   bool pointinfo(double eventx,double eventy);

   //Drawing:

   // Draw the outlines of the buckets and, above them, the contents of the 
   // back buffer (which contains the last stuff drawn from scratch).
   bool drawbuckets(PointBucket** buckets,int numbuckets);

   //Draw all the make methods below.
   void drawoverlays();

   //Make rectangle showing where the ruler is.
   void makerulerbox();

   //Make a scale for the LIDAR "map"
   void makedistancescale();

   // Make a legend explaining what the colours mean. Note that this shows 
   // nothing when colouring by flightline or by nothing.
   void makecolourlegend();

   //Drawing thread related:
   //Draw the main image. This is used by the drawing thread. Handle with care.
   void mainimage(PointBucket** buckets,int numbuckets);

   bool drawpointsfrombuckets(PointBucket** buckets,
                              int numbuckets,
                              bool *drawnbucketsarray,
                              bool cachedonly);

   // Called by the drawing thread when it is told to pause. Should ONLY be 
   // called the the drawing thread. It frees up resources for the main 
   // thread to use and waits until the drawing thread can unpause.
   void threadpause();

   //Clears up after the thread is told to/decides to end.
   void threadend(PointBucket** buckets);

   // Draw the points once again because a previous draw was blocked to 
   // avoid deadlocks and race conditions.
   void extraDraw();

   //Set up OpenGL for drawing.
   void InitGLDraw();

   //Draw the contents of the arrays vertices and colours to the framebuffer.
   void DrawGLToCard();

   //Flush the contents of the framebuffer to the screen.
   void FlushGLToScreen();

   //Clear up the OpenGL settings.
   void EndGLDraw();

   //Allows the user to zoom with the mouse wheel.
   bool on_zoom(GdkEventScroll* event);

   //Set zoomlevels of the overlays.
   void set_overlay_zoomlevels(double zoomlevel){ 
      profbox->setzoomlevel(zoomlevel);
      fencebox->setzoomlevel(zoomlevel); 
   }

   //Set centres of the overlays.
   void set_overlay_centres(Point centre){ 
      profbox->setcentre(centre);
      fencebox->setcentre(centre); 
   }

   //Panning control:   

   //These allow the user to pan by clicking and dragging.
   bool on_pan_start(GdkEventButton* event);
   bool on_pan(GdkEventMotion* event);
   bool on_pan_end(GdkEventButton* event);

   //Profiling:   

   //These allow the user to select an area to view in the profile window.
   bool on_prof_start(GdkEventButton* event);
   bool on_prof(GdkEventMotion* event);
   bool on_prof_end(GdkEventButton* event);

   //Fencing:  

   //These allow the user to define a region that "clips" the points loaded 
   //from flightlines.That is, it is used so that the user can view certain 
   //parts of large files in detail without having to load everything in.
   bool on_fence_start(GdkEventButton* event);
   bool on_fence(GdkEventMotion* event);
   bool on_fence_end(GdkEventButton* event);

   //Rulering control:   

   //These allow the user to ruler by clicking and dragging.
   bool on_ruler_start(GdkEventButton* event);
   bool on_ruler(GdkEventMotion* event);
   bool on_ruler_end(GdkEventButton* event);

   void resetview();
};

#endif
