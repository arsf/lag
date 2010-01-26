/*
 * File: TwoDeeOverview.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: November 2009 - January 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "quadtree.h"
#include "quadtreestructs.h"
#include <vector>
#include "Display.h"
#include "boost/thread.hpp"
class TwoDeeOverview : public Display{
public:
   TwoDeeOverview(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit,Gtk::Label *rulerlabelover);
   ~TwoDeeOverview();
   void clippy(string picturename);
   bool returntostart();//Returns to the initial view.
   bool drawviewable(int imagetype);//Draw the viewable part of the image.
   void makeprofbox();//Make the box showing the profile area.
   void makefencebox();//Make the box showing the fence area.
   void makerulerbox();//Make rectangle showing where the ruler is.
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
   void getprofile(double &startx,double &starty,double &endx,double &endy,double &width){//Get coordinates for profile.
      startx = profstartx;
      starty = profstarty;
      endx = profendx;
      endy = profendy;
      width = profwidth;
   }
   void getfence(double &minX,double &minY,double &maxX,double &maxY){//Get coordinates for fence.
      if(fencestartx<=fenceendx){
         minX = fencestartx;
         maxX = fenceendx;
      }
      else{
         minX = fenceendx;
         maxX = fencestartx;
      }
      if(fencestarty<=fenceendy){
         minY = fencestarty;
         maxY = fenceendy;
      }
      else{
         minY = fenceendy;
         maxY = fencestarty;
      }
   }
   //Setters:
   void setprofwidth(double profwidth){this->profwidth = profwidth;}//Set width of the profile.
   void setshowprofile(double showprofile){this->showprofile = showprofile;}//Set whether profile box should be seen when not being modified.
   void setshowfence(double showfence){this->showfence = showfence;}//Set whether fence should be seen when not being modified.
   void setheightenNonC(bool heightenNonC){this->heightenNonC = heightenNonC;}
   void setheightenGround(bool heightenGround){this->heightenGround = heightenGround;}
   void setheightenLowVeg(bool heightenLowVeg){this->heightenLowVeg = heightenLowVeg;}
   void setheightenMedVeg(bool heightenMedVeg){this->heightenMedVeg = heightenMedVeg;}
   void setheightenHighVeg(bool heightenHighVeg){this->heightenHighVeg = heightenHighVeg;}
   void setheightenBuildings(bool heightenBuildings){this->heightenBuildings = heightenBuildings;}
   void setheightenNoise(bool heightenNoise){this->heightenNoise = heightenNoise;}
   void setheightenMass(bool heightenMass){this->heightenMass = heightenMass;}
   void setheightenWater(bool heightenWater){this->heightenWater = heightenWater;}
   void setheightenOverlap(bool heightenOverlap){this->heightenOverlap = heightenOverlap;}
   void setheightenUndefined(bool heightenUndefined){this->heightenUndefined = heightenUndefined;}
protected:
   Gtk::Label *rulerlabel;//Label showing the distance, in various dimensions, covered by the ruler.
   //Position variables:
   double centrex,centrey;//These give the centre of the viewport in image terms, rather than screen terms.
   double panstartx,panstarty;//Coordinates of the start of the pan move.
 
   //Profiling:
   double profstartx, profstarty;//The start coordinates for the profile.
   double profendx, profendy;//The end coordinates for the profile.
   double profwidth;//The width of the profile.
   bool profiling;//Determines whether or not the profile should be drawn.
   bool showprofile;//Whether to display the profile box when not modifying it.
   //Fencing:
   double fencestartx, fencestarty;//The start coordinates for the fence.
   double fenceendx, fenceendy;//The end coordinates for the fence.
   bool fencing;//Determines whether or not the fence should be drawn.
   bool showfence;//Whether to display the fence when not modifying it.

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
 
   //Drawing:
   bool mainimage(pointbucket** buckets,int numbuckets,int detail);//Draw the main image
   bool previewimage(pointbucket** buckets,int numbuckets,int detail);//Draw the preview (for panning etc.).
 
   //Positioning methods:
   void resetview();//Determines what part of the image is displayed with orthographic projection.
   bool on_zoom(GdkEventScroll* event);//Allows the user to zoom with the mouse wheel.
 
   //Rulering:
   double rulerstartx,rulerstarty;//The start coordinates for the ruler
   double rulerendx,rulerendy;//The end coordinates for the ruler.
   double rulerwidth;//The width of the ruler.
   bool rulering;//Determines whether or not the ruler should be drawn.
 
   bool pointinfo(double eventx,double eventy);
   //Panning control:   //These allow the user to pan by clicking and dragging.
   bool on_pan_start(GdkEventButton* event);
   bool on_pan(GdkEventMotion* event);
   bool on_pan_end(GdkEventButton* event);
   //Profiling:
   bool on_prof_start(GdkEventButton* event);
   bool on_prof(GdkEventMotion* event);
   bool on_prof_end(GdkEventButton* event);
   //Fencing:
   bool on_fence_start(GdkEventButton* event);
   bool on_fence(GdkEventMotion* event);
   bool on_fence_end(GdkEventButton* event);
   //Rulering control:   //These allow the user to ruler by clicking and dragging.
   bool on_ruler_start(GdkEventButton* event);
   bool on_ruler(GdkEventMotion* event);
   bool on_ruler_end(GdkEventButton* event);
 
   
};
