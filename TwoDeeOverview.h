/*
 * File: TwoDeeOverview.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: November 2009 - June 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "quadtree.h"
#include "quadtreestructs.h"
#include "pointbucket.h"
#include <vector>
#include "Display.h"
#include "BoxOverlay.h"
class TwoDeeOverview : public Display{
public:
   TwoDeeOverview(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit,Gtk::Label *rulerlabelover);
   ~TwoDeeOverview();
   void waitforpause(){while(thread_running){usleep(10);}}
   void clippy(string picturename);//EASTER EGG. Draws an image of an annoying person with an equally annoying message.  By the way, you look like you are trying to read this code. Would you like any help?
   bool returntostart();//Returns to the initial view.
   bool drawviewable(int imagetype);//Draw the viewable part of the image.
   void makerulerbox();//Make rectangle showing where the ruler is.
   void makedistancescale();//Make a scale for the LIDAR "map"
   void makecolourlegend();
   void drawoverlays();//Draw all of the above make methods.
   //Short, status changing methods:
   void setupprofile(){//Blocks pan signals and unblocks profile signals:
      sigpanstart.block();
      sigpan.block();
      sigpanend.block();
      sigprofstart.unblock();
      sigprof.unblock();
      sigprofend.unblock();
      if(is_realized())get_window()->set_cursor(*(new Gdk::Cursor(Gdk::CROSSHAIR)));
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
      if(is_realized())get_window()->set_cursor(*(new Gdk::Cursor(Gdk::CROSSHAIR)));
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
      if(is_realized())get_window()->set_cursor(*(new Gdk::Cursor(Gdk::CROSSHAIR)));
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
   void getprofile(double*& profxs,double*& profys,int& profps){//Get coordinates for profile (currently for slanted profile only).
      profbox->getboundaries(profxs,profys,profps);
   }
   void getfence(double*& fencexs,double*& fenceys,int&fenceps){//Get coordinates for fence.
      fencebox->getboundaries(fencexs,fenceys,fenceps);
   }
   bool getpausethread(){ return pausethread; }
   bool getthread_running(){ return thread_running; }
   BoxOverlay* getfencebox(){ return fencebox; }
   BoxOverlay* getprofbox(){ return profbox; }
   //Setters:
   void setshowprofile(double showprofile){ this->showprofile = showprofile;}//Set whether profile box should be seen when not being modified.
   void setshowfence(double showfence){ this->showfence = showfence;}//Set whether fence should be seen when not being modified.
   void setshowdistancescale(double showdistancescale){ this->showdistancescale = showdistancescale;}//Set whether the distance scale should be seen.
   void setshowlegend(double showlegend){ this->showlegend = showlegend;}//Set whether the colour legend should be seen.
   void setpausethread(bool pausethread){ this->pausethread = pausethread; }
   void setreversez(bool reversez){ this->reversez = reversez; }
   void setraiseline(bool raiseline){ this->raiseline = raiseline; }
   void setlinetoraise(int linetoraise){ this->linetoraise = linetoraise; }
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
   bool raiseline;
   int linetoraise;
   bool drawnsinceload;
   bool reversez;
   BoxOverlay* fencebox;
   BoxOverlay* profbox;
   bool showlegend;//Determines whether the colour legend should be drawn, if applicable.
   bool showdistancescale;//Determines whether to draw the distance scale overlay.
   bool drawneverything;//Stores whether all of the buckets were drawn the last time the drawing thread was invoked.
   int pointcount;//The number of points being drawn from the current bucket.
   float* vertices;//This contains the coordinates of all the points in the current bucket.
   float* colours;//This contains the RGB components of the colours of all the points in the current bucket.
   bool thread_existsmain;//This indicates whether a drawing thread currently exists.
   bool interruptthread;//This indicates whether to interrupt the current drawing thread.
   bool thread_existsthread;//This indicates whether a drawing thread currently exists.
   bool drawing_to_GL;//This indicates whether the main thread is/should/will be busy drawing the contents of the arrays vertices and colours to the framebuffer. If so, the drawing thread should pause.
   bool initialising_GL_draw;//This indicates whether the main thread is/should/will be busy setting up OpenGL for drawing. If so, the drawing thread should pause.
   bool flushing;//This indicates whether the main thread is/should/will be busy flushing the contents of the framebuffer to the screen.
   bool extraDrawing;//This indicates whether an extra draw signal, caused by a previous draw signal being blocked to avoid deadlock or race conditions, has been sent by the main thread to itself. If so, the main thread should not send any more until the current one is resolved.
   bool threaddebug;//If true, this causes a huge amount of spam to spew from the program whenever the drawing thread is in use, for debugging purposes.
   bool pausethread;//If this is true, the data manipulation thread will pause until it is set to false. This pausing is done because pointbucket::getpoint() is not threadsafe.
   bool thread_running;//This indicates that the thread is running, and so will be using the pointbucket::getpoint() method often.
   double drawnsofarminx,drawnsofarminy,drawnsofarmaxx,drawnsofarmaxy;//These define a rectangle that (very) roughly corresponds to how much of the flightline has been drawn so far. This is used for when panning, so that if there is not much already loaded then the bucket outlines are not completely covered by black.
   double centrexsafe,centreysafe;//These are "safe" stores of the coordinates of the centre of the screen. Safe in that they will not change (we hope) while the drawing thread is running. These are in object-wide scope, despite being used in multiple threads, because they are needed to draw the buckets properly when doing preview.
   Glib::Dispatcher signal_InitGLDraw;//Signal dispatcher from the drawing thread to the main thread to set up OpenGL for drawing.
   Glib::Dispatcher signal_EndGLDraw;//Signal dispatcher from the drawing thread to the main thread to clear up OpenGL after drawing is complete.
   Glib::Dispatcher signal_DrawGLToCard;//Signal dispatcher from the drawing thread to the main thread to draw the contents of the vertices and colours arrays to the framebuffer.
   Glib::Dispatcher signal_FlushGLToScreen;//Signal dispatcher from the drawing thread to the main thread to flush the contents of the framebuffer to the screen.
   Glib::Dispatcher signal_extraDraw;//Signal dispatcher from the main thread to itself to draw the points yet another time. This method is used as getting direct access to the variables affecting the main (GTK) thread would likely be very messy.
   Gtk::Label *rulerlabel;//Label showing the distance, in various dimensions, covered by the ruler.
   //Position variables:
   double centrex,centrey;//These give the centre of the viewport in image terms, rather than screen terms.
   double panstartx,panstarty;//Coordinates of the start of the pan move.
 
   //Profiling:
   bool profiling;//Determines whether or not the profile should be drawn.
   bool showprofile;//Whether to display the profile box when not modifying it.
   //Fencing:
   bool fencing;//Determines whether or not the fence should be drawn.
   bool showfence;//Whether to display the fence when not modifying it.
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
   void extraDraw();//Draw the points once again because a previous draw was blocked to avoid deadlocks and race conditions.
   void InitGLDraw();//Set up OpenGL for drawing.
   void EndGLDraw();//Clear up the OpenGL settings.
   void DrawGLToCard();//Draw the contents of the arrays vertices and colours to the framebuffer.
   void FlushGLToScreen();//Flush the contents of the framebuffer to the screen.
 
   //Drawing:
   void mainimage(pointbucket** buckets,int numbuckets,int detail);//Draw the main image. This is used by the drawing thread. Handle with care.
   bool drawbuckets(pointbucket** buckets,int numbuckets);//Draw the outlines of the buckets and, above them, the contents of the back buffer (which contains the last stuff drawn from scratch).
 
   //Positioning methods:
   void resetview();//Determines what part of the image is displayed with orthographic projection.
   bool on_zoom(GdkEventScroll* event);//Allows the user to zoom with the mouse wheel.
 
   bool pointinfo(double eventx,double eventy);//This returns some information about a selected point. Does not yet work properly.
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
