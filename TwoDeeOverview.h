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
 * File: TwoDeeOverview.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: November 2009 - July 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "Quadtree.h"
#include "QuadtreeStructs.h"
#include "PointBucket.h"
#include <vector>
#include "Display.h"
#include "BoxOverlay.h"
#ifndef TWODEEOVERVIEW_H
#define TWODEEOVERVIEW_H
class TwoDeeOverview : public Display{
public:
   TwoDeeOverview(const Glib::RefPtr<const Gdk::GL::Config>& config,Quadtree* lidardata,int bucketlimit,Gtk::Label *rulerlabelover);
   ~TwoDeeOverview();
   bool on_pan_key(GdkEventKey* event,double scrollspeed);//Handles keyboard input for panning.
   bool on_prof_key(GdkEventKey* event,double scrollspeed,bool fractionalshift);//Handles keyboard input for profiling.
   bool on_fence_key(GdkEventKey* event,double scrollspeed);//Handles keyboard input for fencing.
   bool on_zoom_key(GdkEventKey* event);//Handles keyboard input for zooming.
   void waitforpause(){while(thread_running){usleep(10);}}//Method that waits until the drawing thread has paused.
   bool returntostart();//Returns to the initial view.
   bool drawviewable(int imagetype);//Draw the viewable part of the image.
   //Short, status changing methods:
   void setupprofile(){//Blocks pan signals and unblocks profile signals:
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
   void unsetupprofile(){//Blocks profile signals and unblocks pan signals:
      sigpanstart.unblock();
      sigpan.unblock();
      sigpanend.unblock();
      sigprofstart.block();
      sigprof.block();
      sigprofend.block();
      if(is_realized())get_window()->set_cursor();
      profiling=false;
   }
   void setupfence(){//Blocks pan signals and unblocks fence signals:
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
   void unsetupfence(){//Blocks fence signals and unblocks pan signals:
      sigpanstart.unblock();
      sigpan.unblock();
      sigpanend.unblock();
      sigfencestart.block();
      sigfence.block();
      sigfenceend.block();
      if(is_realized())get_window()->set_cursor();
      fencing=false;
   }
   void setupruler(){//Blocks pan signals and unblocks ruler signals:
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
   void unsetupruler(){//Blocks ruler signals and unblocks pan signals:
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
   void getprofile(double*& profxs,double*& profys,int& profps){
      profbox->getboundaries(profxs,profys,profps);
   }
   void getfence(double*& fencexs,double*& fenceys,int&fenceps){
      fencebox->getboundaries(fencexs,fenceys,fenceps);
   }
   bool getpausethread(){ return pausethread; }
   bool getthread_running(){ return thread_running; }
   BoxOverlay* getfencebox(){ return fencebox; }
   BoxOverlay* getprofbox(){ return profbox; }
   //Setters:
   void setshowprofile(double showprofile){ this->showprofile = showprofile;}
   void setshowfence(double showfence){ this->showfence = showfence;}
   void setshowdistancescale(double showdistancescale){ this->showdistancescale = showdistancescale;}
   void setshowlegend(double showlegend){ this->showlegend = showlegend;}
   void setpausethread(bool pausethread){ this->pausethread = pausethread; }
   void setreversez(bool reversez){ this->reversez = reversez; }
   void setraiseline(bool raiseline){ this->raiseline = raiseline; }
   void setlinetoraise(int linetoraise){ this->linetoraise = linetoraise; }
   void setresolutiondepth(int resolutiondepth){ this->resolutiondepth = resolutiondepth; }
   void setresolutionbase(int resolutionbase){ this->resolutionbase = resolutionbase; }
   //Classification:
      void setheightenNonC(bool heightenNonC){ this->heightenNonC = heightenNonC;}//These set whether various classifications should be highlighted by making them be drawn above other points:
      void setheightenGround(bool heightenGround){ this->heightenGround = heightenGround; }//...
      void setheightenLowVeg(bool heightenLowVeg){ this->heightenLowVeg = heightenLowVeg; }//...
      void setheightenMedVeg(bool heightenMedVeg){ this->heightenMedVeg = heightenMedVeg; }//...
      void setheightenHighVeg(bool heightenHighVeg){ this->heightenHighVeg = heightenHighVeg; }//...
      void setheightenBuildings(bool heightenBuildings){ this->heightenBuildings = heightenBuildings; }//...
      void setheightenNoise(bool heightenNoise){ this->heightenNoise = heightenNoise; }//...
      void setheightenMass(bool heightenMass){ this->heightenMass = heightenMass; }//...
      void setheightenWater(bool heightenWater){ this->heightenWater = heightenWater; }//...
      void setheightenOverlap(bool heightenOverlap){ this->heightenOverlap = heightenOverlap; }//...
      void setheightenUndefined(bool heightenUndefined){ this->heightenUndefined = heightenUndefined; }//...
protected:

   //Drawing variables:
   double drawnsofarminx,drawnsofarminy,drawnsofarmaxx,drawnsofarmaxy;//These define a rectangle that (very) roughly corresponds to how much of the flightline has been drawn so far. This is used for when panning, so that if there is not much already loaded then the bucket outlines are not completely covered by black.
   int resolutionbase,resolutiondepth;//These store the resolution levels available in the quadtree indirectly.
   int numbuckets;//The number of buckets in the last subset.
   bool raiseline;//Whether to raise a flightline above the others.
   int linetoraise;//Which flightline to raise.
   bool drawnsinceload;//This stores whether the entire image has been drawn since initially being loaded from file.
   bool reversez;//This stores whether the heights have been reversed to allow the seeing of low points above high ones.
   bool showlegend;//Determines whether the colour legend should be drawn, if applicable.
   bool showdistancescale;//Determines whether to draw the distance scale overlay.
   bool drawneverything;//Stores whether all of the buckets were drawn the last time the drawing thread was invoked.
   int pointcount;//The number of points being drawn from the current bucket.
   float* vertices;//This contains the coordinates of all the points in the current bucket.
   float* colours;//This contains the RGB components of the colours of all the points in the current bucket.

   //Drawing thread interaction variables:
   bool threaddebug;//If true, this causes a huge amount of spam to spew from the program whenever the drawing thread is in use, for debugging purposes.
   bool thread_existsmain;//This indicates whether a drawing thread currently exists as recorded by the main thread.
   bool thread_existsthread;//This indicates whether a drawing thread currently exists as recored by the drawing thread.
   bool thread_running;//This indicates that the thread is running, and so will be using the pointbucket::getpoint() method often.
   bool pausethread;//If this is true, the data manipulation thread will pause until it is set to false. This pausing is done because pointbucket::getpoint() is not threadsafe.
   bool interruptthread;//This indicates whether to interrupt the current drawing thread.
   bool initialising_GL_draw;//This indicates whether the main thread is/should/will be busy setting up OpenGL for drawing. If so, the drawing thread should pause.
   bool drawing_to_GL;//This indicates whether the main thread is/should/will be busy drawing the contents of the arrays vertices and colours to the framebuffer. If so, the drawing thread should pause.
   bool flushing;//This indicates whether the main thread is/should/will be busy flushing the contents of the framebuffer to the screen. If so, no new thread must be made before flushing is complete.
   bool extraDrawing;//This indicates whether an extra draw signal, caused by a previous draw signal being blocked to avoid deadlock or race conditions, has been sent by the main thread to itself. If so, the main thread should not send any more until the current one is resolved.
   //Signal dispatchers:
      Glib::Dispatcher signal_InitGLDraw;//Signal dispatcher from the drawing thread to the main thread to set up OpenGL for drawing.
      Glib::Dispatcher signal_EndGLDraw;//Signal dispatcher from the drawing thread to the main thread to clear up OpenGL after drawing is complete.
      Glib::Dispatcher signal_DrawGLToCard;//Signal dispatcher from the drawing thread to the main thread to draw the contents of the vertices and colours arrays to the framebuffer.
      Glib::Dispatcher signal_FlushGLToScreen;//Signal dispatcher from the drawing thread to the main thread to flush the contents of the framebuffer to the screen.
      Glib::Dispatcher signal_extraDraw;//Signal dispatcher from the main thread to itself to draw the points yet another time. This method is used as getting direct access to the variables affecting the main (GTK) thread would likely be very messy.

   //Position variables:
   double centrex,centrey;//These give the centre of the viewport in image terms, rather than screen terms.
   double centrexsafe,centreysafe;//These are "safe" stores of the coordinates of the centre of the screen. Safe in that they will not change (we hope) while the drawing thread is running. These are in object-wide scope, despite being used in multiple threads, because they are needed to draw the buckets properly when doing preview.
   double panstartx,panstarty;//Coordinates of the start of the pan move.
 
   //Overlays:
   Gtk::Label *rulerlabel;//Label showing the distance, in various dimensions, covered by the ruler and also the coordinates of fences and profiles.
      //Profiling:
      bool profiling;//Determines whether or not the profile should be drawn.
      bool showprofile;//Whether to display the profile box when not modifying it.
      BoxOverlay* profbox;//This represents the profile.
      //Fencing:
      bool fencing;//Determines whether or not the fence should be drawn.
      bool showfence;//Whether to display the fence when not modifying it.
      BoxOverlay* fencebox;//This represents the fence.
      //Rulering:
      double rulereventstartx,rulereventstarty;//The start coordinates of the ruler in pixels.
      double rulerstartx,rulerstarty;//The start coordinates for the ruler in world units.
      double rulerendx,rulerendy;//The end coordinates for the ruler in world units
      double rulerwidth;//The width of the ruler in pixels.
      bool rulering;//Determines whether or not the ruler should be drawn.
 
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
   bool pointinfo(double eventx,double eventy);//This returns some information about a selected point. Does not yet work properly.
 
   //Drawing:
   bool drawbuckets(PointBucket** buckets,int numbuckets);//Draw the outlines of the buckets and, above them, the contents of the back buffer (which contains the last stuff drawn from scratch).
   void drawoverlays();//Draw all the make methods indented below.
      void makerulerbox();//Make rectangle showing where the ruler is.
      void makedistancescale();//Make a scale for the LIDAR "map"
      void makecolourlegend();//Make a legend explaining what the colours mean. Note that this shows nothing when colouring by flightline or by nothing.
   //Drawing thread related:
      void mainimage(PointBucket** buckets,int numbuckets);//Draw the main image. This is used by the drawing thread. Handle with care.
      bool drawpointsfrombuckets(PointBucket** buckets,int numbuckets,bool *drawnbucketsarray,bool cachedonly);
      void threadpause();//Called by the drawing thread when it is told to pause. Should ONLY be called the the drawing thread. It frees up resources for the main thread to use and waits until the drawing thread can unpause.
      void threadend(PointBucket** buckets);//Clears up after the thread is told to/decides to end.
      void extraDraw();//Draw the points once again because a previous draw was blocked to avoid deadlocks and race conditions.
      void InitGLDraw();//Set up OpenGL for drawing.
      void DrawGLToCard();//Draw the contents of the arrays vertices and colours to the framebuffer.
      void FlushGLToScreen();//Flush the contents of the framebuffer to the screen.
      void EndGLDraw();//Clear up the OpenGL settings.
 
   //Positioning methods:
   void resetview();//Determines what part of the image is displayed with orthographic projection.
   bool on_zoom(GdkEventScroll* event);//Allows the user to zoom with the mouse wheel.
   void set_overlay_zoomlevels(double zoomlevel){ profbox->setzoomlevel(zoomlevel);fencebox->setzoomlevel(zoomlevel); }//Set zoomlevels of the overlays.
   void set_overlay_centres(double centrex,double centrey){ profbox->setcentre(centrex,centrey);fencebox->setcentre(centrex,centrey); }//Set centres of the overlays.

   //Panning control:   //These allow the user to pan by clicking and dragging.
   bool on_pan_start(GdkEventButton* event);
   bool on_pan(GdkEventMotion* event);
   bool on_pan_end(GdkEventButton* event);
   //Profiling:   //These allow the user to select an area to view in the profile window.
   bool on_prof_start(GdkEventButton* event);
   bool on_prof(GdkEventMotion* event);
   bool on_prof_end(GdkEventButton* event);
   //Fencing:  //These allow the user to define a region that "clips" the points loaded from flightlines.That is, it is used so that the user can view certain parts of large files in detail without having to load everything in.
   bool on_fence_start(GdkEventButton* event);
   bool on_fence(GdkEventMotion* event);
   bool on_fence_end(GdkEventButton* event);
   //Rulering control:   //These allow the user to ruler by clicking and dragging.
   bool on_ruler_start(GdkEventButton* event);
   bool on_ruler(GdkEventMotion* event);
   bool on_ruler_end(GdkEventButton* event);
};

#endif
