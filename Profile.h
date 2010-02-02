/*
 * File: Profile.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: December 2009 - January 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "quadtree.h"
#include "quadtreestructs.h"
#include <vector>
#include <boost/bind.hpp>
#include "Display.h"
class Profile : public Display{
public:
   Profile(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit,Gtk::Label *rulerlabel);
   ~Profile();
   void make_moving_average();
   bool returntostart();//Return to the initial view of the image.
   bool drawviewable(int imagetype);//Draw the viewable part of the image.
   //Public methods:
   void makerulerbox();//Make rectangle showing where the ruler is.
   bool showprofile(double startx,double starty,double endx,double endy,double width);//Gets the parameters of the profile and then draws it to the screen.
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
   //Setters:
   void setdrawpoints(bool drawpoints){this->drawpoints=drawpoints;}
   void setdrawmovingaverage(bool drawmovingaverage){this->drawmovingaverage=drawmovingaverage;}
   void setmavrgrange(int mavrgrange){this->mavrgrange=mavrgrange;}
protected:
   double minplanx,minplany;
   vector<int> flightlinestot;
   vector<point*>* flightlinepoints;
   double leftboundx,leftboundy,rightboundx,rightboundy;
   double** linez;
   int linezsize;
   bool drawpoints;//Determines whether points are drawn.
   bool drawmovingaverage;//Determines whether the best fit line is drawn.
   int mavrgrange;//Defines the range of the moving average, with 0 meaning no averaging.
   bool imageexists;//Determines whether to draw anything, based on the existance or nonexistance of anything to draw.
   Gtk::Label *rulerlabel;//Label showing the distance, in various dimensions, covered by the ruler.
   //Point data and related stuff:
   pointbucket** buckets;//This stores the buckets containing the points to be drawn.
   int numbuckets;//The number of buckets in buckets.
   bool** correctpointsbuckets;//This stores, for each point in each bucket, whether the point is inside the boundaries of the profile and, therefore, whether the point should be drawn.
 
   //Position variables:
   double centrex,centrey,centrez;//These give the centre of the viewport in image terms, rather than screen terms.
   double viewerx,viewery,viewerz;//These give the coordinates of the "eye", looking towards the centre.
   double panstartx,panstarty;//Coordinates of the start of the pan move.
   double startx,starty;//The start coordinates of the profile.
   double endx,endy;//The end coordinates of the profile.
   double width;//The width of the profile.
 
   //Rulering:
   double rulerstartx, rulerstarty,rulerstartz;//The start coordinates for the ruler.
   double rulerendx, rulerendy,rulerendz;//The end coordinates for the ruler.
   double rulerwidth;//The width of the ruler.
   bool rulering;//Determines whether or not the ruler should be drawn.
 
   //Signal handlers:
   //Panning:
   sigc::connection sigpanstart;
   sigc::connection sigpan;
   sigc::connection sigpanend;
   //Rulering:
   sigc::connection sigrulerstart;
   sigc::connection sigruler;
   sigc::connection sigrulerend;
 
   //Methods:
   int get_closest_element_position(point* value,vector<point*>::iterator first,vector<point*>::iterator last);
   bool linecomp(point* a,point* b);
 
   //Drawing:
   bool mainimage(pointbucket** buckets,int numbuckets,int detail);//Draw the main image
   bool previewimage(pointbucket** buckets,int numbuckets,int detail);//Draw the preview (for panning etc.).
 
   //Positioning methods:
   void resetview();//Determines what part of the image is displayed with orthographic projection.
   bool on_zoom(GdkEventScroll* event);//Allows the user to zoom with the mouse wheel.
   
   //Panning control:   //These allow the user to pan by clicking and dragging.
   bool on_pan_start(GdkEventButton* event);
   bool on_pan(GdkEventMotion* event);
   bool on_pan_end(GdkEventButton* event);
   //Rulering control:   //These allow the user to ruler by clicking and dragging.
   bool on_ruler_start(GdkEventButton* event);
   bool on_ruler(GdkEventMotion* event);
   bool on_ruler_end(GdkEventButton* event);

};
