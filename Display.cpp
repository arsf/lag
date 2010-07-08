/*
 * File: Display.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: January - June 2010
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
#include <GL/glut.h>
#include "Display.h"
#include "MathFuncs.h"
#include "Display.h"

Display::Display(const Glib::RefPtr<const Gdk::GL::Config>& config,Quadtree* lidardata,int bucketlimit) : Gtk::GL::DrawingArea(config){
   this->lidardata=lidardata;
   this->bucketlimit = bucketlimit;
   pointsize=1;
   ratio = 1;
   cbmaxz=cbminz=0;
   //Colouring and shading:
   heightcolour = false;
   heightbrightness = false;
   zoffset=0;
   zfloor=0;
   intensitycolour = false;
   intensitybrightness = true;
   intensityoffset = 0;
   intensityfloor = 0;
   rmaxz=rminz=0;
   rmaxintensity=rminintensity=0;
   linecolour = true;
   classcolour = false;
   returncolour = false;
   colourheightarray = NULL;
   colourintensityarray = NULL;
   brightnessheightarray = NULL;
   brightnessintensityarray = NULL;
}

Display::~Display(){
   if(colourheightarray!=NULL)delete[] colourheightarray;
   if(colourintensityarray!=NULL)delete[] colourintensityarray;
   if(brightnessheightarray!=NULL)delete[] brightnessheightarray;
   if(brightnessintensityarray!=NULL)delete[] brightnessintensityarray;
}

//Draw on expose.
bool Display::on_expose_event(GdkEventExpose* event){
   drawviewable(3);
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
   cbmaxz = maxz;
   cbminz = minz;
   cbmaxintensity = maxintensity;
   cbminintensity = minintensity;
   if(colourheightarray!=NULL)delete[] colourheightarray;
   if(colourintensityarray!=NULL)delete[] colourintensityarray;
   if(brightnessheightarray!=NULL)delete[] brightnessheightarray;
   if(brightnessintensityarray!=NULL)delete[] brightnessintensityarray;
   double red=0.0,green=0.0,blue=0.0;
   double z=0,intensity=0;
   colourheightarray = new double[30*(int)(rmaxz-rminz+4)];//This is at 30, rather than three, for a reason: three, one for each colour, by ten, so that the "resolution" of the colouring by height is 0.1 metres, not one whole metre. It makes it look better.
   for(int i=0;i<(int)(10*(rmaxz-rminz)+3);i++){//Fill height colour array (by ten for extra detail):
      z = 0.1*(double)i + rminz;//0.1 for 0.1 metres for extra detail.
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
   brightnessheightarray = new double[10*(int)(rmaxz-rminz+4)];//This is at 30, rather than three, for a reason: three, one for each colour, by ten, so that the "resolution" of the colouring by height is 0.1 metres, not one whole metre. It makes it look better.
   for(int i=0;i<10*(int)(rmaxz-rminz+4);i++){//Fill height brightness array:
      z = 0.1*(double)i + rminz;//Please note that, as elsewhere in this method, the exact value rmaxz will not be reached, so its position in the array will probably not be the first to equal 1, while the first element will be 0 and be for rminz.
      brightnessheightarray[i] = brightness_by(z,maxz,minz,zoffset,zfloor);
   }
   brightnessintensityarray = new double[(int)(rmaxintensity-rminintensity+4)];
   for(int i=0;i<rmaxintensity-rminintensity+4;i++){//Fill intensity brightness array:
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
     col2 = 1.0;
     col3 = 1.0;
  }
  if(col1>1.0)col1=1.0;//These prevent the situation where two negative values (for colour and brightness respectively) multiply to create a positive value giving a non-black colour:
  if(col2>1.0)col2=1.0;//...
  if(col3>1.0)col3=1.0;//...
  if(col1<0.0)col1=0.0;//...
  if(col2<0.0)col2=0.0;//...
  if(col3<0.0)col3=0.0;//...
}

//Given maximum and minimum values, find out the brightness a certain value should be mapped to.
double Display::brightness_by(double value,double maxvalue,double minvalue,double offsetvalue,double floorvalue){
  double multiplier = floorvalue + offsetvalue + (1.0 - floorvalue)*(value-minvalue)/(maxvalue-minvalue);
  if(multiplier < 0)multiplier = 0;//This prevents the situation where two negative values (for colour and brightness respectively) multiply to create a positive value giving a non-black colour.
  if(multiplier > 1)multiplier = 1;//This prevents the situation where a non-white colour can be made paler (rather than simply brighter) as a result of its RGB values being multiplied by somenumber greater than 1. Otherwise, non-white colours can eventually be mad white when brightness mode is on.
  return multiplier;
}

bool Display::advsubsetproc(vector<PointBucket*>*& pointvector,double *xs,double *ys,int ps){
   try{
      pointvector = lidardata->advSubset(xs,ys,ps);
   }catch(DescriptiveException e){
      cout << "There has been an exception:" << endl;
      cout << "What: " << e.what() << endl;
      cout << "Why: " << e.why() << endl;
      cout << "No points returned." << endl;
      return false;
   }
   if(pointvector==NULL||pointvector->size()==0){ return false; }
   else return true;
}

bool Display::clearscreen(){
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return false;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   else glFlush();
   glwindow->gl_end();
   return true;
}

void Display::guard_against_interaction_between_GL_areas(){
   get_gl_window()->make_current(get_gl_context());//These are done so that graphical artefacts through changes of view to not occur. This is because of being a multiwindow application. NOTE: This line MUST come before the other ones for this purpose as otherwise the others might be applied to the wrong context!
   glPointSize(pointsize);//...
   glViewport(0, 0, get_width(), get_height());//...
   resetview();//...
}

//This method prepares the image for drawing and sets up OpenGl. It gets data from the quadtree in order to find the maximum and minimum height and intensity values and calls the coloursandshades() method to prepare the colouring of the points. It also sets up clearing and the initial view.
void Display::prepare_image(){
   Boundary* lidarboundary = lidardata->getBoundary();
   double *xs = new double[4];
   xs[0] = lidarboundary->minX;
   xs[1] = lidarboundary->minX;
   xs[2] = lidarboundary->maxX;
   xs[3] = lidarboundary->maxX;
   double *ys = new double[4];
   ys[0] = lidarboundary->minY;
   ys[1] = lidarboundary->maxY;
   ys[2] = lidarboundary->maxY;
   ys[3] = lidarboundary->minY;
   vector<PointBucket*> *pointvector = NULL;
   bool gotdata = advsubsetproc(pointvector,xs,ys,4);//Get ALL data.
   delete[]xs;
   delete[]ys;
   delete lidarboundary;
   if(!gotdata){//If there is no data, then clear the screen to show no data.
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClearDepth(1.0);
      clearscreen();
      if(pointvector!=NULL)delete pointvector;
      return;
   }
   int numbuckets = pointvector->size();
   PointBucket** buckets = new PointBucket*[numbuckets];
   for(int i=0;i<numbuckets;i++){//Convert to pointer for faster access in for loops in image methods. Why? Expect >100000 points.
      buckets[i]=(*pointvector)[i];
   }
   double maxz = buckets[0]->getmaxZ(),minz = buckets[0]->getminZ();
   int maxintensity = buckets[0]->getmaxintensity(),minintensity = buckets[0]->getminintensity();
   for(int i=0;i<numbuckets;i++){//Find the maximum and minimum values from the buckets:
      if(maxz<buckets[i]->getmaxZ())maxz = buckets[i]->getmaxZ();
      if(minz>buckets[i]->getminZ())minz = buckets[i]->getminZ();
      if(maxintensity<buckets[i]->getmaxintensity())maxintensity = buckets[i]->getmaxintensity();
      if(minintensity>buckets[i]->getminintensity())minintensity = buckets[i]->getminintensity();
   }
   if(maxz<=minz)maxz=minz+1;
   if(maxintensity<=minintensity)maxintensity=minintensity+1;
   rmaxz = maxz; rminz = minz;
   rmaxintensity = maxintensity; rminintensity = minintensity;
   coloursandshades(maxz,minz,maxintensity,minintensity);//Prepare colour and shading arrays.
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return;
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClearDepth(1.0);
   glPointSize(pointsize);
   glEnable(GL_DEPTH_TEST);//Very important to include this! This allows us to see the things on the top above the things on the bottom!
   glViewport(0, 0, get_width(), get_height());
   resetview();
   delete pointvector;//Must remain here, as buckets is just a pointer to its contents (and so does not need to be deleted as well) and it has other contents to be removed (presumably).
   delete[]buckets;
   glwindow->gl_end();
   returntostart();//Set up initial view and draw it (though, for some reason, will not actually draw anything if this is the first time (i.e. the widget has just been realised); quite annoying).
}
