/*
 ===============================================================================

 Profile.h

 Created on: December 2009
 Authors: Haraldur Tristan Gunnarsson, Berin Smaldon

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

#ifndef PROFILE_H
#define PROFILE_H

#include <gtkmm.h>
#include <gtkglmm.h>
#include "Quadtree.h"
#include "SelectionBox.h"
#include "PointBucket.h"
#include <boost/bind.hpp>
#include "LagDisplay.h"
#include "ProfileTypes.h"

#include <vector>
#include <list>
#include <utility>

/*
 ===============================================================================

 Profile - represents the drawing area of the profile window.

 ===============================================================================
 */
class Profile: public LagDisplay
{
   public:
      Profile(const Glib::RefPtr<const Gdk::GL::Config>& config, int bucketlimit, Gtk::Label *rulerlabel);

      ~Profile();

      void clearProfile();

      //This classifies the points selected by the profile fence.
      bool classify(Point fenceStart, Point fenceEnd, uint8_t classification);

      //Moves the view using keyboard input.
      bool on_pan_key(GdkEventKey *event, double scrollspeed);

      //Moved the fence using keyboard input.
      bool on_fence_key(GdkEventKey *event, double scrollspeed);

      //Zooms the view using keyboard input.
      bool on_zoom_key(GdkEventKey* event);

      // This creates an array of z values for the points in the profile 
      // that are derived from the real z values through a moving average. 
      // This results in a smoothed line.
      void make_moving_average();

      // Move the centre of the view (specifically its x and y components) and 
      // the fence coordinates so that shifted profiles are both visible and 
      // have visible and valid fences.
      bool shift_viewing_parameters(GdkEventKey* event, double shiftspeed);

      //Return to the initial view of the image.
      bool returntostart();

      //Draw the viewable part of the image.
      bool drawviewable(int imagetype);

      //Public methods:
      
      // Methods managing the classification queue
      bool queueActiveFence(uint8_t);
      bool hasClassifyJobs();
      void enqueueClassify(FenceType f, uint8_t c);
      ClassificationJob popNextClassify();

      void points_classified();

      // Clear the (one) fence being processed
      void clearProcessingFence();

      //Gets the parameters of the profile and then draws it to the screen.
      bool loadprofile(std::vector<double> profxs, std::vector<double> profys, int profps);

      bool draw_profile(bool changeview);

      void setupruler()
      {
         sigpanstart.block();
         sigpan.block();
         sigpanend.block();
         sigrulerstart.unblock();
         sigruler.unblock();
         sigrulerend.unblock();
         Gdk::Cursor cursor(Gdk::CROSSHAIR);
         if(is_realized())
            get_window()->set_cursor(cursor);
         rulering = true;
      }
      
      //Blocks ruler signals and unblocks pan signals:
      void unsetupruler()
      {
         sigpanstart.unblock();
         sigpan.unblock();
         sigpanend.unblock();
         sigrulerstart.block();
         sigruler.block();
         sigrulerend.block();
         if(is_realized())
            get_window()->set_cursor();
         rulering = false;
      }
      
      //Blocks pan signals and unblocks fence signals:
      void setupfence()
      {
         sigpanstart.block();
         sigpan.block();
         sigpanend.block();
         sigfencestart.unblock();
         sigfence.unblock();
         sigfenceend.unblock();
         Gdk::Cursor cursor(Gdk::CROSSHAIR);
         if(is_realized())
            get_window()->set_cursor(cursor);
         fencing = true;
      }
      
      //Blocks fence signals and unblocks pan signals:
      void unsetupfence()
      {
         sigpanstart.unblock();
         sigpan.unblock();
         sigpanend.unblock();
         sigfencestart.block();
         sigfence.block();
         sigfenceend.block();
         if(is_realized())
            get_window()->set_cursor();
         fencing = false;
      }
      
      //Setters:
      void setdrawpoints(bool drawpoints)
      {
         this->drawpoints = drawpoints;
      }
      
      void setdrawmovingaverage(bool drawmovingaverage)
      {
         this->drawmovingaverage = drawmovingaverage;
      }
      
      void setmavrgrange(int mavrgrange)
      {
         this->mavrgrange = mavrgrange;
      }
      
      void setpreviewdetail(double previewdetailmod)
      {
         this->previewdetailmod = previewdetailmod;
      }
      
      void setshowheightscale(bool showheightscale)
      {
         this->showheightscale = showheightscale;
      }
      
      void setslantwidth(double slantwidth)
      {
         this->slantwidth = slantwidth;
      }
      
      void setslanted(double slanted)
      {
         this->slanted = slanted;
      }
      void toggleNoise();

      std::vector<double> get_averages(bool exclude_noise);

      void set_slicing(bool slice)
      {
         this->slicing = slice;
      }
      
      void set_minz(double z)
      {
         this->minz = z;
      }
      
      void set_maxz(double z)
      {
         this->maxz = z;
      }
      
      Glib::Dispatcher sig_class_progress;

   protected:
      //Label showing the distance, in various dimensions, covered by the ruler.
      Gtk::Label *rulerlabel;

      // These contain the x and y coordinates, respectively, of the corners of 
      // the profile.
      vector<double> profxs, profys;

      //This contains the number of corners the profile has.
      int profps;

      //Control:
      //Determines whether points are drawn. 
      bool drawpoints;

      //Determines whether the best fit line is drawn.
      bool drawmovingaverage;

      // Determines whether to draw anything, based on the existance or 
      // nonexistance of anything to draw.
      bool imageexists;

      //Determines whether to draw the fence slanted or not.
      bool slanted;

      //Determines the width of the slanted fence.
      double slantwidth;

      //Determines whether to draw the height(Z) scale on the screen.
      bool showheightscale;

      // Point data and related stuff:
      // This is the total number of points in the profile. This is used to 
      // determine how many points to skip, along with the modifiers. The 
      // more points there are, the more points will be skipped.
      int totnumpoints;

      //Defines the range of the moving average, with 0 meaning no averaging.
      int mavrgrange;

      // This array contains, for each flightline (upper level), the z 
      // coordinates for each point after calculation of the moving average.
      double** linez;

      // It is necessary to store the size of linez in class scope because the 
      // old version of linez is deleted before the new version is produced, 
      // needing a record of the old length that will not fall out of scope.
      int linezsize;

      // This modifies the amount of points skipped for each point in the 
      // preview, when drawing. Lower means more detail, higher means less.
      double previewdetailmod;

      //This vector contains all the flightline numbers.
      vector<int> flightlinestot;

      // This is a pointer (array) of vectors of points, representing for each 
      // flightline (the elements of the array) the points that it contains (the 
      // vectors).
      vector<LidarPoint>* flightlinepoints;

      // Arrays for openGL input
      float* vertices;
      float* colours;

      // Largest known flightline size, used for assigning memory correctly
      int vertex_limit;

      //Store the maximum and minimum heights of the profile sample.
      double samplemaxz, sampleminz;

      //Position variables:
      // The boundary coordinates of the window, translated into world 
      // coordinates. Please note that the Z axis is "up", not the Y axis. 
      // The user cannot see the difference between the X and Y axes. 
      double leftboundx, leftboundy, rightboundx, rightboundy;

      // These give the centre of the viewport in image terms, rather than 
      // screen terms.
      //double centrex,centrey,centrez;
      //These give the coordinates of the "eye", looking towards the centre.
      Point viewer;

      // These indicate the "minimum" (i.e. left) coordinates of the 
      // viewable plane.
      Point minPlan;

      //The start coordinates of the profile.
      Point start;

      //The end coordinates of the profile.
      Point end;

      //The width of the profile.
      double width;

      //Rulering:
      //Determines whether or not the ruler should be drawn.
      bool rulering;

      //The start coordinates for the ruler.
      Point rulerStart;

      //The end coordinates for the ruler.
      Point rulerEnd;

      //The width of the ruler.
      double rulerwidth;

      //Fencing:
      
      // Co-ordinates of either side of the fence
      // first: start of fence
      // second: end of fence
      //pair <Point,Point> activeFence;
      FenceType activeFence;
      list<ClassificationJob> classificationQueue;

      Glib::Mutex classificationQueue_mutex;

      // number of classification jobs being processed at any given moment in time
      int classificationsHappening;

      // Values for the fence being processed by the (one) classification job
      bool isProcessingFence;
      FenceType processingFence;

      //Determines whether or not the fence should be drawn.
      bool fencing;

      Glib::Mutex profile_mainimage_mutex;

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
      
      // This determines which points in the bucket (bucket) fit both in the 
      // profile (from the correctpoints pointer passed in) and in the fence 
      // (the xs,ys,zs pointers and numcorners passed in) and classifies those 
      // that do (with the classification passed in).
      void classify_bucket(double *xs, double *ys, double *zs, int numcorners, bool *correctpoints, PointBucket* bucket, uint8_t classification);

      //Drawing:
      
      //Draw the main image
      bool mainimage(int detail);

      // Satisfy the compiler with a dummy function
      // TODO: This is temporary. Really, mainimage should be a universal way to
      // draw images in LagDisplay
      void mainimage(PointBucket** buckets, int numbuckets)
      {
      }
      
      // This takes a point (usually artificial) and then returns the point whose 
      // "distance along the viewing plane" value is the cloeset to it or, more 
      // strictly, the one whose value would mean it would be just before it if 
      // the given point was already part of the vector.
      int get_closest_element_position(LidarPoint value, vector<LidarPoint>::iterator first, vector<LidarPoint>::iterator last);

      //Draw all of the make methods indented below.
      void drawoverlays();

      //Make line showing where the ruler is.
      void makerulerbox();

      //Make rectangle showing where the fence is.
      void makefencebox(Point fenceStart, Point fenceEnd, Colour c);

      //This makes a height scale.
      void makeZscale();

      // This compares the two points passed to it and returns true if the first 
      // point is further from the "start line" of the plane than the second point.
      // It is used both to sort the points along the plane and to search for 
      // points along the plane (called from get_closest_element_position).
      bool linecomp(LidarPoint a, LidarPoint b);

      //Positioning methods:
      // Determines what part of the image is displayed with orthographic 
      // projection.
      void resetview();

      //Allows the user to zoom with the mouse wheel.
      bool on_zoom(GdkEventScroll* event);

      //Panning control:   
      //These allow the user to pan by clicking and dragging.
      bool on_pan_start(GdkEventButton* event);
      bool on_pan(GdkEventMotion* event);
      bool on_pan_end(GdkEventButton* event);

      //Rulering control:   
      //These allow the user to ruler by clicking and dragging.
      bool on_ruler_start(GdkEventButton* event);
      bool on_ruler(GdkEventMotion* event);
      bool on_ruler_end(GdkEventButton* event);

      //Fencing control:   
      //These allow the user to fence by clicking and dragging.
      bool on_fence_start(GdkEventButton* event);
      bool on_fence(GdkEventMotion* event);
      bool on_fence_end(GdkEventButton* event);

      // hide profile noisy points
      bool hideProfNoise;

      // Slicing
      bool slicing;
      double minz;
      double maxz;
};

#endif
