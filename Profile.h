/*
 * File: Profile.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: December 2009 - June 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "Quadtree.h"
#include "quadtreestructs.h"
#include "PointBucket.h"
#include <vector>
#include <boost/bind.hpp>
#include "Display.h"
#ifndef PROFILE_H
#define PROFILE_H
class Profile : public Display{
public:
   Profile(const Glib::RefPtr<const Gdk::GL::Config>& config,Quadtree* lidardata,int bucketlimit,Gtk::Label *rulerlabel);
   ~Profile();
   bool classify(uint8_t classification);//This classifies the points selected by the profile fence.
   void make_moving_average();//This creates an array of z values for the points in the profile that are derived from the real z values through a moving average. This results in a smoothed line.
   void drawoverlays();//Draw all of the make methods indented below.
      void makerulerbox();//Make line showing where the ruler is.
      void makefencebox();//Make rectangle showing where the fence is.
      void makeZscale();//This makes a height scale.
   bool returntostart();//Return to the initial view of the image.
   bool drawviewable(int imagetype);//Draw the viewable part of the image.
   //Public methods:
   bool showprofile(double* profxs,double* profys,int profps,bool changeview);//Gets the parameters of the profile and then draws it to the screen.
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
   //Setters:
   void setdrawpoints(bool drawpoints){ this->drawpoints=drawpoints; }
   void setdrawmovingaverage(bool drawmovingaverage){ this->drawmovingaverage=drawmovingaverage; }
   void setmavrgrange(int mavrgrange){ this->mavrgrange=mavrgrange; }
   void setpreviewdetail(double previewdetailmod){ this->previewdetailmod=previewdetailmod; }
   void setshowheightscale(bool showheightscale){ this->showheightscale = showheightscale; }
   void setslantwidth(double slantwidth){ this->slantwidth = slantwidth; }
   void setslanted(double slanted){ this->slanted = slanted; }
protected:
   Gtk::Label *rulerlabel;//Label showing the distance, in various dimensions, covered by the ruler.
   bool showheightscale;//Determines whether to draw the height(Z) scale on the screen.
   double samplemaxz,sampleminz;//Store the maximum and minimum heights of the profile sample.
   double *profxs,*profys;//These contain the x and y coordinates, respectively, of the corners of the profile.
   int profps;//This contains the number of corners the profile has.
   
   //Control:
   bool drawpoints;//Determines whether points are drawn.
   bool drawmovingaverage;//Determines whether the best fit line is drawn.
   bool imageexists;//Determines whether to draw anything, based on the existance or nonexistance of anything to draw.
   bool slanted;//Determines whether to draw the fence slanted or not.
   double slantwidth;//Determines the width of the slanted fence.
   
   //Point data and related stuff:
   int totnumpoints;//This is the total number of points in the profile. This is used to determine how many points to skip, along with the modifiers. The more points there are, the more points will be skipped.
   int mavrgrange;//Defines the range of the moving average, with 0 meaning no averaging.
   double** linez;//This array contains, for each flightline (upper level), the z coordinates for each point after calculation of the moving average.
   int linezsize;//It is necessary to store the size of linez in class scope because the old version of linez is deleted before the new version is produced, needing a record of the old length that will not fall out of scope.
   double previewdetailmod;//This modifies the amount of points skipped for each point in the preview, when drawing. Lower means more detail, higher means less.
   vector<int> flightlinestot;//This vector contains all the flightline numbers.
   vector<Point>* flightlinepoints;//This is a pointer (array) of vectors of points, representing for each flightline (the elements of the array) the points that it contains (the vectors).

   //Position variables:
   double leftboundx,leftboundy,rightboundx,rightboundy;//The boundary coordinates of the window, translated into world coordinates. Please note that the Z axis is "up", not the Y axis. The user cannot see the difference between the X and Y axes.
   double centrex,centrey,centrez;//These give the centre of the viewport in image terms, rather than screen terms.
   double viewerx,viewery,viewerz;//These give the coordinates of the "eye", looking towards the centre.
   double panstartx,panstarty;//Coordinates of the start of the pan move.
   double minplanx,minplany;//These indicate the "minimum" coordinates of the viewable plane.
   double startx,starty;//The start coordinates of the profile.
   double endx,endy;//The end coordinates of the profile.
   double width;//The width of the profile.
 
   //Rulering:
   double rulerstartx,rulerstarty,rulerstartz;//The start coordinates for the ruler.
   double rulerendx,rulerendy,rulerendz;//The end coordinates for the ruler.
   double rulerwidth;//The width of the ruler.
   bool rulering;//Determines whether or not the ruler should be drawn.
   //Fencing:
   double fencestartx,fencestarty,fencestartz;//The start coordinates for the fence.
   double fenceendx,fenceendy,fenceendz;//The end coordinates for the fence.
   bool fencing;//Determines whether or not the fence should be drawn.
 
   //Signal handlers:
   //Panning:
   sigc::connection sigpanstart;
   sigc::connection sigpan;
   sigc::connection sigpanend;
   //Rulering:
   sigc::connection sigrulerstart;
   sigc::connection sigruler;
   sigc::connection sigrulerend;
   //Fencing:
   sigc::connection sigfencestart;
   sigc::connection sigfence;
   sigc::connection sigfenceend;
 
   //Methods:
 
   //Drawing:
   bool mainimage(int detail);//Draw the main image
   int get_closest_element_position(Point* value,vector<Point>::iterator first,vector<Point>::iterator last);//This takes a point (usually artificial) and then returns the point whose "distance along the viewing plane" value is the cloeset to it or, more strictly, the one whose value would mean it would be just before it if the given point was already part of the vector.
   bool linecomp(const Point& a,const Point& b);//This compares the two points passed to it and returns true if the first point is further from the "start line" of the plane than the second point. It is used both to sort the points along the plane and to search for points along the plane (called from get_closest_element_position).
 
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
   //Fencing control:   //These allow the user to fence by clicking and dragging.
   bool on_fence_start(GdkEventButton* event);
   bool on_fence(GdkEventMotion* event);
   bool on_fence_end(GdkEventButton* event);

};

#endif
