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

TwoDeeOverview::TwoDeeOverview(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata)  : Gtk::GL::DrawingArea(config){
   this->lidardata=lidardata;
   lidarboundary = lidardata->getboundary();
   zoomlevel=1;
   showbuckets=false;
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
   //Coloouring and shading:
   heightcolour = false;
   heightbrightness = false;
   zoffset=0;
   zfloor=0.25;
   intensitycolour = false;
   intensitybrightness = false;
   intensityoffset = 0.0/3;
   intensityfloor = 0;
   //Events and signals:
   add_events(Gdk::SCROLL_MASK   |   Gdk::BUTTON1_MOTION_MASK   |   Gdk::BUTTON_PRESS_MASK   |   Gdk::BUTTON_RELEASE_MASK);
   signal_scroll_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_zoom));
   signal_button_press_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_pan_start));
   signal_motion_notify_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_pan));
   signal_button_release_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_pan_end));
}

TwoDeeOverview::~TwoDeeOverview(){}

//This determine what part of the image is displayed with orthographic projection. It sets the active matrix to that of projection and makes it the identity matrix, and then defines the limits of the viewing area using offsets from the centre. *ratio*zoomlevel is there to convert screen dimensions to image dimensions.
void TwoDeeOverview::resetview(){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(centrex-get_width()/2*ratio/zoomlevel,
          centrex+get_width()/2*ratio/zoomlevel,
	  centrey-get_height()/2*ratio/zoomlevel,
	  centrey+get_height()/2*ratio/zoomlevel,
	  -10.0,5.0);
}

//Draw on expose. 1 indicates that the non-preview image is drawn.
bool TwoDeeOverview::on_expose_event(GdkEventExpose* event){ return draw(1); }

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
      return draw(2);
}

bool TwoDeeOverview::on_pan_end(GdkEventButton* event){
   if(event->button==1){
      double tempx=centrex;
      double tempy=centrey;
      tempx = tempx + (event->x-panstartx)*ratio/zoomlevel;
      tempy = tempy - (event->y-panstarty)*ratio/zoomlevel;//Y is reversed because gtk has origin at top left and opengl has it at bottom left.
      centrex = tempx;
      centrey = tempy;
      panstartx=event->x;
      panstarty=event->y;
      resetview();
      return draw(1);
   }
   else return false;
}

bool TwoDeeOverview::on_zoom(GdkEventScroll* event){
   double tempx=centrex;
   double tempy=centrey;
   tempx = tempx + (event->x-get_width()/2)*ratio/zoomlevel;
   tempy = tempy - (event->y-get_height()/2)*ratio/zoomlevel;
//Y is reversed because gtk has origin at top left and opengl has it at bottom left.
   if(zoomlevel>=1){
      if(event->direction==GDK_SCROLL_UP)zoomlevel+=sqrt(zoomlevel)/2;
      else if(event->direction==GDK_SCROLL_DOWN)zoomlevel-=sqrt(zoomlevel)/2;
   }
   else if(zoomlevel>=0.2){
      if(event->direction==GDK_SCROLL_UP)zoomlevel+=0.1;
      else if(event->direction==GDK_SCROLL_DOWN)zoomlevel-=0.1;
   }
   else if(event->direction==GDK_SCROLL_UP)zoomlevel+=0.1;
   tempx = tempx - (event->x-get_width()/2)*ratio/zoomlevel;
   tempy = tempy + (event->y-get_height()/2)*ratio/zoomlevel;
   centrex = tempx;
   centrey = tempy;
   resetview();
   return draw(1);
}

bool TwoDeeOverview::draw(int listno){
   cout << "Getting gl_window." << endl;
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return false;
   cout << "Clearing buffers." << endl;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   cout << "Calling lists." << endl;
   if(showbuckets)glCallList(3);
   glCallList(listno);
   cout << "Flushing." << endl;
   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   else glFlush();
   cout << "Ending gl session." << endl;
   glwindow->gl_end();
   cout << "Drawn." << endl;
   return true;
}

bool TwoDeeOverview::on_configure_event(GdkEventConfigure* event){
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))return false;
  glViewport(0, 0, get_width(), get_height());
  resetview();
  glwindow->gl_end();
  return true;
}

void TwoDeeOverview::bucketimage(pointbucket** buckets,unsigned long numbuckets){
   double red,green,blue;
   glNewList(3, GL_COMPILE);
      for(int i=0;i<numbuckets;i++){
         red=green=blue=1;
	 glColor3d(red,green,blue);
	 glBegin(GL_LINE_LOOP);
	    glVertex2d(buckets[i]->minx, buckets[i]->miny);
	    glVertex2d(buckets[i]->minx, buckets[i]->maxy);
	    glVertex2d(buckets[i]->maxx, buckets[i]->maxy);
 	    glVertex2d(buckets[i]->maxx, buckets[i]->miny);
 	 glEnd();
      }
   glEndList();
}

void TwoDeeOverview::mainimage(pointbucket** buckets,unsigned long numbuckets,double maxz,double minz,double maxintensity,double minintensity){
   double red,green,blue;
   double z=0,intensity=0;
   glNewList(1, GL_COMPILE);
   glBegin(GL_POINTS);
   for(int i=0;i<numbuckets;i++){
     for(int j=0;j<buckets[i]->numberofpoints;j++){
         red = 0.0; green = 1.0; blue = 0.0;
         z = buckets[i]->points[j].z;
         intensity = buckets[i]->points[j].intensity;
         if(heightcolour)colour_by(z,maxz,minz,red,green,blue);
         else if(intensitycolour)colour_by(intensity,maxintensity,minintensity,red,green,blue);
         if(intensitybrightness)brightness_by(intensity,maxintensity,minintensity,intensityoffset,intensityfloor,red,green,blue);
         else if(heightbrightness)brightness_by(z,maxz,minz,zoffset,zfloor,red,green,blue);
         glColor3d(red,green,blue);
         glVertex2d(buckets[i]->points[j].x,buckets[i]->points[j].y);
      }
   }
      glEnd();
     
   glBegin(GL_LINE_LOOP);
      glVertex2d(lidarboundary->minX, lidarboundary->minY);
      glVertex2d(lidarboundary->minX, lidarboundary->maxY);
      glVertex2d(lidarboundary->maxX, lidarboundary->maxY);
      glVertex2d(lidarboundary->maxX, lidarboundary->minY);
   glEnd();
  
   glEndList();
}

void TwoDeeOverview::previewimage(pointbucket** buckets,unsigned long numbuckets,double maxz,double minz,double maxintensity,double minintensity){
   double red,green,blue;
   double z=0,intensity=0;
   glNewList(2, GL_COMPILE);
      glBegin(GL_POINTS);
	 for(int i=0;i<numbuckets;i++){
	    for(int j=0;j<buckets[i]->numberofpoints;j+=25){
               red = 0.0; green = 1.0; blue = 0.0;
	       z = buckets[i]->points[j].z;
	       intensity = buckets[i]->points[j].intensity;
	       red = 0.0; green = 1.0; blue = 0.0;
	       if(heightcolour)colour_by(z,maxz,minz,red,green,blue);
	       else if(intensitycolour)colour_by(intensity,maxintensity,minintensity,red,green,blue);
	       if(intensitybrightness)brightness_by(intensity,maxintensity,minintensity,intensityoffset,intensityfloor,red,green,blue);
	       else if(heightbrightness)brightness_by(z,maxz,minz,zoffset,zfloor,red,green,blue);
	       glColor3d(red,green,blue);
	       glVertex2d(buckets[i]->points[j].x,buckets[i]->points[j].y);
	    }
 	 }
      glEnd();
   glEndList();
}

void TwoDeeOverview::make_image(){
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return;
   glColor3f(0.0,1.0,0.0);
   cout << "Acquiring subset." << endl;
   vector<pointbucket*> *pointvector = lidardata->subset(lidarboundary->minX,lidarboundary->minY,lidarboundary->maxX,lidarboundary->maxY);
   //vector<pointbucket*> *pointvector = lidardata->advsubset(lidarboundary->minX,lidarboundary->maxY-1000,lidarboundary->maxX,lidarboundary->maxY-1000,300);
   cout << "Converting vector." << endl;
   unsigned long numbuckets = pointvector->size();
   pointbucket** buckets = new pointbucket*[numbuckets];
   for(int i=0;i<numbuckets;i++){
      buckets[i]=pointvector->at(i);
   }
   cout << "Finding maxima and minima." << endl;
   double maxz = buckets[0]->maxz,minz = buckets[0]->minz;
   double maxintensity = buckets[0]->maxintensity;
   double minintensity = buckets[0]->minintensity;
   for(int i=0;i<numbuckets;i++){
//Perhaps need a better method in order to find 10th and 90th percentiles instead:
      if(maxz<buckets[i]->maxz)maxz = buckets[i]->maxz;
      if(minz>buckets[i]->minz)minz = buckets[i]->minz;
      if(maxintensity<buckets[i]->maxintensity)maxintensity = buckets[i]->maxintensity;
      if(minintensity>buckets[i]->minintensity)minintensity = buckets[i]->minintensity;
   }
   if(maxz<=minz)maxz=minz+1;
   cout << "Getting bucket image." << endl;
   double red,green,blue;
   double z=0,intensity=0;
   bucketimage(buckets,numbuckets);
   cout << "Getting main image." << endl;
   mainimage(buckets,numbuckets,maxz,minz,maxintensity,minintensity);
   cout << "Getting preview image." << endl;
   previewimage(buckets,numbuckets,maxz,minz,maxintensity,minintensity);
   cout << "Getting clear colour and depth." << endl;
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClearDepth(1.0);
/*   glEnable(GL_POINT_SMOOTH);     //Antialiasing stuff, for use later, possibly.
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);*/
   cout << "Setting viewport." << endl;
   glViewport(0, 0, get_width(), get_height());
   cout << "Getting identity matrix for projection." << endl;
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   cout << "Calling resetview function." << endl;
   resetview();
   cout << "Ending gl session." << endl;
   glwindow->gl_end();
   cout << "Images made." << endl;
}

void TwoDeeOverview::on_realize(){
  Gtk::GL::DrawingArea::on_realize();
  make_image();
}

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
     col2 = 1.0;
     col3 = 1.0;
  }
}

void TwoDeeOverview::brightness_by(double value,double maxvalue,double minvalue,double offsetvalue,double floorvalue,double& col1,double& col2,double& col3){
  double multiplier = floorvalue + offsetvalue + (1.0 - offsetvalue)*(value-minvalue)/(maxvalue-minvalue);
  col1*=multiplier;
  col2*=multiplier;
  col3*=multiplier;
}
