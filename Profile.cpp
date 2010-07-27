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
 * File: Profile.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: December 2009 - July 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <vector>
#include <iostream>
#include "Quadtree.h"
#include "QuadtreeStructs.h"
#include "PointBucket.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glc.h>
#include "Profile.h"
#include "MathFuncs.h"

Profile::Profile(const Glib::RefPtr<const Gdk::GL::Config>& config,Quadtree* lidardata,int bucketlimit,Gtk::Label *rulerlabel)  : Display(config,lidardata,bucketlimit){
   //Profile stats:
   samplemaxz = sampleminz = 0;
   profxs=profys=NULL;
   profps=0;
   viewerz = 0;
   startx = 0;
   starty = 0;
   totnumpoints = 0;
   //Initialisation:
   flightlinepoints = NULL;
   linez = NULL;
   //Drawing control:
   zoompower = 0.7;
   imageexists = false;
   drawpoints = true;
   maindetailmod = 0;
   previewdetailmod = 0.3;
   drawmovingaverage = false;
   mavrgrange = 5;
   showheightscale = false;
   //Rulering:
   rulering = false;
   rulerwidth = 3;
   this->rulerlabel = rulerlabel;
   rulerstartx = rulerstarty = rulerstartz = 0;
   rulerendx = rulerendy = rulerendz = 0;
   //Fencing:
   fencing=false;
   fencestartx = fencestarty = fencestartz = 0;
   fenceendx = fenceendy = fenceendz = 0;
   slanted = true;
   slantwidth = 5;
   //Events and signals:
   add_events(Gdk::SCROLL_MASK   |   Gdk::BUTTON1_MOTION_MASK   |   Gdk::BUTTON2_MOTION_MASK   |   Gdk::BUTTON_PRESS_MASK   |   Gdk::BUTTON_RELEASE_MASK);
   signal_scroll_event().connect(sigc::mem_fun(*this,&Profile::on_zoom));
      //Panning:
      sigpanstart = signal_button_press_event().connect(sigc::mem_fun(*this,&Profile::on_pan_start));
      sigpan = signal_motion_notify_event().connect(sigc::mem_fun(*this,&Profile::on_pan));
      sigpanend = signal_button_release_event().connect(sigc::mem_fun(*this,&Profile::on_pan_end));
      //Rulering:
      sigrulerstart = signal_button_press_event().connect(sigc::mem_fun(*this,&Profile::on_ruler_start));
      sigruler = signal_motion_notify_event().connect(sigc::mem_fun(*this,&Profile::on_ruler));
      sigrulerend = signal_button_release_event().connect(sigc::mem_fun(*this,&Profile::on_ruler_end));
      sigrulerstart.block();
      sigruler.block();
      sigrulerend.block();
      //Fencing:
      sigfencestart = signal_button_press_event().connect(sigc::mem_fun(*this,&Profile::on_fence_start));
      sigfence = signal_motion_notify_event().connect(sigc::mem_fun(*this,&Profile::on_fence));
      sigfenceend = signal_button_release_event().connect(sigc::mem_fun(*this,&Profile::on_fence_end));
      sigfencestart.block();
      sigfence.block();
      sigfenceend.block();
}

Profile::~Profile(){
   if(profxs!=NULL)delete[]profxs;
   if(profys!=NULL)delete[]profys;
   if(flightlinepoints!=NULL)delete[]flightlinepoints;
   if(linez!=NULL)delete[]linez;
}

//Firstly, this determines the boundary of the viewable area in world coordinates (for use by the drawing method(s)). It then sets the active matrix to that of projection and makes it the identity matrix, and then defines the limits of the viewing area from the dimensions of the window. *ratio*zoomlevel is there to convert screen dimensions to image dimensions. gluLookAt is then used so that the viewpoint is that of seeing the centre from a position to the right of the profile, when looking from the start to the end of it.
void Profile::resetview(){
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   leftboundx = -((get_width()/2)*ratio/zoomlevel) * breadth / length;//This part determines the boundary coordinates in world coordinates. Please note that only Z is "up"; both X and Y have an influence on "left" and "right".
   rightboundx = ((get_width()/2)*ratio/zoomlevel) * breadth / length;//...
   leftboundy = -((get_width()/2)*ratio/zoomlevel) * height / length;//...
   rightboundy = ((get_width()/2)*ratio/zoomlevel) * height / length;//...
   glMatrixMode(GL_PROJECTION);//Switching to projection matrix for defining the projection.
   glLoadIdentity();//We must do this (set the currently active matrix, the projection matrix, to the identity matrix) because the other modifications we can do are all, in fact, multiplication operations on the existing matrix.
   glOrtho(-(get_width()/2)*ratio/zoomlevel,//This sets the boundaries of the viewable area and then...
           +(get_width()/2)*ratio/zoomlevel,//...resizes everything to fit those boundaries.
           -(get_height()/2)*ratio/zoomlevel,//Strictly speaking, this sets the scale of the viewport to the "world".
           +(get_height()/2)*ratio/zoomlevel,//The four "flat" corners (the first four) are the "world" coordinates of...
           -5*width,//...the viewport corners. Thew last two parameters are basically just the "depth" of our view.
           +5*width);//We want to make sure we do not cut out stuff we want to draw. A 5*width margin gives much room.
   glMatrixMode(GL_MODELVIEW);//Switching to modelview matrix for defining the viewpoint. Be aware that the projection matrix should not be used for this and, if you DO use the projection matrix, everything will be reversed (I think).
   glLoadIdentity();
   gluLookAt(viewerx,viewery,viewerz,//Since viewerz is always 0, we are setting this so that the profile looks from the...
             0,0,0,//...right hand side of the profile (if seen from start to end) to the centre...
             0,0,1);//...Also, the Z direction is "up".
}

//Depending on the imagetype requested, this sets the detail level and then calls one of the image methods, which actually draws the data to the screen. The passed value should be 1 for the main image, 2 for the preview and 3 for the expose event (which is the same as the preview). Note that if the imagetype is anything other than 1, 2 or 3 then all points will be drawn.
bool Profile::drawviewable(int imagetype){
   if(!imageexists){//If there is an attempt to draw with no data, the program will probably crash.
      clearscreen();
      return false;
   }
   guard_against_interaction_between_GL_areas();//This is done to prevent interference from the overview window, which might cause changes in point size, area being drawn or might even cause the profile to be drawn in the wrong window!
   int detail=1;//This determines how many points are skipped between reads.
   //If there are very few points on the screen, show them all (note that if the imagetype is anything other than 1, 2 or 3 then all points will be drawn):
   if(imagetype==1)detail=(int)(totnumpoints*maindetailmod/100000);//Main image.  
   else if(imagetype==2||imagetype==3)detail=(int)(totnumpoints*previewdetailmod/100000);//Preview.
   mainimage(detail);//The image is now drawn.
   return true;
}

//This is called by a "reset button". It returns the view to the initial one. It sets the centre of the screen to the centre of the profile and then sets the viewer position and the ratio of world coordinates to window coordinates so that all of the porifle is visible before resetting the view and then drawing.
bool Profile::returntostart(){
   centrex = (startx + endx)/2;//This way, all of the profile should be on-screen.
   centrey = (starty + endy)/2;//...
   centrez = (samplemaxz + sampleminz)/2;//...
   zoomlevel=1;//...
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   viewerx = width * height / length;//To the right when looking from start to end.
   viewery = -width * breadth / length;//...
   ratio = length/get_parent()->get_width();//This makes sure that, at the initial view when zoomlevel==1, all of the profile points are visible.
   double Z = samplemaxz - sampleminz;//...
   if(ratio<Z/get_parent()->get_height())ratio = Z/get_parent()->get_height();//...
   ratio*=1.1;//This allows for some comfortable white (actually black) space between the edgemost points and the edges themselves.
   resetview();//Now change the view settings using some of the values just changed.
   return drawviewable(1);
}

//This shifts the centre and fence coordinates so that they stay the same relative to the profile when the profile is moved with the keyboard.
bool Profile::shift_viewing_parameters(GdkEventKey* event,double shiftspeed){
   shiftspeed *= 0.1*width;//The 0.1 is in there because the profile itself will also have moded at 1/10th speed.
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   double sameaxis = shiftspeed*breadth/length;//Where "up" and "forward" are supposed to be the same, these account for moving a slanted profile.
   double diffaxis = -shiftspeed*height/length;//...
   switch(event->keyval){
      case GDK_W:centrex += diffaxis;centrey += sameaxis;
                 fencestartx += diffaxis;fencestarty += sameaxis;
                 fenceendx += diffaxis;fenceendy += sameaxis;break;
      case GDK_S:centrex -= diffaxis;centrey -= sameaxis;
                 fencestartx -= diffaxis;fencestarty -= sameaxis;
                 fenceendx -= diffaxis;fenceendy -= sameaxis;break;
      case GDK_A:centrey += diffaxis;centrex -= sameaxis;
                 fencestarty += diffaxis;fencestartx -= sameaxis;
                 fenceendy += diffaxis;fenceendx -= sameaxis;break;
      case GDK_D:centrey -= diffaxis;centrex += sameaxis;
                 fencestarty -= diffaxis;fencestartx += sameaxis;
                 fenceendy -= diffaxis;fenceendx += sameaxis;break;
      default:return false;break;
   }
   resetview();//Now change the view settings using some of the values just changed.
   return true;
}

//This method prepares the profile for drawing and then draws. It first defines the parameters of the new profile and then grabs a subset of the quadtree of which some of the points may be in the profile. After that it determines what flightlines are in the profile and then adds all appropriate points to the profile by flightline before sorting them by flightline so that it can then constuct the moving averages of the flightlines. It then draws the profile. changeview should be true when the profile area has changed and false when it has not, such as when the classification (only) has been changed. If changeview is true then the view is reset and the fence is removed, otherwise not. The fence is removed to prevent accidental classification.
/*
 * Define profile parameters.
 * Get subset.
 * If subset is empty or NULL, delete subset and make so nothing will be drawn. Return.
 * For every cached bucket:
 *    For every point within the profile and within the cached bucket:
 *       If the flightline is not already recorded, record it.
 * For every uncached bucket:
 *    For every point within the profile and within the uncached bucket:
 *       If the flightlije is not already recorded, record it.
 * For every recorded flightline:
 *    For every cached bucket:
 *       For every point within the profile and the flightline and the cached bucket:
 *          Add the point to the profile.
 *          Update the minimum and maximum heights.
 *    For every uncached bucket:
 *       For every point within the profile and the flightline and the uncached bucket:
 *          Add the point to the profile.
 *          Update the minimum and maximum heights.
 *    Sort the points in the flightline.
 * If there are no points in the profile, make so nothing will be drawn. Return.
 * Make the moving averages.
 * Draw the profile.
 *
 * */
bool Profile::showprofile(double* profxs,double* profys,int profps,bool changeview){
   //Defining profile parameters (used elsewhere only):{
      startx = (profxs[0]+profxs[1])/2;//Used in many places.
      starty = (profys[0]+profys[1])/2;//...
      endx = (profxs[profps-1]+profxs[profps-2])/2;//...
      endy = (profys[profps-1]+profys[profps-2])/2;//...
      width = sqrt((profxs[0]-profxs[1])*(profxs[0]-profxs[1])+(profys[0]-profys[1])*(profys[0]-profys[1]));//...
      minplanx = startx;//These are the initial values, as the initial position of the viewing area will be defined by the start and end coordinates of the profile.
      minplany = starty;//...
      this->profps = profps;//These are used in the classify() method to define a parallelepiped (more precisely a monoclinic or parallelogram prism as only the fence parallelogram can be non-rectangular), which, when the fence is not "slanted", will also be a cuboid. This paralleogram prism (from the point of view of the fence (as cross-section) especially) contains the points to be classified.
      if(this->profxs!=NULL)delete[]this->profxs;//...
      if(this->profys!=NULL)delete[]this->profys;//...
      this->profxs = new double[this->profps];//...
      this->profys = new double[this->profps];//...
      for(int i = 0;i < this->profps;i++){//...Please note that it is done this way and not simply assigned so that the profile never deletes attributes of the overview.
         this->profxs[i] = profxs[i];//...
         this->profys[i] = profys[i];///..
      }//...
   //...}
   vector<PointBucket*> *pointvector = NULL;
   imageexists = advsubsetproc(pointvector,profxs,profys,profps);//Get data.
   if(!imageexists){//Drawing from a null vector would be bad, and a zero vector pointless. imageexists being false will prevent drawing.
      if(pointvector!=NULL)delete pointvector;
      return false;
   }
   int numbuckets = pointvector->size();
   bool** correctpointsbuckets = new bool*[numbuckets];//This stores, for each point in each bucket, whether the point is inside the boundaries of the profile and, therefore, whether the point should be drawn.
   bool *queriedbucketsarray = new bool[numbuckets];//This stores, for each bucket, whether the bucket has been accessed while already in cache. This is to make loading a profile a little faster.
   for(int i = 0;i < numbuckets;i++)queriedbucketsarray[i] = false;
   //Determine how many and which flightlines are represented in the profile:{
      flightlinestot.clear();
      for(int i=0;i<numbuckets;i++)if((*pointvector)[i]->getIncacheList()[0]){//For every cached bucket:
         queriedbucketsarray[i] = true;//Record as cached.
         correctpointsbuckets[i] = vetpoints((*pointvector)[i],profxs,profys,profps);//Determine whether the points in this bucket are within the profile.
         for(int j=0;j<(*pointvector)[i]->getNumberOfPoints(0);j++){//For all points in the bucket:
            if(correctpointsbuckets[i][j]){//If the point is within the profile:
               if(find(flightlinestot.begin(),flightlinestot.end(),(*pointvector)[i]->getPoint(j,0).flightLine) == flightlinestot.end()){//If the flightline number does not already exist in flightlinestot...
                  flightlinestot.push_back((*pointvector)[i]->getPoint(j,0).flightLine);//...add it.
               }
            }
         }
      }
      for(int i=0;i<numbuckets;i++)if(!queriedbucketsarray[i]){//For every bucket not originally cached:
         correctpointsbuckets[i] = vetpoints((*pointvector)[i],profxs,profys,profps);//Determine whether the points in this bucket are within the profile.
         for(int j=0;j<(*pointvector)[i]->getNumberOfPoints(0);j++){//For all points in the bucket:
            if(correctpointsbuckets[i][j]){//If the point is within the profile:
               if(find(flightlinestot.begin(),flightlinestot.end(),(*pointvector)[i]->getPoint(j,0).flightLine)==flightlinestot.end()){//If the flightline number does not already exist in flightlinestot...
                  flightlinestot.push_back((*pointvector)[i]->getPoint(j,0).flightLine);//...add it.
               }
            }
         }
      }
   //...}
   //Add appropriate points into the profile by flightline and sort for each flightline:{
      if(flightlinepoints!=NULL)delete[] flightlinepoints;
      flightlinepoints = new vector<Point>[flightlinestot.size()];//This pointer array of vectors will contain all the points in the profile.
      totnumpoints = 0;
      samplemaxz = rminz;//These are for the minimum and maximum heights of the points in the profile.
      sampleminz = rmaxz;//...
      for(int i=0;i<(int)flightlinestot.size();i++){//For every flightline:
         for(int j = 0;j < numbuckets;j++)queriedbucketsarray[j] = false;//Reset this so it can be properly used again.
         for(int j=0;j<numbuckets;j++)if((*pointvector)[j]->getIncacheList()[0]){//For every bucket already cached:
            queriedbucketsarray[j] = true;//Record as cached.
            for(int k=0;k<(*pointvector)[j]->getNumberOfPoints(0);k++){//For every point:
               if(correctpointsbuckets[j][k]){//If the point is in the profile...
                  if((*pointvector)[j]->getPoint(k,0).flightLine == flightlinestot[i]){//...and if it is from the right flightline (see above):
                     flightlinepoints[i].push_back((*pointvector)[j]->getPoint(k,0));//Add it
                     totnumpoints++;//...and add it to the "census" and...
                     if(samplemaxz<(*pointvector)[j]->getPoint(k,0).z)samplemaxz = (*pointvector)[j]->getPoint(k,0).z;//...modify the maximum and minimum heights if appropriate.
                     if(sampleminz>(*pointvector)[j]->getPoint(k,0).z)sampleminz = (*pointvector)[j]->getPoint(k,0).z;//...
                  }
               }
            }
         }
         for(int j=0;j<numbuckets;j++)if(!queriedbucketsarray[j]){//For every bucket not already cached:
            for(int k=0;k<(*pointvector)[j]->getNumberOfPoints(0);k++){//For every point:
               if(correctpointsbuckets[j][k]){//If the point is in the profile...
                  if((*pointvector)[j]->getPoint(k,0).flightLine == flightlinestot[i]){//...and if it is from the right flightline (see above):
                     flightlinepoints[i].push_back((*pointvector)[j]->getPoint(k,0));//Add it
                     totnumpoints++;//...and add it to the "census" and...
                     if(samplemaxz<(*pointvector)[j]->getPoint(k,0).z)samplemaxz = (*pointvector)[j]->getPoint(k,0).z;//...modify the maximum and minimum heights if appropriate.
                     if(sampleminz>(*pointvector)[j]->getPoint(k,0).z)sampleminz = (*pointvector)[j]->getPoint(k,0).z;//...
                  }
               }
            }
         }
         sort(flightlinepoints[i].begin(),flightlinepoints[i].end(),boost::bind(&Profile::linecomp,this,_1,_2));//Sort so that lines are intelligible and right. Otherwise when the user elects to draw lines they will get a chaotic scribble.
      }
   //...}
   delete[]queriedbucketsarray;
   if(pointvector!=NULL)delete pointvector;
   for(int i=0;i<numbuckets;i++)delete[] correctpointsbuckets[i];
   delete[] correctpointsbuckets;
   if(totnumpoints < 1){//If there are no points within the profile area, even if there were in the subset taken, then nothing should be drawn.
      imageexists = false;//This will ensure that nothing is drawn.
      return false;
   }
   make_moving_average();//Make now the lines to be drawn when the user elects to draw them.
   if(is_realized()){//If an attempt to draw is made when the widget is not yet attached to the GUI then there will be a segfault.
      if(changeview){//If the view is to be changed (like for when a totally new profile is made at possibly a different angle):
         fencestartx = fencestarty = fencestartz = 0;//Reset the fence to prevent the situation where a fence is preserved from profile to profile in a warped fashion allowing accidental classification.
         fenceendx = fenceendy = fenceendz = 0;//...
         return returntostart();
      }
      else return drawviewable(1);//Otherwise trust that any changes (like with scrolling) are dealt with or that there are no position or viewpoint changes (like with classification).
   }
   else return false;
}

//This determines which points in the bucket (bucket) fit both in the profile (from the correctpoints pointer passed in) and in the fence (the xs,ys,zs pointers and numcorners passed in) and classifies those that do (with the classification passed in).
void Profile::classify_bucket(double *xs,double *ys,double *zs,int numcorners,bool *correctpoints,PointBucket* bucket,uint8_t classification){
   bool pointinboundary;//Determines whether the point is within the boundary.
   int lastcorner;//These define the edge being considered.
   Point *pnt = new Point;//Fake point for sending to linecomp the boundaries of the fence.
   for(int i = 0;i < bucket->getNumberOfPoints(0);i++){//For all points:
      if(correctpoints[i]){//If in the profile area:
         pointinboundary = false;//Zero is an even number, so if the point is to the right of an edge of the boundary zero times, it cannot be within it.
         lastcorner = numcorners - 1;//Initially the last corner is looped back.
         for(int j = 0;j < numcorners;j++){//For every edge:
            if((zs[j] < bucket->getPoint(i,0).z && zs[lastcorner] >= bucket->getPoint(i,0).z) ||
               (zs[lastcorner] < bucket->getPoint(i,0).z && zs[j] >= bucket->getPoint(i,0).z)){//This segments the line to the length of the segment that helps define the boundary. That segment is the same in Z as the total Z range of the fence.
               pnt->x = xs[j] + ((bucket->getPoint(i,0).z - zs[j])/(zs[lastcorner] - zs[j])) * (xs[lastcorner] - xs[j]);//These make the fake point be on one of the edges of the fence and be the same height (z) as the point it is to be compared against. This allows comparison to see whether the point is wthing the box or not.
               pnt->y = ys[j] + ((bucket->getPoint(i,0).z - zs[j])/(zs[lastcorner] - zs[j])) * (ys[lastcorner] - ys[j]);//...
               if(linecomp(bucket->getPoint(i,0),*pnt))pointinboundary = !pointinboundary;//If the point is to the right of (i.e. further along than) the line defined by the corners (and segmented by the above if statement), i.e. the edge, then change the truth value of this boolean. If this is done an odd number of times then the point must be within the shape, otherwise without.
            }
            lastcorner = j;
         }
         if(pointinboundary)bucket->setClassification(i,classification);//Finally!
      }
   }
   delete pnt;
}
//This takes the points selected by the fence and then classifies them as the type sent.
bool Profile::classify(uint8_t classification){
   if(!imageexists || fencestartz == fenceendz || (fencestartx == fenceendx && fencestarty == fenceendy))return false;//If there should not be any points there or if the fence coveres no area/volume, do nothing. Otherwise get a divide by zero error in the latter case.
   vector<PointBucket*> *pointvector = NULL;
   bool gotdata = advsubsetproc(pointvector,profxs,profys,profps);//Get data using profile parameters.
   if(!gotdata){//Obviously if no data is retrieved then nothing more must be attempted.
      if(pointvector!=NULL)delete pointvector;
      return false;
   }
   int numbuckets = pointvector->size();
   bool** correctpointsbuckets = new bool*[numbuckets];//This stores, for each point in each bucket, whether the point is inside the boundaries of the profile and, therefore, whether the point should classified.
   for(int i=0;i<numbuckets;i++)correctpointsbuckets[i] = vetpoints((*pointvector)[i],profxs,profys,profps);//Store whether the points in the buckets are inside the boundaries of the profile.
   double *xs,*ys,*zs;//These will contain the corner coordinates of the fence.
   xs = new double[4];
   ys = new double[4];
   zs = new double[4];
   int numcorners = 4;
   if(slanted){
      double breadth = fenceendx - fencestartx;
      double height = fenceendy - fencestarty;
      double deltaz = fenceendz - fencestartz;
      double horiz = sqrt(breadth*breadth + height*height);//Right triangle
      double length = sqrt(breadth*breadth + height*height + deltaz*deltaz);//Right triangle in 3D! Half a cuboid!
      double horizratio = horiz/length;//This will be used for the "vertical" Z values because the corners are separated from the start and end points by vectors at right angles to the vector from the start and end point of the fence. Since the right angle to the Z axis that we are interested in is some combination of X and Y, this is the ratio we use.
      double deltazratio = deltaz/length;//...and analogously for this one.
      /*
       *                   0
       *                   /\____ 
       *                  /      \____ 
       *                 /p1          \____3
       *                /slantwidth       / 
       *              1/                 /
       *               \____            /p2
       *                    \____      /slantwidth
       *                         \____/
       *                              2
       *         p1 = fencestart
       *         p2 = fenceend
       * */
      xs[0] = fencestartx - (slantwidth/2)*deltazratio*breadth/horiz;//Please note that we use slantwidth here instead of width as this only affects the fence's cross-sectional area and has NO effect on the thickness of the classified area.
      xs[1] = fencestartx + (slantwidth/2)*deltazratio*breadth/horiz;//...
      xs[2] = fenceendx + (slantwidth/2)*deltazratio*breadth/horiz;//...
      xs[3] = fenceendx - (slantwidth/2)*deltazratio*breadth/horiz;//...
      ys[0] = fencestarty - (slantwidth/2)*deltazratio*height/horiz;//...
      ys[1] = fencestarty + (slantwidth/2)*deltazratio*height/horiz;//...
      ys[2] = fenceendy + (slantwidth/2)*deltazratio*height/horiz;//...
      ys[3] = fenceendy - (slantwidth/2)*deltazratio*height/horiz;//...
      zs[0] = fencestartz + (slantwidth/2)*horizratio;//...
      zs[1] = fencestartz - (slantwidth/2)*horizratio;//...
      zs[2] = fenceendz - (slantwidth/2)*horizratio;//...
      zs[3] = fenceendz + (slantwidth/2)*horizratio;//...
   }
   else{
      xs[0] = fencestartx;
      xs[1] = fencestartx;
      xs[2] = fenceendx;
      xs[3] = fenceendx;
      ys[0] = fencestarty;
      ys[1] = fencestarty;
      ys[2] = fenceendy;
      ys[3] = fenceendy;
      zs[0] = fencestartz;
      zs[1] = fenceendz;
      zs[2] = fenceendz;
      zs[3] = fencestartz;
   }
   bool *classifiedbucketsarray = new bool[numbuckets];//This stores, for each bucket, whether the bucket has been accessed while already in cache. This is to make classification a little faster.
   for(int i = 0;i < numbuckets;i++)classifiedbucketsarray[i] = false;
   for(int i=0;i<numbuckets;i++)if((*pointvector)[i]->getIncacheList()[0]){//For all buckets already cached:
      classifiedbucketsarray[i] = true;
      classify_bucket(xs,ys,zs,numcorners,correctpointsbuckets[i],(*pointvector)[i],classification);
   }
   for(int i=0;i<numbuckets;i++)if(!classifiedbucketsarray[i]){//For all buckets not already cached:
      classify_bucket(xs,ys,zs,numcorners,correctpointsbuckets[i],(*pointvector)[i],classification);
   }
   delete[]classifiedbucketsarray;
   delete zs;
   delete ys;
   delete xs;
   for(int i=0;i<numbuckets;i++)delete[] correctpointsbuckets[i];
   delete[] correctpointsbuckets;
   if(pointvector!=NULL)delete pointvector;
   int tempps = profps;//Make temporary copies of these to send to showprofile to avoid the situation where shoprofile deletes the arrays it is about to copy!
   double* tempxs = new double[tempps];//...
   double* tempys = new double[tempps];//...
   for(int i = 0;i < tempps;i++){//...
      tempxs[i] = profxs[i];//...
      tempys[i] = profys[i];//...
   }//...
   showprofile(tempxs,tempys,tempps,false);//Reload the modified points from the quadtree without changing the view or resetting the fence etc..
   delete[]tempxs;
   delete[]tempys;
   return true;
}

//This method is used by sort() and get_closest_element_position(). It projects the points onto a plane defined by the z axis and the other line perpendicular to the viewing direction. It then returns whether the first point is "further along" the plane than the second one, with one of the edges of the plane being defined as the "start" (the left edge as the user sees it). Essentially, if a is to the right of b it returns true, otherwise false.
bool Profile::linecomp(const Point &a,const Point &b){
   const double xa = a.x;
   const double xb = b.x;
   const double ya = a.y;
   const double yb = b.y;
   double alongprofa,alongprofb;
   if(startx==endx){//If the profile is parallel to the y axis:
      double mult=-1;//Used so that points are projecting onto the correct side (NOT face, but left or right) of the plane.
      if(starty<endy)mult=1;
      alongprofa = mult * (ya - minplany);
      alongprofb = mult * (yb - minplany);
   }
   else if(starty==endy){//If the profile is parallel to the x axis:
      double mult=-1;//Used so that points are projecting onto the correct side (NOT face, but left or right) of the plane.
      if(startx<endx)mult=1;
      alongprofa = mult * (xa - minplanx);
      alongprofb = mult * (xb - minplanx);
   }
   else{//If the profile is skewed:
      double breadth = endx - startx;
      double height = endy - starty;
      double multx=-1;//Used so that points are projecting onto the correct side (NOT face, but left or right) of the plane.
      if(startx<endx)multx=1;
      double multy=-1;//Used so that points are projecting onto the correct side (NOT face, but left or right) of the plane.
      if(starty<endy)multy=1;
      //Gradients of the profile and point-to-profile lines:
      double lengradbox = multx * multy * height / breadth;//Profile line gradient
      double widgradbox = -1.0 / lengradbox;//Point-to-profile lines gradient
      double widgradboxa = multy * (ya - minplany) - (multx * (xa - minplanx) * widgradbox);//Constant values (y intercepts) of the formulae for lines from each point to the profile line.
      double widgradboxb = multy * (yb - minplany) - (multx * (xb - minplanx) * widgradbox);//...
      //Identify the points of intercept for each point-to-profile line and the profile line and find the distance along the profile line:{
         /*0 (adjusted origin)
          * \ Profile line       ____/p
          *  \              ____/ Point line
          *   \        ____/
          *    \  ____/
          *  ___\/P
          * /    \
          *       \
          *        \
          *                              
          *  For point p:
          *     x of P is interxp
          *     y of P is interyp
          *     z is ignored (or "swept along")
          *     alongprofp is sqrt(interxp^2 + interyp^2), i.e. Pythagoras to find distance along the profile i.e distance from the adjusted origin.
          *
          * */
         double interxa,interxb,interya,interyb;
         interxa = widgradboxa / (widgradbox - lengradbox);//The x (intercept with plane) value of the line from the point a to the plane.
         interya = interxa * lengradbox;//The y (intercept with plane) value of the line from the point a to the plane.
         interxb = widgradboxb / (widgradbox - lengradbox);//The x (intercept with plane) value of the line from the point b to the plane.
         interyb = interxb * lengradbox;//The y (intercept with plane) value of the line from the point b to the plane.
         alongprofa = sqrt(interxa*interxa+interya*interya);//Use the values of x and y as well as pythagoras to find position along non-z axis of the plane.
         alongprofb = sqrt(interxb*interxb+interyb*interyb);//Use the values of x and y as well as pythagoras to find position along non-z axis of the plane.
      //...}
   }
   return alongprofa > alongprofb;
}
//This returns the index (in the vector of points in a flightline) of the nearest point "before" the position along the horizontal line (from the viewable plane) of the point passed in. It is used for determining which points to draw by passing separately as "points" the coordinates of the limits of the viewable plane. It needs to be passed a "point" because the linecomp function, which it uses, only accepts points because it was originally made just for sorting points. Fundamentally, it works similarly to a BINARY SEARCH algorithm.
int Profile::get_closest_element_position(Point* value,vector<Point>::iterator first,vector<Point>::iterator last){
   vector<Point>::iterator originalFirst = first;
   vector<Point>::iterator middle;
   while(true){//INFINITE LOOP interrupted by returns.
      middle = first + distance(first,last)/2;
      if(linecomp(*middle,*value))first = middle;//IF the "middle" point is further along the horizontal plane-line than the passed-value point then make the "first" point equal to the "middle" point. This is because the vector and the plane are in the "wrong" order.
      else if(linecomp(*value,*middle))last = middle;//ELSE IF the opposite, make the "last" point equal to the "middle" point.
      else return distance(originalFirst,middle);//ELSE, in the very rare event that the passed point is exactly equal in x and y coordinates (or, more correctly, its "hypotenuse" is equal, as that can happen with the coordinates being different) return the position in the vector where that happens.
      if(distance(first,last)<2 && distance(first,middle)<1)return distance(originalFirst,middle);//IF the "first" and "middle" are now in the same position AND the distance between first and last is now just 1, as will (almost, see above line) inevitably happen because of the properties of integer division (at the beginning of the loop), then return the distane between the original "first" and the current "middle", as this is the point that most closely approximates the position along the vector (and HORIZONTALLY across the plane) of the point passed in.
   }
}

//On a left click, this prepares for panning by storing the initial position of the cursor.
bool Profile::on_pan_start(GdkEventButton* event){
   if(event->button==1 || event->button==2){
      panstartx = event->x;
      panstarty = event->y;
      get_parent()->grab_focus();//This causes the event box containing the profile to grab the focus, and so to allow keyboard control of the profile (this is not done directly as that would cause expose events to be called when focus changes, resulting in graphical glitches).
      return true;
   }
   else return false;
}
//As the cursor moves while the left button is depressed, the image is dragged along as a preview (with fewer points) to reduce lag. The centre point is modified by the negative of the distance (in image units, hence the ratio/zoomlevel mention) the cursor has moved to make a dragging effect and then the current position of the cursor is taken to be the starting position for the next drag (if there is one). The view is then refreshed and then the image is drawn (as a preview).
bool Profile::on_pan(GdkEventMotion* event){
   if((event->state & Gdk::BUTTON1_MASK) == Gdk::BUTTON1_MASK || (event->state & Gdk::BUTTON2_MASK) == Gdk::BUTTON2_MASK){
      double breadth = endx - startx;
      double height = endy - starty;
      double length = sqrt(breadth*breadth+height*height);//Right triangle.
      double hypotenuse = (event->x-panstartx)*ratio/zoomlevel;//The horizontal distance is a combination of x and y so:
      centrex -= hypotenuse * breadth / length;
      centrey -= hypotenuse * height / length;
      centrez += (event->y-panstarty)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
      panstartx=event->x;
      panstarty=event->y;
      return drawviewable(2);
   }
   else return false;
}
//At the end of the pan draw the full image.
bool Profile::on_pan_end(GdkEventButton* event){
   if(event->button==1 || event->button==2)return drawviewable(1);
   else return false;
}
//Moves the view depending on the keyboard signals.
bool Profile::on_pan_key(GdkEventKey *event,double scrollspeed){
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   double hypotenuse = scrollspeed*ratio/zoomlevel;//The horizontal distance is a combination of x and y so:
   switch(event->keyval){
      case GDK_w:centrez += hypotenuse;return drawviewable(2);break;//Up.
      case GDK_s:centrez -= hypotenuse;return drawviewable(2);break;//Down.
      case GDK_a:centrex -= hypotenuse*breadth/length;centrey -= hypotenuse*height/length;return drawviewable(2);break;//Left.
      case GDK_d:centrex += hypotenuse*breadth/length;centrey += hypotenuse*height/length;return drawviewable(2);break;//Right.
      case GDK_z:case GDK_Z:return drawviewable(1);//Redraw.
      default:return false;break;
   }
   return false;
}
//Find the starting coordinates of the fence and draw.
bool Profile::on_fence_start(GdkEventButton* event){
   if(event->button==1){
      double breadth = endx - startx;
      double height = endy - starty;
      double length = sqrt(breadth*breadth+height*height);//Right triangle.
      double hypotenuse = (event->x-get_width()/2)*ratio/zoomlevel;//The horizontal distance is a combination of x and y so:
      fencestartx = fenceendx = centrex + viewerx + hypotenuse * breadth / length;
      fencestarty = fenceendy = centrey + viewery + hypotenuse * height / length;
      fencestartz = fenceendz = centrez + viewerz - (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has the origin at top left and opengl has it at bottom left.
      get_parent()->grab_focus();//This causes the event box containing the profile to grab the focus, and so to allow keyboard control of the profile (this is not done directly as that would cause expose events to be called when focus changes, resulting in graphical glitches).
      return drawviewable(1);
   }
   else if(event->button==2)return on_pan_start(event);
   else return false;
}
//Update the fence with new ending coordinates and draw.
bool Profile::on_fence(GdkEventMotion* event){
   if((event->state & Gdk::BUTTON1_MASK) == Gdk::BUTTON1_MASK){
      double breadth = endx - startx;
      double height = endy - starty;
      double length = sqrt(breadth*breadth+height*height);//Right triangle.
      double hypotenuse = (event->x-get_width()/2)*ratio/zoomlevel;//The horizontal distance is a combination of x and y so:
      fenceendx = centrex + viewerx + hypotenuse * breadth / length;
      fenceendy = centrey + viewery + hypotenuse * height / length;
      fenceendz = centrez + viewerz - (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
      return drawviewable(1);
   }
   else if((event->state & Gdk::BUTTON2_MASK) == Gdk::BUTTON2_MASK)return on_pan(event);
   else return false;
}
//Draw again. This is for if/when the on_fence() method calls drawviewable(2) rather than drawviewable(1).
bool Profile::on_fence_end(GdkEventButton* event){
   if(event->button==1)return drawviewable(1);
   else if(event->button==2)return on_pan_end(event);
   else return false;
}
//Moves the fence depending on keyboard commands.
bool Profile::on_fence_key(GdkEventKey *event,double scrollspeed){
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   double hypotenuse = scrollspeed*ratio/zoomlevel;//The horizontal distance is a combination of x and y so:
   switch(event->keyval){
      case GDK_W:fencestartz += hypotenuse;fenceendz += hypotenuse;break;//Up.
      case GDK_S:fencestartz -= hypotenuse;fenceendz -= hypotenuse;break;//Down.
      case GDK_A:fencestartx -= hypotenuse*breadth/length;fenceendx -= hypotenuse*breadth/length;//Left/
                 fencestarty -= hypotenuse*height/length;fenceendy -= hypotenuse*height/length;break;
      case GDK_D:fencestartx += hypotenuse*breadth/length;fenceendx += hypotenuse*breadth/length;//Right.
                 fencestarty += hypotenuse*height/length;fenceendy += hypotenuse*height/length;break;
      default:return false;break;
   }
   return drawviewable(2);
}
//Make the fence.
void Profile::makefencebox(){
   glColor3f(0.0,0.0,1.0);
   if(slanted){
      double breadth = fenceendx - fencestartx;
      double height = fenceendy - fencestarty;
      double deltaz = fenceendz - fencestartz;
      double horiz = sqrt(breadth*breadth + height*height);//Right triangle
      double length = sqrt(breadth*breadth + height*height + deltaz*deltaz);//Right triangle IN 3D!
      double horizratio = horiz/length;//This will be used for the "vertical" Z values because the corners are separated from the start and end points by vectors at right angles to the vector from the start and end point of the fence. Since the right angle to the Z axis that we are interested in is some combination of X and Y, this is the ratio we use.
      double deltazratio = deltaz/length;//...and analogously for this one.
      if(length==0)length=1;
      glBegin(GL_LINE_LOOP);
         glVertex3d(fencestartx - (slantwidth/2)*deltazratio*breadth/horiz - centrex,
                    fencestarty - (slantwidth/2)*deltazratio*height/horiz - centrey,
                    fencestartz + (slantwidth/2)*horizratio - centrez);
         glVertex3d(fencestartx + (slantwidth/2)*deltazratio*breadth/horiz - centrex,
                    fencestarty + (slantwidth/2)*deltazratio*height/horiz - centrey,
                    fencestartz - (slantwidth/2)*horizratio - centrez);
         glVertex3d(fenceendx + (slantwidth/2)*deltazratio*breadth/horiz - centrex,
                    fenceendy + (slantwidth/2)*deltazratio*height/horiz - centrey,
                    fenceendz - (slantwidth/2)*horizratio - centrez);
         glVertex3d(fenceendx - (slantwidth/2)*deltazratio*breadth/horiz - centrex,
                    fenceendy - (slantwidth/2)*deltazratio*height/horiz - centrey,
                    fenceendz + (slantwidth/2)*horizratio - centrez);
      glEnd();
   }
   else{
      glBegin(GL_LINE_LOOP);
         glVertex3d(fencestartx-centrex,fencestarty-centrey,fencestartz-centrez);
         glVertex3d(fencestartx-centrex,fencestarty-centrey,fenceendz-centrez);
         glVertex3d(fenceendx-centrex,fenceendy-centrey,fenceendz-centrez);
         glVertex3d(fenceendx-centrex,fenceendy-centrey,fencestartz-centrez);
      glEnd();
   }
}
//Find the starting coordinates of the ruler and set the label values to zero.
bool Profile::on_ruler_start(GdkEventButton* event){
   if(event->button==1){
      double breadth = endx - startx;
      double height = endy - starty;
      double length = sqrt(breadth*breadth+height*height);//Right triangle.
      double hypotenuse = (event->x-get_width()/2)*ratio/zoomlevel;//The horizontal distance is a combination of x and y so:
      rulerstartx = rulerendx = centrex + viewerx + hypotenuse * breadth / length;
      rulerstarty = rulerendy = centrey + viewery + hypotenuse * height / length;
      rulerstartz = rulerendz = centrez + viewerz - (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
      ostringstream zpos;
      zpos << rulerendz;
      rulerlabel->set_text("Distance: 0\nX: 0\nY: 0\nHoriz: 0\nZ: 0 Pos: " + zpos.str());
      get_parent()->grab_focus();//This causes the event box containing the profile to grab the focus, and so to allow keyboard control of the profile (this is not done directly as that would cause expose events to be called when focus changes, resulting in graphical glitches).
      return drawviewable(1);
   }
   else if(event->button==2)return on_pan_start(event);
   else return false;
}
//Find the current cursor coordinates in image terms (as opposed to window/screen terms) and then update the label with the distances. Then draw the ruler.
bool Profile::on_ruler(GdkEventMotion* event){
   if((event->state & Gdk::BUTTON1_MASK) == Gdk::BUTTON1_MASK){
      double breadth = endx - startx;
      double height = endy - starty;
      double length = sqrt(breadth*breadth+height*height);//Right triangle.
      double hypotenuse = (event->x-get_width()/2)*ratio/zoomlevel;//The horizontal distance is a combination of x and y so:
      rulerendx = centrex + viewerx + hypotenuse * breadth / length;
      rulerendy = centrey + viewery + hypotenuse * height / length;
      rulerendz = centrez + viewerz - (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
      double d,xd,yd,hd,zd;
      xd = abs(rulerendx-rulerstartx);
      yd = abs(rulerendy-rulerstarty);
      zd = abs(rulerendz-rulerstartz);
      hd = sqrt(xd*xd+yd*yd);//Combined horizontal distance.
      d = sqrt(hd*hd+zd*zd);//Combined horizontal and vertical distance.
      ostringstream dist,xdist,ydist,horizdist,zdist,zpos;
      dist << d;
      xdist << xd;
      ydist << yd;
      horizdist << hd;
      zdist << zd;
      zpos << rulerendz;
      string rulerstring = "Distance: " + dist.str() + "\nX: " + xdist.str() + "\nY: " + ydist.str() + "\nHoriz: " + horizdist.str() + "\nZ: " + zdist.str() + " Pos: " + zpos.str();
      rulerlabel->set_text(rulerstring);
      return drawviewable(1);
   }
   else if((event->state & Gdk::BUTTON2_MASK) == Gdk::BUTTON2_MASK)return on_pan(event);
   else return false;
}
//Draw again. This is for if/when the on_ruler() method calls drawviewable(2) rather than drawviewable(1).
bool Profile::on_ruler_end(GdkEventButton* event){
   if(event->button==1)return drawviewable(1);
   else if(event->button==2)return on_pan_end(event);
   else return false;
}
//Make the ruler as a thick line.
void Profile::makerulerbox(){
   glColor3f(1.0,1.0,1.0);
   glLineWidth(rulerwidth);
      glBegin(GL_LINES);
         glVertex3d(rulerstartx-centrex,rulerstarty-centrey,rulerstartz-centrez);
         glVertex3d(rulerendx-centrex,rulerendy-centrey,rulerendz-centrez);
      glEnd();
   glLineWidth(1);
}
//Draw the appropriate overlays when other drawing is already happening (i.e. the flushing or swapping of buffers must be done elsewhere).
void Profile::drawoverlays(){
   if(rulering)makerulerbox();
   if(fencing)makefencebox();
   if(showheightscale)makeZscale();
}

//This draws a scale. It works out what order of magnitude to use for the scale and the number of intervals to have in it and then modifies these if there would be too few or too many intervals. It then draws the vertical line and the small horizontal markers before setting up the font settings and then drawing the numbers by the markers.
void Profile::makeZscale(){
   double rheight = get_height()*ratio/zoomlevel;
   double order=1;
   if(rheight>5)for(int i=rheight;i>10;i/=10)if(rheight/(order*10)>5)order*=10;//This finds the order of magnitude (base 10) of rheight with the added proviso that rheight must be at least five times that order so that there are enough intervals to draw a decent scale. This gives a range of nummarks values (below) of 5-50. While it may seem that the i variable could be used instead of rheight/(order*10), this is not the case as the latter is a double calculation, while the former is a result of a series of integer calculations, so the results diverge.
   if(rheight<=5)for(double i=rheight;i<10;i*=10)order/=10;//For when the user zooms really far in.
   int nummarks = (int)(0.9*rheight/order);//Again, it would be tempting to use i here, but this is only one integer calculation while i is the result (probably) of several such calculations, and so has lost more precision.
   while(nummarks>10){//The original order we calculated would give a number of scale widths from 5-50, but anything more than 10 is probably too much, so this loop doubles the order value until nummarks falls below 10.
      order*=2;
      nummarks = (int)(0.9*rheight/order);
   }
   double padding = (rheight - nummarks*order)/2;//It would be more aesthetically pleasing to centre the scale.
   GLint viewport[4];
   GLdouble modelview[16];
   GLdouble projection[16];
   GLdouble origx,origy,origz;//The world coordinates of the origin for the screen coordinates.
   glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
   glGetDoublev(GL_PROJECTION_MATRIX,projection);
   glGetIntegerv(GL_VIEWPORT,viewport);
   gluUnProject(50,0,0.1,modelview,projection,viewport,&origx,&origy,&origz);
   GLdouble origx2,origy2,origz2;
   gluUnProject(80,0,0.1,modelview,projection,viewport,&origx2,&origy2,&origz2);
   GLdouble origx3,origy3,origz3;
   gluUnProject(85,0,0.1,modelview,projection,viewport,&origx3,&origy3,&origz3);//DAMN! Precision problems when zoomed right in! This is because all OpenGL functions on most graphics cards use floats only, not doubles. Might want to replace this with my own method (!!!).
   glColor3f(1.0,1.0,1.0);
   glBegin(GL_LINES);
      glVertex3d(origx,origy,origz + padding);//Vertical line.
      glVertex3d(origx,origy,origz + padding + nummarks*order);//...
      for(int i=0;i<=nummarks;i++){//Horizontal lines.
         glVertex3d(origx,origy,origz + padding + i*order);
         glVertex3d(origx2,origy2,origz2 + padding + i*order);
      }
   glEnd();
   GLint ctx = glcGenContext();
   glcContext(ctx);
   glcScale(14,14);
   GLfloat stringboundingbox[8];
   double stringheight = 0;
   for(int i=0;i<=nummarks;i++){
      ostringstream number;
      number << origz3 + centrez + i*order + padding;
      glcMeasureString(GL_FALSE,number.str().c_str());
      glcGetStringMetric(GLC_BOUNDS,stringboundingbox);
      stringheight = stringboundingbox[5] - stringboundingbox[3];
      glRasterPos3d(origx3,origy3,origz3 + padding + i*order - 0.5*stringheight*ratio/zoomlevel);//Draw numbers by the horizontal lines.
      glcRenderString(number.str().c_str());
   }
   glcDeleteContext(ctx);
}
   
//First, the distance between the centre of the window and the window position of the event is converted to image coordinates and added to the image centre. This is analogous to moving the centre to where the event occured. Then, depending on the direction of the scroll, the zoomlevel is increased or decreased. Then the centre is moved to where the centre of the window will now lie. The image is then drawn.
bool Profile::on_zoom(GdkEventScroll* event){
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   double hypotenuse = (event->x-get_width()/2)*ratio/zoomlevel;
   centrex += hypotenuse * breadth / length;
   centrey += hypotenuse * height / length;
   centrez -= (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
   if(zoomlevel>=1){
      if(event->direction==GDK_SCROLL_UP)zoomlevel+=pow(zoomlevel,zoompower)/2;//Zoom in.
      else if(event->direction==GDK_SCROLL_DOWN)zoomlevel-=pow(zoomlevel,zoompower)/2;//Zoom out.
   }
   else if(zoomlevel>=0.2){
      if(event->direction==GDK_SCROLL_UP)zoomlevel+=0.1;//Zoom in.
      else if(event->direction==GDK_SCROLL_DOWN)zoomlevel-=0.1;//Zoom out.
   }
   else if(event->direction==GDK_SCROLL_UP)zoomlevel+=0.1;//Zoom in only.
   if(zoomlevel<0.2)zoomlevel=0.2;
   hypotenuse = (event->x-get_width()/2)*ratio/zoomlevel;
   centrex -= hypotenuse * breadth / length;
   centrey -= hypotenuse * height / length;
   centrez += (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
   resetview();
   get_parent()->grab_focus();//This causes the event box containing the profile to grab the focus, and so to allow keyboard control of the profile (this is not done directly as that wuld cause expose events to be called when focus changes, resulting in graphical glitches).
   return drawviewable(1);
}
//Zooms depending on keyboard commands.
bool Profile::on_zoom_key(GdkEventKey* event){
   if(zoomlevel>=1)switch(event->keyval){
         case GDK_i:case GDK_I:case GDK_g:case GDK_G:zoomlevel+=pow(zoomlevel,zoompower)/2;break;//In.
         case GDK_o:case GDK_O:case GDK_b:case GDK_B:zoomlevel-=pow(zoomlevel,zoompower)/2;break;//Out.
         default:return false;break;
   }
   else if(zoomlevel>=0.2)switch(event->keyval){
         case GDK_i:case GDK_I:case GDK_g:case GDK_G:zoomlevel+=0.1;break;//In.
         case GDK_o:case GDK_O:case GDK_b:case GDK_B:zoomlevel-=0.1;break;//Out.
         default:return false;break;
   }
   else switch(event->keyval){
         case GDK_i:case GDK_I:case GDK_g:case GDK_G:zoomlevel+=0.1;break;//In only.
         default:return false;break;
   }
   if(zoomlevel<0.2)zoomlevel=0.2;
   resetview();
   return drawviewable(1);
}

//This creates an array of z values for the points in the profile that are derived from the real z values through a moving average. This results in a smoothed line.
void Profile::make_moving_average(){
   if(linez!=NULL){
      for(int i=0;i<linezsize;i++){
         delete[] linez[i];
      }
      delete[] linez;
   }
   linezsize = flightlinestot.size();//One (smoothed) line for each flightline.
   linez = new double*[linezsize];
   for(int i=0;i<linezsize;i++){
      int numofpoints = (int)flightlinepoints[i].size();
      linez[i] = new double[numofpoints];
      for(int j=0;j<numofpoints;j++){
         double z=0,zcount=0;
         for(int k=-mavrgrange;k<=mavrgrange;k++)if(j+k>=0&&j+k<numofpoints){//For (up to) the range (depending on how close to the edge the point is) add up the points...
            z+=flightlinepoints[i][j+k].z;
            zcount++;
         }
         z /= zcount;//... and divide by the number of them to get the moving average at that point.
         linez[i][j]=z;
      }
   }
}

/*This method draws the main image. First, the gl_window is acquired for drawing. It is then cleared, otherwise the method would just draw over the previous image and, since this image will probably have gaps in it, the old image would be somewhat visible. Then:
 *
 *   for every bucket:
 *      for every point:
 *         determine colour and brightness of point
 *         place point
 *      end for
 *      draw all points in bucket
 *   end for
 *
 *Then the profiling box, fence box and ruler are drawn if they exist.
 *
 *
 * */
bool Profile::mainimage(int detail){
   if(detail<1)detail=1;//Values of less than 1 would cause infinite loops (though negative value would eventually stop due to overflow.
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return false;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Need to clear screen because of gaps.
   double red,green,blue,z;
   int intensity;
   int limit = 0;
   for(int i=0;i<(int)flightlinestot.size();i++){//The size of the vertex and colour arrays should be the same as that of the largest group of points, by flightline.
      if((int)flightlinepoints[i].size()>limit)limit = (int)flightlinepoints[i].size();
   }
   float* vertices = new float[3*limit];//Needed for the glDrawArrays() call further down.
   float* colours = new float[3*limit];//...
   glEnableClientState(GL_VERTEX_ARRAY);//...
   glEnableClientState(GL_COLOR_ARRAY);//...
   glVertexPointer(3, GL_FLOAT, 0, vertices);//...
   glColorPointer(3, GL_FLOAT, 0, colours);//...
   Point *leftpnt = new Point;//Fake point for sending to linecomp and get_closest_element_position the boundaries of the screen.
   leftpnt->x = leftboundx + centrex;//Assign x and y coordinates. All other fields irrelevant.
   leftpnt->y = leftboundy + centrey;//...
   Point *rightpnt = new Point;//Fake point for sending to linecomp and get_closest_element_position the boundaries of the screen.
   rightpnt->x = rightboundx + centrex;//Assign x and y coordinates. All other fields irrelevant.
   rightpnt->y = rightboundy + centrey;//...
   for(int i=0;i<(int)flightlinestot.size();i++){
      minplanx = startx + leftboundx;//These ensure that the entire screen will be filled, otherwise, because the screen position of startx changes, only part of the point-set will be drawn.
      minplany = starty + leftboundy;//...
      int startindex = get_closest_element_position(rightpnt,flightlinepoints[i].begin(),flightlinepoints[i].end());
      int endindex = get_closest_element_position(leftpnt,flightlinepoints[i].begin(),flightlinepoints[i].end());
      for(int l=0;l<detail*2;l++)if(endindex < (int)flightlinepoints[i].size()-1)endindex++;//This is to ensure that the left edge of the screen always has a line crossing it if there are extra points beyond it, otherwise it will only draw up to, not beyond, the last point actually on-screen.
      int count = 0;
      if(drawmovingaverage){
         int index = flightlinestot.at(i) % 6;
         switch(index){//Colour line by flightline, always. Repeat 6 distinct colours.
            case 0:red=0;green=1;blue=0;break;//Green
            case 1:red=0;green=0;blue=1;break;//Blue
            case 2:red=1;green=0;blue=0;break;//Red
            case 3:red=0;green=1;blue=1;break;//Cyan
            case 4:red=1;green=1;blue=0;break;//Yellow
            case 5:red=1;green=0;blue=1;break;//Purple
            default:red=green=blue=1;break;//White in the event of strangeness.
         }
         for(int j=startindex;j<=endindex;j+=detail){
            vertices[3*count] = flightlinepoints[i][j].x-centrex;
            vertices[3*count+1] = flightlinepoints[i][j].y-centrey;
            vertices[3*count+2] = linez[i][j]-centrez;
            colours[3*count] = red;
            colours[3*count+1] = green;
            colours[3*count+2] = blue;
            count++;
         }
         glDrawArrays(GL_LINE_STRIP,0,count);//Send contents of arrays to OpenGL, ready to be drawn when the buffer is flushed.
      }
      count = 0;
      if(drawpoints){
         for(int j=startindex;j<=endindex;j+=detail){
            red = 0.0; green = 1.0; blue = 0.0;//Default colour.
            z = flightlinepoints[i][j].z;//This is here because it is used in calculations.
            intensity = flightlinepoints[i][j].intensity;
            if(heightcolour){//Colour by elevation.
               red = colourheightarray[3*(int)(10*(z-rminz))];
               green = colourheightarray[3*(int)(10*(z-rminz)) + 1];
               blue = colourheightarray[3*(int)(10*(z-rminz)) + 2];
            }
            else if(intensitycolour){//Colour by intensity.
               red = colourintensityarray[3*(int)(intensity-rminintensity)];
               green = colourintensityarray[3*(int)(intensity-rminintensity) + 1];
               blue = colourintensityarray[3*(int)(intensity-rminintensity) + 2];
            }
            else if(linecolour){//Colour by flightline. Repeat 6 distinct colours.
                int index = flightlinepoints[i][j].flightLine % 6;
                switch(index){
                   case 0:red=0;green=1;blue=0;break;//Green
                   case 1:red=0;green=0;blue=1;break;//Blue
                   case 2:red=1;green=0;blue=0;break;//Red
                   case 3:red=0;green=1;blue=1;break;//Cyan
                   case 4:red=1;green=1;blue=0;break;//Yellow
                   case 5:red=1;green=0;blue=1;break;//Purple
                   default:red=green=blue=1;break;//White in the event of strangeness.
                }
            }
            else if(classcolour){//Colour by classification.
                switch(flightlinepoints[i][j].classification){
                   case 0:case 1:red=1;green=1;blue=0;break;//Yellow for non-classified.
                   case 2:red=0.6;green=0.3;blue=0;break;//Brown for ground.
                   case 3:red=0;green=0.3;blue=0;break;//Dark green for low vegetation.
                   case 4:red=0;green=0.6;blue=0;break;//Medium green for medium vegetation.
                   case 5:red=0;green=1;blue=0;break;//Bright green for high vegetation.
                   case 6:red=0;green=1;blue=0;break;//Cyan for buildings.
                   case 7:red=1;green=0;blue=1;break;//Purple for low point (noise).
                   case 8:red=0.5;green=0.5;blue=0.5;break;//Grey for model key-point (mass point).
                   case 9:red=0;green=0;blue=1;break;//Blue for water.
                   case 12:red=1;green=1;blue=1;break;//White for overlap points.
                   default:red=1;green=0;blue=0;break;//Red for undefined.
                }
            }
            else if(returncolour){//Colour by return.
                switch(flightlinepoints[i][j].packedByte & returnnumber){//FIXME! If there is desire to change system to handle a very large number of returns then this sytem must be changed completely. code: [woolol9999]
                   case 1:red=0;green=0;blue=1;break;//Blue
                   case 2:red=0;green=1;blue=1;break;//Cyan
                   case 3:red=0;green=1;blue=0;break;//Green
                   case 4:red=1;green=0;blue=0;break;//Red
                   case 5:red=1;green=0;blue=1;break;//Purple
                   case 6:red=1;green=1;blue=0;break;//Yellow
                   case 7:red=1;green=0.5;blue=0.5;break;//Pink
                   default:red=green=blue=1;break;//White in the event of strangeness.
                }
            }
            if(heightbrightness){//Shade by height.
               red *= brightnessheightarray[(int)(10*(z-rminz))];
               green *= brightnessheightarray[(int)(10*(z-rminz))];
               blue *= brightnessheightarray[(int)(10*(z-rminz))];
            }
            else if(intensitybrightness){//Shade by intensity.
               red *= brightnessintensityarray[(int)(intensity-rminintensity)];
               green *= brightnessintensityarray[(int)(intensity-rminintensity)];
               blue *= brightnessintensityarray[(int)(intensity-rminintensity)];
            }
            vertices[3*count] = flightlinepoints[i][j].x-centrex;
            vertices[3*count+1] = flightlinepoints[i][j].y-centrey;
            vertices[3*count+2]= z-centrez;
            colours[3*count]=red;
            colours[3*count+1]=green;
            colours[3*count+2]=blue;
            count++;
         }
         glDrawArrays(GL_POINTS,0,count);//Send contents of arrays to OpenGL, ready to be drawn when the buffer is flushed.
      }
   }
   delete leftpnt;
   delete rightpnt;
   drawoverlays();
   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   else glFlush();
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glwindow->gl_end();
   delete[] vertices;
   delete[] colours;
   return true;
}
