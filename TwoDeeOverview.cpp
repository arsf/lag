#include <stdlib.h>
#include <cstdlib> 
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <ctime>
#include <vector>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cmath>
#include "quadtree.h"
#include "quadtreestructs.h"
#include "liblas/laspoint.hpp"
#include "liblas/lasreader.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "TwoDeeOverview.h"

TwoDeeOverview::TwoDeeOverview(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata)  : Gtk::GL::DrawingArea(config){
   this->lidardata=lidardata;
   lidarboundary = lidardata->getboundary();
   zoomlevel=1;
   xoffset=yoffset=0;
   xdif = lidarboundary->maxX-lidarboundary->minX;
   ydif = lidarboundary->maxY-lidarboundary->minY;
   centrex = lidarboundary->minX+xdif/2;
   centrey = lidarboundary->minY+ydif/2;
   double swidth = get_screen()->get_width();
   double sheight = get_screen()->get_height();
   double xratio = xdif/swidth;
   double yratio = ydif/sheight;
   yratio*=1.25;
   ratio = 0;
   if(xratio>yratio)ratio = xratio;
   else ratio = yratio;
//   ratio/=2;
   zoomcentrex = lidarboundary->minX;
   zoomcentrey = lidarboundary->maxY;
   add_events(Gdk::SCROLL_MASK   |   Gdk::BUTTON1_MOTION_MASK   |   Gdk::BUTTON_PRESS_MASK   |   Gdk::BUTTON_RELEASE_MASK);
   signal_scroll_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_zoom));
   heightcolour = false;
   heightbrightness = true;
   zoffset=0;
   zfloor=0.25;
   intensitycolour = true;
   intensitybrightness = false;
   intensityoffset = 0.0/3;
   intensityfloor = 0;
   signal_button_press_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_pan_start));
   signal_motion_notify_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_pan));
   signal_button_release_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_pan_end));
}

TwoDeeOverview::~TwoDeeOverview(){}

void TwoDeeOverview::resetview(){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(centrex-get_width()/2*ratio/zoomlevel,
          centrex+get_width()/2*ratio/zoomlevel,
	  centrey-get_height()/2*ratio/zoomlevel,
	  centrey+get_height()/2*ratio/zoomlevel,
	  -10.0,5.0);
  xdif = get_width()*ratio/zoomlevel;
  ydif = get_width()*ratio/zoomlevel;
}

bool TwoDeeOverview::on_expose_event(GdkEventExpose* event){
  return draw(1);
}

bool TwoDeeOverview::on_pan_start(GdkEventButton* event){
   if(event->button==1){
      panstartx = event->x;
      panstarty = event->y;
   }
   return true;
}

bool TwoDeeOverview::on_pan(GdkEventMotion* event){
//   if(abs(event->x-panstartx)>=10||abs(event->y-panstarty)>=10){
      double tempx=centrex;
      double tempy=centrey;
      tempx = tempx - (event->x-panstartx)*ratio/zoomlevel;
      tempy = tempy + (event->y-panstarty)*ratio/zoomlevel;
//Y is reversed because gtk has origin at top left and opengl has it at bottom left.
      centrex = tempx;
      centrey = tempy;
      panstartx=event->x;
      panstarty=event->y;
      resetview();
      return draw(2);
//   }
//   else return false;
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
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))return false;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCallList(listno);
  if (glwindow->is_double_buffered())glwindow->swap_buffers();
  else glFlush();
  glwindow->gl_end();
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

void TwoDeeOverview::make_image(){
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))return;
  glColor3f(0.0,1.0,0.0);
  //vector<pointbucket*> *pointvector = lidardata->subset(lidarboundary->minX,lidarboundary->minY,lidarboundary->maxX,lidarboundary->maxY);
  vector<pointbucket*> *pointvector = lidardata->advsubset(lidarboundary->minX,lidarboundary->maxY-1000,lidarboundary->maxX,lidarboundary->maxY-1000,300);
  double maxz = pointvector->at(0)->maxz,minz = pointvector->at(0)->minz;
  double maxintensity = pointvector->at(0)->maxintensity;
  double minintensity = pointvector->at(0)->minintensity;
  for(int i=0;i<pointvector->size();i++){
//Perhaps need a better method in order to find 10th and 90th percentiles instead:
     if(maxz<pointvector->at(i)->maxz)maxz = pointvector->at(i)->maxz;
     if(minz>pointvector->at(i)->minz)minz = pointvector->at(i)->minz;
     if(maxintensity<pointvector->at(i)->maxintensity)maxintensity = pointvector->at(i)->maxintensity;
     if(minintensity>pointvector->at(i)->minintensity)minintensity = pointvector->at(i)->minintensity;
  }
  if(maxz<=minz)maxz=minz+1;
  double range = maxz-minz;
  cout << range << endl;
  cout << maxz << " " << minz << endl;
  double red,green,blue;
  glNewList(1, GL_COMPILE);
     glBegin(GL_POINTS);
	for(int i=0;i<pointvector->size();i++){
      red=green=blue=1;
      glColor3d(red,green,blue);
      glBegin(GL_LINE_LOOP);
      glVertex2d(pointvector->at(i)->minx, pointvector->at(i)->miny);
      glVertex2d(pointvector->at(i)->minx, pointvector->at(i)->maxy);
      glVertex2d(pointvector->at(i)->maxx, pointvector->at(i)->maxy);
      glVertex2d(pointvector->at(i)->maxx, pointvector->at(i)->miny);
      glEnd();
   }
   glBegin(GL_POINTS);
   for(int i=0;i<pointvector->size();i++){
	   for(int j=0;j<pointvector->at(i)->numberofpoints;j++){
	      red = 0.0; green = 1.0; blue = 0.0;
	      if(heightcolour)colour_by(pointvector->at(i)->points[j].z,maxz,minz,red,green,blue);
	      else if(intensitycolour)colour_by(pointvector->at(i)->points[j].intensity,maxintensity,minintensity,red,green,blue);
	      if(intensitybrightness)brightness_by(pointvector->at(i)->points[j].intensity,maxintensity,minintensity,intensityoffset,intensityfloor,red,green,blue);
	      else if(heightbrightness)brightness_by(pointvector->at(i)->points[j].z,maxz,minz,zoffset,zfloor,red,green,blue);
	      glColor3d(red,green,blue);
	      glVertex2d(pointvector->at(i)->points[j].x,pointvector->at(i)->points[j].y);
	   }
	}
     glEnd();
     
     glBegin(GL_LINE_LOOP);
  glVertex2d(lidarboundary->minX, lidarboundary->maxY-1150);
  glVertex2d(lidarboundary->minX, lidarboundary->maxY-850);
  glVertex2d(lidarboundary->maxX, lidarboundary->maxY-850);
  glVertex2d(lidarboundary->maxX, lidarboundary->maxY-1150);
  glEnd();
  
  glEndList();
  glNewList(2, GL_COMPILE);
     glBegin(GL_POINTS);
	for(int i=0;i<pointvector->size();i++)
   {
	   for(int j=0;j<pointvector->at(i)->numberofpoints;j+=50)
      {
	      red = 0.0; green = 1.0; blue = 0.0;
	      if(heightcolour)colour_by(pointvector->at(i)->points[j].z,maxz,minz,red,green,blue);
	      else if(intensitycolour)colour_by(pointvector->at(i)->points[j].intensity,maxintensity,minintensity,red,green,blue);
	      if(intensitybrightness)brightness_by(pointvector->at(i)->points[j].intensity,maxintensity,minintensity,intensityoffset,intensityfloor,red,green,blue);
	      else if(heightbrightness)brightness_by(pointvector->at(i)->points[j].z,maxz,minz,zoffset,zfloor,red,green,blue);
	      glColor3d(red,green,blue);
	      glVertex2d(pointvector->at(i)->points[j].x,pointvector->at(i)->points[j].y);
	   }
	}
     glEnd();
   
  glEndList();
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearDepth(1.0);
  glEnable(GL_POINT_SMOOTH);     //Antialiasing stuff, for use later, possibly.
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
  glViewport(0, 0, get_width(), get_height());
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  resetview();
  glwindow->gl_end();
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
