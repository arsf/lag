/*
 * File: Display.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: January 2010
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
#include "Display.h"
#include "MathFuncs.h"
#include "Display.h"

Display::Display(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit) : Gtk::GL::DrawingArea(config){
   this->lidardata=lidardata;
   this->bucketlimit = bucketlimit;
   pointsize=1;
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
   classcolour = false;
   returncolour = false;
   colourheightarray = new double[2];
   colourintensityarray = new double[2];
   brightnessheightarray = new double[2];
   brightnessintensityarray = new double[2];
}

Display::~Display(){
   delete[] colourheightarray;
   delete[] colourintensityarray;
   delete[] brightnessheightarray;
   delete[] brightnessintensityarray;
//   delete lidarboundary;
}

//Draw on expose. 1 indicates that the non-preview image is drawn.
bool Display::on_expose_event(GdkEventExpose* event){
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return false;
   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   else glFlush();
   return true;
}

//When the window is resized, the viewport is resized accordingly and so are the viewing properties.
bool Display::on_configure_event(GdkEventConfigure* event){
  glViewport(0, 0, get_width(), get_height());
  resetview();
  return true;
}

//Prepares the arrays for looking up the colours and shades of the points.
void Display::coloursandshades(double maxz,double minz,int maxintensity,int minintensity){
   delete[] colourheightarray;
   delete[] colourintensityarray;
   delete[] brightnessheightarray;
   delete[] brightnessintensityarray;
   double red=0.0,green=0.0,blue=0.0;
   double z=0,intensity=0;
   colourheightarray = new double[30*(int)(rmaxz-rminz+4)];
   for(int i=0;i<(int)(10*(rmaxz-rminz)+3);i++){//Fill height colour array:
      z = 0.1*(double)i + rminz;
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

//Prepare the image when the widget is first realised.
void Display::on_realize(){
  Gtk::GL::DrawingArea::on_realize();
  prepare_image();
}

//Given maximum and minimum values, find out the colour a certain value should be mapped to.
void Display::colour_by(double value,double maxvalue,double minvalue,double& col1,double& col2,double& col3){//Following comments assume col1=red, col2=green and col3=blue.
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
double Display::brightness_by(double value,double maxvalue,double minvalue,double offsetvalue,double floorvalue){
  double multiplier = floorvalue + offsetvalue + (1.0 - offsetvalue)*(value-minvalue)/(maxvalue-minvalue);
  return multiplier;
}

//This method prepares the image for drawing and sets up OpenGl. It gets the data from the quadtree in order to find the maximum and minimum height and intensity values and calls the coloursandshades() method to prepare the colouring of the points. It also sets ups anti-aliasing, clearing and the initial view.
void Display::prepare_image(){
   //Initial state:
   boundary* lidarboundary = lidardata->getboundary();
   double xdif = lidarboundary->maxX-lidarboundary->minX;
   double ydif = lidarboundary->maxY-lidarboundary->minY;
      //Scaling to screen dimensions:
      double swidth = get_screen()->get_width();
      double sheight = get_screen()->get_height();
      double xratio = xdif/swidth;
      double yratio = ydif/sheight;
      yratio*=1.3;
      ratio = 0;
      if(xratio>yratio)ratio = xratio;
      else ratio = yratio;
   vector<pointbucket*> *pointvector;
   try{
      pointvector = lidardata->subset(lidarboundary->minX,lidarboundary->minY,lidarboundary->maxX,lidarboundary->maxY);//Get ALL data.
   }catch(const char* e){
      cout << e << endl;
      cout << "No points returned." << endl;
      return;
   }
   int numbuckets = pointvector->size();
   pointbucket** buckets = new pointbucket*[numbuckets];
   for(int i=0;i<numbuckets;i++){
      buckets[i]=pointvector->at(i);
   }
   double maxz = buckets[0]->maxz,minz = buckets[0]->minz;
   int maxintensity = buckets[0]->maxintensity,minintensity = buckets[0]->minintensity;
   for(int i=0;i<numbuckets;i++){//Find the maximum and minimum values from the buckets:
//      bool skip = false;
//      if(skipNonC ||
//         skipGround ||
//         skipLowVeg ||
//         skipMedVeg ||
//         skipHighVeg ||
//         skipBuildings ||
//         skipNoise ||
//         skipMass ||
//         skipWater ||
//         skipOverlap ||
//         skipUndefined){
//         classification = buckets[i]->points[j].classification;
//         int index = classification;
//         double incrementor = 2*abs(rmaxz-rminz);
//         switch(index){
//            case 0:case 1:if(skipNonC)skip=true;break;
//            case 2:if(skipGround)skip=true;break;
//            case 3:if(skipLowVeg)skip=true;break;
//            case 4:if(skipMedVeg)skip=true;break;
//            case 5:if(skipHighVeg)skip=true;break;
//            case 6:if(skipBuildings)skip=true;break;
//            case 7:if(skipNoise)skip=true;break;
//            case 8:if(skipMass)skip=true;break;
//            case 9:if(skipWater)skip=true;break;
//            case 12:if(skipOverlap)skip=true;break;
//            default:if(skipUndefined)skip=true;break;
//         }
//      }
//      if(!skip)
         if(maxz<buckets[i]->maxz)maxz = buckets[i]->maxz;
         if(minz>buckets[i]->minz)minz = buckets[i]->minz;
         if(maxintensity<buckets[i]->maxintensity)maxintensity = buckets[i]->maxintensity;
         if(minintensity>buckets[i]->minintensity)minintensity = buckets[i]->minintensity;
//      }
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
   coloursandshades(maxz,minz,maxintensity,minintensity);//Prepare colour and shading arrays.
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return;
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClearDepth(1.0);
   glPointSize(pointsize);
   glEnable(GL_DEPTH_TEST);//Very important to include this! This allows us to see the things on the top above the things on the bottom!
   glViewport(0, 0, get_width(), get_height());
   resetview();
   delete[] buckets;
   delete pointvector;
   delete lidarboundary;
   glwindow->gl_end();
   returntostart();
}
