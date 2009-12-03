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
#include "MathFuncs.cpp"

TwoDeeOverview::TwoDeeOverview(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit)  : Gtk::GL::DrawingArea(config){
   this->lidardata=lidardata;
   lidarboundary = lidardata->getboundary();
   zoomlevel=1;
   profwidth=30;
   profiling=false;
   this->bucketlimit = bucketlimit;
   double xdif = lidarboundary->maxX-lidarboundary->minX;
   double ydif = lidarboundary->maxY-lidarboundary->minY;
   centrex = lidarboundary->minX+xdif/2;
   centrey = lidarboundary->minY+ydif/2;
   //Scaling to screen dimensions:
   double swidth = get_screen()->get_width();
   double sheight = get_screen()->get_height();
   double xratio = xdif/swidth;
   double yratio = ydif/sheight;
   yratio*=1.25;
   ratio = 0;
   if(xratio>yratio)ratio = xratio;
   else ratio = yratio;
   //Colouring and shading:
   heightcolour = false;
   heightbrightness = false;
   zoffset=0;
   zfloor=0.25;
   intensitycolour = false;
   intensitybrightness = true;
   intensityoffset = 0.0/3;
   intensityfloor = 0;
   rmaxz=rminz=0;
   rmaxintensity=rminintensity=0;
   linecolour = true;
   colourheightarray = new double[2];
   colourintensityarray = new double[2];
   brightnessheightarray = new double[2];
   brightnessintensityarray = new double[2];
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
}

TwoDeeOverview::~TwoDeeOverview(){
   delete[] colourheightarray;
}

//This determines what part of the image is displayed with orthographic projection. It sets the active matrix to that of projection and makes it the identity matrix, and then defines the limits of the viewing area using offsets from the centre. *ratio*zoomlevel is there to convert screen dimensions to image dimensions.
void TwoDeeOverview::resetview(){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(centrex-(get_width()/2)*ratio/zoomlevel,
          centrex+(get_width()/2)*ratio/zoomlevel,
          centrey-(get_height()/2)*ratio/zoomlevel,
          centrey+(get_height()/2)*ratio/zoomlevel,
          -1000.0,50000.0);
}

//Draw on expose. 1 indicates that the non-preview image is drawn.
bool TwoDeeOverview::on_expose_event(GdkEventExpose* event){ return drawviewable(1); }

//On a left click, this prepares for panning by storing the initial position of the cursor.
bool TwoDeeOverview::on_pan_start(GdkEventButton* event){
   if(event->button==1){
      panstartx = event->x;
      panstarty = event->y;
   }
   return true;
}

//As the cursor moves while the left button is depressed, the image is dragged along as a preview (with fewer points) to reduce lag. The centre point is modified by the negative of the distance (in image units, hence the ratio/zoomlevel mention) the cursor has moved to make a dragging effect and then the current position of the cursor is taken to be the starting position for the next drag (if there is one). The view is then refreshed and then the image is drawn (as a preview).
bool TwoDeeOverview::on_pan(GdkEventMotion* event){
//Y is reversed because gtk has origin at top left and opengl has it at bottom left.
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
   glNewList(4,GL_COMPILE);
   glColor3f(1.0,1.0,1.0);
   glBegin(GL_LINE_LOOP);
      glVertex3d(profstartx-profwidth/2,profstarty,-0.1);
      glVertex3d(profstartx-profwidth/2,profendy,-0.1);
      glVertex3d(profendx+profwidth/2,profendy,-0.1);
      glVertex3d(profendx+profwidth/2,profstarty,-0.1);
   glEnd();
   glEndList();
   return drawviewable(2);
}

//Updates the end point of the profile and then gets the vertical and horisontal differences betweent the start and end points. These are used to determine the length of the profile and hence the positions of the vertices of the profile rectangle. The rectangle is prepared and then the drawing method is called.
bool TwoDeeOverview::on_prof(GdkEventMotion* event){
   profendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
   profendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
   double breadth = profendx - profstartx;
   double height = profendy - profstarty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   glNewList(4,GL_COMPILE);
   glColor3f(1.0,1.0,1.0);
   glBegin(GL_LINE_LOOP);
      glVertex3d(profstartx-(profwidth/2)*height/length,profstarty+(profwidth/2)*breadth/length,-0.1);
      glVertex3d(profstartx+(profwidth/2)*height/length,profstarty-(profwidth/2)*breadth/length,-0.1);
      glVertex3d(profendx+(profwidth/2)*height/length,profendy-(profwidth/2)*breadth/length,-0.1);
      glVertex3d(profendx-(profwidth/2)*height/length,profendy+(profwidth/2)*breadth/length,-0.1);
   glEnd();
   glEndList();
   return drawviewable(2);
}

//Draw the full image at the end of selecting a profile.
bool TwoDeeOverview::on_prof_end(GdkEventButton* event){return drawviewable(1);}

//Gets the limits of the viewable area and passes them to the subsetting method of the quadtree to get the relevant data. It then converts from a vector to a pointer array to make data extraction faster. Then, depending on the imagetype requested, it sets the detail level and then calls one of the image methods, which actually draws the dat to the screen.
bool TwoDeeOverview::drawviewable(int imagetype){
   double minx = centrex-get_width()/2*ratio/zoomlevel;
   double maxx = centrex+get_width()/2*ratio/zoomlevel;
   double miny = centrey-get_height()/2*ratio/zoomlevel;
   double maxy = centrey+get_height()/2*ratio/zoomlevel;
   vector<pointbucket*> *pointvector = lidardata->subset(minx,miny,maxx,maxy);//Get data.
   int numbuckets = pointvector->size();
   pointbucket** buckets = new pointbucket*[numbuckets];
   for(int i=0;i<numbuckets;i++){//Convert to pointer for faster access in for loops in image methods. Why? Expect >100000 points.
      buckets[i]=pointvector->at(i);
   }
//This is for possible use later if it is needed to see how heights vary only in the viewable area:
//   double maxz = buckets[0]->maxz,minz = buckets[0]->minz;
//   double maxintensity = buckets[0]->maxintensity;
//   double minintensity = buckets[0]->minintensity;
//   for(unsigned long int i=0;i<numbuckets;i++){
////Perhaps need a better method in order to find 10th and 90th percentiles instead:
//      if(maxz<buckets[i]->maxz)maxz = buckets[i]->maxz;
//      if(minz>buckets[i]->minz)minz = buckets[i]->minz;
//      if(maxintensity<buckets[i]->maxintensity)maxintensity = buckets[i]->maxintensity;
//      if(minintensity>buckets[i]->minintensity)minintensity = buckets[i]->minintensity;
//   }
   int detail=1;//This determines how many points are skipped between reads, to make drawing faster when zoomed out.
   if(imagetype==1){
      detail=numbuckets/100;
      if(detail<1)detail=1;
      mainimage(buckets,numbuckets,detail);
   }
   else if(imagetype==2){
      detail=numbuckets*1;
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
      if(event->direction==GDK_SCROLL_UP)zoomlevel+=sqrt(zoomlevel)/2;
      else if(event->direction==GDK_SCROLL_DOWN)zoomlevel-=sqrt(zoomlevel)/2;
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

//When the window is resized, the viewport is resized accordingly and so are teh viewing properties.
bool TwoDeeOverview::on_configure_event(GdkEventConfigure* event){
  glViewport(0, 0, get_width(), get_height());
  resetview();
  return true;
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
   glClear(GL_COLOR_BUFFER_BIT);//Need to clear screen because of gaps.
   double red,green,blue;
   double x=0,y=0,z=0;
   int line=0,intensity=0;
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
            red = colourheightarray[3*(int)(z-rminz)];
            green = colourheightarray[3*(int)(z-rminz) + 1];
            blue = colourheightarray[3*(int)(z-rminz) + 2];
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
      if (glwindow->is_double_buffered())glwindow->swap_buffers();//Draw to screen every bucket to show user stuff is happening.
      else glFlush();
   }
//   glCallList(4);
   if(profiling)glCallList(4);//Draw the profile box if profile mode is on.
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
   glClear(GL_COLOR_BUFFER_BIT);//Need to clear screen because of gaps.
   double red,green,blue;
   double x=0,y=0,z=0;
   int line=0,intensity=0;
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
            red = colourheightarray[3*(int)(z-rminz)];
            green = colourheightarray[3*(int)(z-rminz) + 1];
            blue = colourheightarray[3*(int)(z-rminz) + 2];
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
//   glCallList(4);
   if(profiling)glCallList(4);//Draw the profile box if profile mode is on.
   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   else glFlush();
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glwindow->gl_end();
   delete[] vertices;
   delete[] colours;
   return true;
}

//Prepares the arrays for looking up the colours and shades of the points.
void TwoDeeOverview::coloursandshades(double maxz,double minz,int maxintensity,int minintensity){
   delete[] colourheightarray;
   delete[] colourintensityarray;
   delete[] brightnessheightarray;
   delete[] brightnessintensityarray;
   double red=0.0,green=0.0,blue=0.0;
   int z=0,intensity=0;
   colourheightarray = new double[3*(int)(rmaxz-rminz+4)];
   for(int i=0;i<(int)(rmaxz-rminz)+3;i++){//Fill height colour array:
      z = i + (int)rminz;
      colour_by(z,maxz,minz,red,green,blue);
      colourheightarray[3*i]=red;
      colourheightarray[3*i+1]=green;
      colourheightarray[3*i+2]=blue;
   }
   colourintensityarray = new double[3*(int)(rmaxintensity-rminintensity+4)];
   for(int i=0;i<rmaxintensity-rminintensity+3;i++){//Fill intensity colour array:
      intensity = i + rminintensity;
      colour_by(intensity,maxintensity,minintensity,red,green,blue);
      colourintensityarray[3*i]=red;
      colourintensityarray[3*i+1]=green;
      colourintensityarray[3*i+2]=blue;
   }
   brightnessheightarray = new double[(int)(rmaxz-rminz+4)];
   for(int i=0;i<(int)(rmaxz-rminz)+3;i++){//Fill height brightness array:
      z = i + (int)rminz;
      brightnessheightarray[i] = brightness_by(z,maxz,minz,zoffset,zfloor);
   }
   brightnessintensityarray = new double[(int)(rmaxintensity-rminintensity+4)];
   for(int i=0;i<rmaxintensity-rminintensity+3;i++){//Fill intensity brightness array:
      intensity = i + rminintensity;
      brightnessintensityarray[i] = brightness_by(intensity,maxintensity,minintensity,intensityoffset,intensityfloor);
   }
}

//This method prepares the image for drawing and sets up OpenGl. It gets the data from the quadtree in order to find the maximum and minimum height and intensity values and calls the coloursandshades() method to prepare the colouring of the points. It also sets ups anti-aliasing, clearing and the initial view.
void TwoDeeOverview::prepare_image(){
   vector<pointbucket*> *pointvector = lidardata->subset(lidarboundary->minX,lidarboundary->minY,lidarboundary->maxX,lidarboundary->maxY);//Get ALL data.
   int numbuckets = pointvector->size();
   pointbucket** buckets = new pointbucket*[numbuckets];
   for(int i=0;i<numbuckets;i++){
      buckets[i]=pointvector->at(i);
   }
   double maxz = buckets[0]->maxz,minz = buckets[0]->minz;
   int maxintensity = buckets[0]->maxintensity,minintensity = buckets[0]->minintensity;
   for(int i=0;i<numbuckets;i++){//Find the maximum and minimum values from the buckets:
      if(maxz<buckets[i]->maxz)maxz = buckets[i]->maxz;
      if(minz>buckets[i]->minz)minz = buckets[i]->minz;
      if(maxintensity<buckets[i]->maxintensity)maxintensity = buckets[i]->maxintensity;
      if(minintensity>buckets[i]->minintensity)minintensity = buckets[i]->minintensity;
   }
   rmaxz = maxz; rminz = minz;
   rmaxintensity = maxintensity; rminintensity = minintensity;
   if(maxz<=minz)maxz=minz+1;
   else{//Find the 0.01 and 99.99 percentiles of the height and intensity from the buckets. Not perfect (it could miss a hill in a bucket) but it does the job reasonably well:
      double* zdata = new double[numbuckets];
      for(int i=0;i<numbuckets;i++)zdata[i]=buckets[i]->maxz;
      double lowperc = percentilevalue(zdata,numbuckets,0.01,minz,maxz);
      for(int i=0;i<numbuckets;i++)zdata[i]=buckets[i]->minz;
      double highperc = percentilevalue(zdata,numbuckets,99.99,minz,maxz);
      maxz=highperc;
      minz=lowperc;
      delete[] zdata;
   }
//Might want this later:
//   if(maxintensity<=minintensity)maxintensity=minintensity+1;
//   else{
//      double* intensitydata = new double[numbuckets];
//      for(int i=0;i<numbuckets;i++)intensitydata[i]=buckets[i]->maxintensity;
//      double lowperc = percentilevalue(intensitydata,numbuckets,0.01,minintensity,maxintensity);
//      for(int i=0;i<numbuckets;i++)intensitydata[i]=buckets[i]->minintensity;
//      double highperc = percentilevalue(intensitydata,numbuckets,99.99,minintensity,maxintensity);
//      maxintensity=(maxintensity+highperc)/2;
//      minintensity=(minintensity+lowperc)/2;
//      delete intensitydata;
//   }
   coloursandshades(maxz,minz,maxintensity,minintensity);//Prepare colour and shading arrays.
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return;
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClearDepth(1.0);
   glEnable(GL_POINT_SMOOTH);//Antialiasing stuff, for use later, possibly.
   glEnable(GL_LINE_SMOOTH);//...
   glEnable(GL_BLEND);//...
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);//...
   glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);//...
   glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);//...
   glViewport(0, 0, get_width(), get_height());
   resetview();
   delete[] buckets;
   delete pointvector;
   glwindow->gl_end();
}

//Prepare the image when the widget is first realised.
void TwoDeeOverview::on_realize(){
  Gtk::GL::DrawingArea::on_realize();
  prepare_image();
}

//Given maximum and minimum values, find out the colour a certain value should be mapped to.
void TwoDeeOverview::colour_by(double value,double maxvalue,double minvalue,double& col1,double& col2,double& col3){//Following comments assume col1=red, col2=green and col3=blue.
  double range = maxvalue-minvalue;
  if(value<=minvalue+range/6){//Green to Yellow:
     col1 = 6*(value-minvalue)/range;
     col2 = 1.0;
     col3 = 0.0;
  }
  else if(value<=minvalue+range/3){//Yellow to Red:
     col1 = 1.0;
     col2 = 2.0 - 6*(value-minvalue)/range;
     col3 = 0.0;
  }
  else if(value<=minvalue+range/2){//Red to Purple:
     col1 = 1.0;
     col2 = 0.0;
     col3 = 6*(value-minvalue)/range - 2.0;
  }
  else if(value<=minvalue+range*2/3){//Purple to Blue:
     col1 = 4.0 - 6*(value-minvalue)/range;
     col2 = 0.0;
     col3 = 1.0;
  }
  else if(value<=minvalue+range*5/6){//Blue to Cyan:
     col1 = 0.0;
     col2 = 6*(value-minvalue)/range - 4.0;
     col3 = 1.0;
  }
  else{//Cyan to White:
     col1 = 6*(value-minvalue)/range - 5.0;
     if(col1>1.0)col1=1.0;
     col2 = 1.0;
     col3 = 1.0;
  }
}

//Given maximum and minimum values, find out the brightness a certain value should be mapped to.
double TwoDeeOverview::brightness_by(double value,double maxvalue,double minvalue,double offsetvalue,double floorvalue){
  double multiplier = floorvalue + offsetvalue + (1.0 - offsetvalue)*(value-minvalue)/(maxvalue-minvalue);
  return multiplier;
}
