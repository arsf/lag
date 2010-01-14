/*
 * File: TwoDeeOverview.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: November 2009 - January 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <vector>
#include <iostream>
#include "quadtree.h"
#include "quadtreestructs.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "TwoDeeOverview.h"
#include "MathFuncs.h"

TwoDeeOverview::TwoDeeOverview(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit,Gtk::Label *rulerlabel)  : Display(config,lidardata,bucketlimit){
   zoompower = 0.5;
   maindetailmod = 0.01;
   previewdetailmod = 1;
   //Profiling:
   profwidth=30;
   profiling=false;
   showprofile=false;
   //Fencing:
   fencing=false;
   showfence=false;
   //Rulering:
   rulering=false;
   rulerwidth=2;
   this->rulerlabel = rulerlabel;
   //Classification heightening:
   heightenNonC = false;
   heightenGround = false;
   heightenLowVeg = false;
   heightenMedVeg = false;
   heightenHighVeg = false;
   heightenBuildings = false;
   heightenNoise = false;
   heightenMass = false;
   heightenWater = false;
   heightenOverlap = false;
   heightenUndefined = false;
   //Events and signals:
   add_events(Gdk::SCROLL_MASK   |   Gdk::BUTTON1_MOTION_MASK   |   Gdk::BUTTON_PRESS_MASK   |   Gdk::BUTTON_RELEASE_MASK);
   signal_scroll_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_zoom));
   sigpanstart = signal_button_press_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_pan_start));
   sigpan = signal_motion_notify_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_pan));
   sigpanend = signal_button_release_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_pan_end));
   sigprofstart = signal_button_press_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_prof_start));
   sigprof = signal_motion_notify_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_prof));
   sigprofend = signal_button_release_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_prof_end));
   sigprofstart.block();
   sigprof.block();
   sigprofend.block();
   sigfencestart = signal_button_press_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_fence_start));
   sigfence = signal_motion_notify_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_fence));
   sigfenceend = signal_button_release_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_fence_end));
   sigfencestart.block();
   sigfence.block();
   sigfenceend.block();
   sigrulerstart = signal_button_press_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_ruler_start));
   sigruler = signal_motion_notify_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_ruler));
   sigrulerend = signal_button_release_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_ruler_end));
   sigrulerstart.block();
   sigruler.block();
   sigrulerend.block();
}

TwoDeeOverview::~TwoDeeOverview(){}

//Return to initial viewing position.
bool TwoDeeOverview::returntostart(){
   boundary* lidarboundary = lidardata->getboundary();
   double xdif = lidarboundary->maxX-lidarboundary->minX;
   double ydif = lidarboundary->maxY-lidarboundary->minY;
   centrex = lidarboundary->minX+xdif/2;
   centrey = lidarboundary->minY+ydif/2;
   zoomlevel=1;
   resetview();
   delete lidarboundary;
   return drawviewable(1);
}

//This determines what part of the image is displayed with orthographic projection. It sets the active matrix to that of projection and makes it the identity matrix, and then defines the limits of the viewing area from the dimensions of the window. *ratio*zoomlevel is there to convert screen dimensions to image dimensions. gluLookAt is then used so that the viewpoint is that of seeing the centre from above, with North being up.
void TwoDeeOverview::resetview(){
   double altitude = rmaxz+5000;
   double depth = rminz-5000;
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-(get_width()/2)*ratio/zoomlevel,
           +(get_width()/2)*ratio/zoomlevel,
           -(get_height()/2)*ratio/zoomlevel,
           +(get_height()/2)*ratio/zoomlevel,
           -5*altitude,-5*depth);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(centrex,centrey,0,
             centrex,centrey,-100,
             0,1,0);
}

//On a left click, this prepares for panning by storing the initial position of the cursor.
bool TwoDeeOverview::on_pan_start(GdkEventButton* event){
   if(event->button==1){
      panstartx = event->x;
      panstarty = event->y;
   }
   return true;
}
//As the cursor moves while the left button is depressed, the image is dragged along as a preview (with fewer points) to reduce lag. The centre point is modified by the negative of the distance (in image units, hence the ratio/zoomlevel mention) the cursor has moved to make a dragging effect and then the current position of the cursor is taken to be the starting position for the next drag (if there is one). The view is then refreshed and then the image is drawn (as a preview). The button is not defined here as it is defined in the /glade file.
bool TwoDeeOverview::on_pan(GdkEventMotion* event){
      centrex -= (event->x-panstartx)*ratio/zoomlevel;
      centrey += (event->y-panstarty)*ratio/zoomlevel;//Y is reversed because gtk has origin at top left and opengl has it at bottom left.
      panstartx=event->x;
      panstarty=event->y;
      resetview();
      return drawviewable(2);
}
//At the end of the pan draw the full image.
bool TwoDeeOverview::on_pan_end(GdkEventButton* event){
   if(event->button==1)return drawviewable(1);
   else return false;
}

//At the beginning of profiling, defines the start point and, for the moment, the end point of the profile, Prepares the profile box for drawing and then calls the drawing method.
bool TwoDeeOverview::on_prof_start(GdkEventButton* event){
   profstartx = profendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
   profstarty = profendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
   makeprofbox();
   return drawviewable(2);
}
//Updates the end point of the profile and then gets the vertical and horisontal differences betweent the start and end points. These are used to determine the length of the profile and hence the positions of the vertices of the profile rectangle. The rectangle is prepared and then the drawing method is called.
bool TwoDeeOverview::on_prof(GdkEventMotion* event){
   profendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
   profendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
   makeprofbox();
   return drawviewable(2);
}
//Draw the full image at the end of selecting a profile.
bool TwoDeeOverview::on_prof_end(GdkEventButton* event){return drawviewable(1);}
//This makes the box showing the profile area. It calculates the ratio between the length of the profile and its x and y dimensions. It then prepares a rectangle for drawing from this.
void TwoDeeOverview::makeprofbox(){
   double breadth = profendx - profstartx;
   double height = profendy - profstarty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   double altitude = rmaxz+1000;//This makes sure the profile box is drawn over the top of the flightlines.
   if(length==0)length=1;
   glNewList(4,GL_COMPILE);
      glColor3f(1.0,1.0,1.0);
      glBegin(GL_LINE_LOOP);
         glVertex3d(profstartx-(profwidth/2)*height/length,profstarty+(profwidth/2)*breadth/length,altitude);
         glVertex3d(profstartx+(profwidth/2)*height/length,profstarty-(profwidth/2)*breadth/length,altitude);
         glVertex3d(profendx+(profwidth/2)*height/length,profendy-(profwidth/2)*breadth/length,altitude);
         glVertex3d(profendx-(profwidth/2)*height/length,profendy+(profwidth/2)*breadth/length,altitude);
      glEnd();
   glEndList();
}

//Initialises the coordinates of the fence and prepares it for drawing, then draws preview.
bool TwoDeeOverview::on_fence_start(GdkEventButton* event){
   fencestartx = fenceendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
   fencestarty = fenceendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
   makefencebox();
   return drawviewable(2);
}
//Updates end coordinates of the fence and prepares it for drawing, then draws preview.
bool TwoDeeOverview::on_fence(GdkEventMotion* event){
   fenceendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
   fenceendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
   makefencebox();
   return drawviewable(2);
}
//Draws the main image one more.
bool TwoDeeOverview::on_fence_end(GdkEventButton* event){return drawviewable(1);}
//Makes the fence box.
void TwoDeeOverview::makefencebox(){
   double altitude = rmaxz+1000;//This makes sure the fence box is drawn over the top of the flightlines.
   glNewList(6,GL_COMPILE);
      glColor3f(1.0,1.0,1.0);
      glBegin(GL_LINE_LOOP);
         glVertex3d(fencestartx,fencestarty,altitude);
         glVertex3d(fencestartx,fenceendy,altitude);
         glVertex3d(fenceendx,fenceendy,altitude);
         glVertex3d(fenceendx,fencestarty,altitude);
      glEnd();
   glEndList();
}

//Find the starting coordinates of the ruler and set the label values to zero.
bool TwoDeeOverview::on_ruler_start(GdkEventButton* event){
   rulerstartx = rulerendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
   rulerstarty = rulerendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
   rulerlabel->set_text("Distance: 0\nX: 0\nY: 0");
   makerulerbox();
   return drawviewable(2);
}
//Find the current cursor coordinates in image terms (as opposed to window/screen terms) and then update the label with the distances. Then draw the ruler.
bool TwoDeeOverview::on_ruler(GdkEventMotion* event){
   rulerendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
   rulerendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
   double d,xd,yd;
   xd = abs(rulerendx-rulerstartx);
   yd = abs(rulerendy-rulerstarty);
   d = sqrt(xd*xd+yd*yd);
   ostringstream dist,xdist,ydist;
   dist << d;
   xdist << xd;
   ydist << yd;
   string rulerstring = "Distance: " + dist.str() +"\nX: " + xdist.str() + "\nY: " + ydist.str();
   rulerlabel->set_text(rulerstring);
   makerulerbox();
   return drawviewable(2);
}
//Draw again. This is for if/when the on_ruler() method calls drawviewable(2) rather than drawviewable(1).
bool TwoDeeOverview::on_ruler_end(GdkEventButton* event){return drawviewable(1);}
//Make the ruler as a thick line.
void TwoDeeOverview::makerulerbox(){
   double altitude = rmaxz+1000;//This makes sure the profile box is drawn over the top of the flightlines.
   glNewList(5,GL_COMPILE);
   glColor3f(1.0,1.0,1.0);
   glLineWidth(3);
   glBegin(GL_LINES);
      glVertex3d(rulerstartx,rulerstarty,altitude);
      glVertex3d(rulerendx,rulerendy,altitude);
   glEnd();
   glLineWidth(1);
   glEndList();
}

//Gets the limits of the viewable area and passes them to the subsetting method of the quadtree to get the relevant data. It then converts from a vector to a pointer array to make data extraction faster. Then, depending on the imagetype requested, it sets the detail level and then calls one of the image methods, which actually draws the data to the screen.
bool TwoDeeOverview::drawviewable(int imagetype){
  get_gl_window()->make_current(get_gl_context());
  glViewport(0, 0, get_width(), get_height());
  resetview();
   double minx = centrex-get_width()/2*ratio/zoomlevel;//Limits of viewable area:
   double maxx = centrex+get_width()/2*ratio/zoomlevel;//...
   double miny = centrey-get_height()/2*ratio/zoomlevel;//...
   double maxy = centrey+get_height()/2*ratio/zoomlevel;//...
   vector<pointbucket*> *pointvector;
   try{
      pointvector = lidardata->subset(minx,miny,maxx,maxy);//Get data.
   }catch(const char* e){
      cout << e << endl;
      cout << "No points returned." << endl;
      return false;
   }
   int numbuckets = pointvector->size();
   pointbucket** buckets = new pointbucket*[numbuckets];
   for(int i=0;i<numbuckets;i++){//Convert to pointer for faster access in for loops in image methods. Why? Expect >100000 points.
      buckets[i]=pointvector->at(i);
   }
   int detail=1;//This determines how many points are skipped between reads, to make drawing faster when zoomed out.
   if(imagetype==1){
      detail=(int)(numbuckets*maindetailmod);
      if(detail<1)detail=1;
      mainimage(buckets,numbuckets,detail);
   }
   else if(imagetype==2){
      detail=(int)(numbuckets*previewdetailmod);
      if(detail<1)detail=1;
      previewimage(buckets,numbuckets,detail);
   }
   delete[] buckets;
   delete pointvector;
   return true;
}

//(Anything that happens for centrex is reversed for centrey). First, half the distance between the centre of the window and the window position of the event is converted to image coordinates and added to the image centre. This is analogous to moving the centre to where the event occured. Then, depending on the direction of the scroll, the zoomlevel is increased or decreased. Then the centre is moved to where the centre of the window will now lie. The image is then drawn.
bool TwoDeeOverview::on_zoom(GdkEventScroll* event){
   centrex += (event->x-get_width()/2)*ratio/zoomlevel;
   centrey -= (event->y-get_height()/2)*ratio/zoomlevel;//Y is reversed because gtk has origin at top left and opengl has it at bottom left.
   if(zoomlevel>=1){
      if(event->direction==GDK_SCROLL_UP)zoomlevel+=pow(zoomlevel,zoompower)/2;
      else if(event->direction==GDK_SCROLL_DOWN)zoomlevel-=pow(zoomlevel,zoompower)/2;
   }
   else if(zoomlevel>=0.2){
      if(event->direction==GDK_SCROLL_UP)zoomlevel+=0.1;
      else if(event->direction==GDK_SCROLL_DOWN)zoomlevel-=0.1;
   }
   else if(event->direction==GDK_SCROLL_UP)zoomlevel+=0.1;
   if(zoomlevel<0.2)zoomlevel=0.2;
   centrex -= (event->x-get_width()/2)*ratio/zoomlevel;
   centrey += (event->y-get_height()/2)*ratio/zoomlevel;//Y is reversed because gtk has origin at top left and opengl has it at bottom left.
   resetview();
   return drawviewable(1);
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
 *Then the profiling box is drawn if it exists.
 *
 *
 * */
bool TwoDeeOverview::mainimage(pointbucket** buckets,int numbuckets,int detail){
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return false;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Need to clear screen because of gaps.
   int line=0,intensity=0,classification=0,rnumber=0;
   double x=0,y=0,z=0;//Point values
   double red,green,blue;//Colour values
   float* vertices = new float[3*bucketlimit];//Needed for the glDrawArrays() call further down.
   float* colours = new float[3*bucketlimit];//...
   glEnableClientState(GL_VERTEX_ARRAY);//...
   glEnableClientState(GL_COLOR_ARRAY);//...
   glVertexPointer(3, GL_FLOAT, 0, vertices);//...
   glColorPointer(3, GL_FLOAT, 0, colours);//...
   for(int i=0;i<numbuckets;i++){//For every bucket...
      int count=0;//This is needed for putting values in the right indices for the above arrays. j does not suffice because of the detail variable.
      for(int j=0;j<buckets[i]->numberofpoints;j+=detail){//... and for every point, determine point colour and position:
         red = 0.0; green = 1.0; blue = 0.0;//Default colour.
         x = buckets[i]->points[j].x;
         y = buckets[i]->points[j].y;
         z = buckets[i]->points[j].z;
         intensity = buckets[i]->points[j].intensity;
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
             line = buckets[i]->points[j].flightline;
             int index = line % 6;
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
             classification = buckets[i]->points[j].classification;
             int index = classification;
             switch(index){
                case 0:case 1:red=1;green=0;blue=0;break;//Red for non-classified.
                case 2:red=0.6;green=0.3;blue=0;break;//Brown for ground.
                case 3:red=0;green=0.3;blue=0;break;//Dark green for low vegetation.
                case 4:red=0;green=0.6;blue=0;break;//Medium green for medium vegetation.
                case 5:red=0;green=1;blue=0;break;//Bright green for high vegetation.
                case 6:red=0;green=1;blue=0;break;//Cyan for buildings.
                case 7:red=1;green=0;blue=1;break;//Purple for low point (noise).
                case 8:red=0.5;green=0.5;blue=0.5;break;//Grey for model key-point (mass point).

                case 9:red=0;green=0;blue=1;break;//Blue for water.
                case 12:red=1;green=1;blue=1;break;//White for overlap points.
                default:red=1;green=1;blue=0;cout << "Undefined point." << endl;break;//Yellow for undefined.
             }
         }
         else if(returncolour){//Colour by flightline. Repeat 6 distinct colours.
             rnumber = buckets[i]->points[j].rnumber;
             int index = rnumber;
             switch(index){
                case 1:red=0;green=0;blue=1;break;//Blue
                case 2:red=0;green=1;blue=1;break;//Cyan
                case 3:red=0;green=1;blue=0;break;//Green
                case 4:red=1;green=0;blue=0;break;//Red
                case 5:red=1;green=0;blue=1;break;//Purple
                default:red=green=blue=1;break;//White in the event of strangeness.
             }
         }
         if(heightbrightness){//Shade by height.
            red *= brightnessheightarray[(int)(z-rminz)];
            green *= brightnessheightarray[(int)(z-rminz)];
            blue *= brightnessheightarray[(int)(z-rminz)];
         }
         else if(intensitybrightness){//Shade by intensity.
            red *= brightnessintensityarray[(int)(intensity-rminintensity)];
            green *= brightnessintensityarray[(int)(intensity-rminintensity)];
            blue *= brightnessintensityarray[(int)(intensity-rminintensity)];
         }
         vertices[3*count]=x;
         vertices[3*count+1]=y;
         if(heightenNonC ||
            heightenGround ||
            heightenLowVeg ||
            heightenMedVeg ||
            heightenHighVeg ||
            heightenBuildings ||
            heightenNoise ||
            heightenMass ||
            heightenWater ||
            heightenOverlap ||
            heightenUndefined){
            classification = buckets[i]->points[j].classification;
            int index = classification;
            double incrementor = 2*abs(rmaxz-rminz);
            switch(index){
               case 0:case 1:if(heightenNonC)z+=incrementor;break;
               case 2:if(heightenGround)z+=incrementor;break;
               case 3:if(heightenLowVeg)z+=incrementor;break;
               case 4:if(heightenMedVeg)z+=incrementor;break;
               case 5:if(heightenHighVeg)z+=incrementor;break;
               case 6:if(heightenBuildings)z+=incrementor;break;
               case 7:if(heightenNoise)z+=incrementor;break;
               case 8:if(heightenMass)z+=incrementor;break;
               case 9:if(heightenWater)z+=incrementor;break;
               case 12:if(heightenOverlap)z+=incrementor;break;
               default:if(heightenUndefined)z+=incrementor;break;
            }
         }
         vertices[3*count+2]=z;
         colours[3*count]=red;
         colours[3*count+1]=green;
         colours[3*count+2]=blue;
         count++;
      }
      glDrawArrays(GL_POINTS,0,count);
      //Perhaps modify to happen only when the estimated number of points exceeds a certain value? Estimation could be: numbuckets * bucketlimit / detail. Quite rough, though. This might then cause previewimage to become useless. :-)
      if (glwindow->is_double_buffered())glwindow->swap_buffers();//Draw to screen every bucket to show user stuff is happening.
      else glFlush();
   }
   if(profiling||showprofile)glCallList(4);//Draw the profile box if profile mode is on.
   if(rulering)glCallList(5);//Draw the ruler if ruler mode is on.
   if(fencing||showfence)glCallList(6);//Draw the fence box if fence mode is on.
   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   else glFlush();
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glwindow->gl_end();
   delete[] vertices;
   delete[] colours;
   return true;
}

//NOTE: This method is almost identical to the mainimage method. Only two lines are different, and could easily be replace with an if statement. The methods are kept separate as this one might change in the future to make it faster and/or more detailed.
/*This method draws the preview used in panning etc.. Basically, if something must be drawn quickly, this method is used. First, the gl_window is acquired for drawing. It is then cleared, otherwise the method would just draw over the previous image and, since this image will probably have gaps in it, the old image would be somewhat visible. Then:
 *
 *   for every bucket:
 *      for every point:
 *         determine colour and brightness of point
 *         place point
 *      end for
 *   end for
 *   draw all points
 *
 *Then the profiling box is drawn if it exists.
 *
 *
 * */
bool TwoDeeOverview::previewimage(pointbucket** buckets,int numbuckets,int detail){
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return false;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Need to clear screen because of gaps.
   int line=0,intensity=0,classification=0,rnumber=0;
   double x=0,y=0,z=0;//Point values
   double red,green,blue;//Colour values
   float* vertices = new float[3*bucketlimit];//Needed for the glDrawArrays() call further down.
   float* colours = new float[3*bucketlimit];//...
   glEnableClientState(GL_VERTEX_ARRAY);//...
   glEnableClientState(GL_COLOR_ARRAY);//...
   glVertexPointer(3, GL_FLOAT, 0, vertices);//...
   glColorPointer(3, GL_FLOAT, 0, colours);//...
   for(int i=0;i<numbuckets;i++){//For every bucket...
      int count=0;//This is needed for putting values in the right indices for the above arrays. j does not suffice because of the detail variable.
      for(int j=0;j<buckets[i]->numberofpoints;j+=detail){//... and for every point, determine point colour and position:
         red = 0.0; green = 1.0; blue = 0.0;//Default colour.
         x = buckets[i]->points[j].x;
         y = buckets[i]->points[j].y;
         z = buckets[i]->points[j].z;
         intensity = buckets[i]->points[j].intensity;
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
             line = buckets[i]->points[j].flightline;
             int index = line % 6;
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
             classification = buckets[i]->points[j].classification;
             int index = classification;
             switch(index){
                case 0:case 1:red=1;green=0;blue=0;break;//Red for non-classified.
                case 2:red=0.6;green=0.3;blue=0;break;//Brown for ground.
                case 3:red=0;green=0.3;blue=0;break;//Dark green for low vegetation.
                case 4:red=0;green=0.6;blue=0;break;//Medium green for medium vegetation.
                case 5:red=0;green=1;blue=0;break;//Bright green for high vegetation.
                case 6:red=0;green=1;blue=0;break;//Cyan for buildings.
                case 7:red=1;green=0;blue=1;break;//Purple for low point (noise).
                case 8:red=0.5;green=0.5;blue=0.5;break;//Grey for model key-point (mass point).

                case 9:red=0;green=0;blue=1;break;//Blue for water.
                case 12:red=1;green=1;blue=1;break;//White for overlap points.
                default:red=1;green=1;blue=0;cout << "Undefined point." << endl;break;//Yellow for undefined.
             }
         }
         else if(returncolour){//Colour by flightline. Repeat 6 distinct colours.
             rnumber = buckets[i]->points[j].rnumber;
             int index = rnumber;
             switch(index){
                case 1:red=0;green=0;blue=1;break;//Blue
                case 2:red=0;green=1;blue=1;break;//Cyan
                case 3:red=0;green=1;blue=0;break;//Green
                case 4:red=1;green=0;blue=0;break;//Red
                case 5:red=1;green=0;blue=1;break;//Purple
                default:red=green=blue=1;break;//White in the event of strangeness.
             }
         }
         if(heightbrightness){//Shade by height.
            red *= brightnessheightarray[(int)(z-rminz)];
            green *= brightnessheightarray[(int)(z-rminz)];
            blue *= brightnessheightarray[(int)(z-rminz)];
         }
         else if(intensitybrightness){//Shade by intensity.
            red *= brightnessintensityarray[(int)(intensity-rminintensity)];
            green *= brightnessintensityarray[(int)(intensity-rminintensity)];
            blue *= brightnessintensityarray[(int)(intensity-rminintensity)];
         }
         vertices[3*count]=x;
         vertices[3*count+1]=y;
         vertices[3*count+2]=z;
         colours[3*count]=red;
         colours[3*count+1]=green;
         colours[3*count+2]=blue;
         count++;
      }
      glDrawArrays(GL_POINTS,0,count);
   }
   if(profiling||showprofile)glCallList(4);//Draw the profile box if profile mode is on.
   if(rulering)glCallList(5);//Draw the ruler if ruler mode is on.
   if(fencing||showfence)glCallList(6);//Draw the fence box if fence mode is on.
   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   else glFlush();
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glwindow->gl_end();
   delete[] vertices;
   delete[] colours;
   return true;
}
