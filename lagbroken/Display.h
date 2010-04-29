/*
 * File: Display.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: January 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "quadtree.h"
#include "quadtreestructs.h"
#include "pointbucket.h"
#include <vector>
#ifndef DISPLAY_H
#define DISPLAY_H
class Display : public Gtk::GL::DrawingArea{
public:
   Display(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit);
   ~Display();
   virtual bool returntostart() = 0;//Subclasses must have this method because this class tries to call it.
   void prepare_image();//Reads from subset of quadtree and prepares variables for colouring etc..
   void coloursandshades(double maxz,double minz,int maxintensity,int minintensity);//Prepare colour and brightness arrays.
   //Getters:
   double getrmaxz(){return rmaxz;}
   double getrminz(){return rminz;}
   double getzoffset(){return zoffset;}
   double getzfloor(){return zfloor;}
   int getrmaxintensity(){return rmaxintensity;}
   int getrminintensity(){return rminintensity;}
   double getintensityoffset(){return intensityoffset;}
   double getintensityfloor(){return intensityfloor;}
   //Setters:
   void setlidardata(quadtree* lidardata,int bucketlimit){this->lidardata=lidardata;this->bucketlimit=bucketlimit;}
   void setintensitycolour(bool intensitycolour){this->intensitycolour=intensitycolour;}
   void setheightcolour(bool heightcolour){this->heightcolour=heightcolour;}
   void setlinecolour(bool linecolour){this->linecolour=linecolour;}
   void setclasscolour(bool classcolour){this->classcolour=classcolour;}
   void setreturncolour(bool returncolour){this->returncolour=returncolour;}
   void setintensitybrightness(bool intensitybrightness){this->intensitybrightness=intensitybrightness;}
   void setheightbrightness(bool heightbrightness){this->heightbrightness=heightbrightness;}
   void setpointwidth(double pointsize){this->pointsize=pointsize;glPointSize(pointsize);}
   void setmaindetail(double maindetailmod){this->maindetailmod=maindetailmod;}
   void setzoffset(double zoffset){this->zoffset = zoffset;}
   void setzfloor(double zfloor){this->zfloor = zfloor;}
   void setintensityoffset(double intensityoffset){this->intensityoffset = intensityoffset;}
   void setintensityfloor(double intensityfloor){this->intensityfloor = intensityfloor;}
protected:
   double zoompower;//The zoomlevel's change is determined by a pow(a,b) call. This variable stores the power.

   //Point data and related stuff:
   quadtree* lidardata;//The point data is stored here.
   int bucketlimit;//This is the maximum number of points a single bucket can contain.
   double maindetailmod;//This modifies the amount of points skipped for each point in the main image, when drawing. Lower is means more detail, higher means less.
   double pointsize;//The diameter of the points in pixels.

   //Position variables:
   double zoomlevel;//This is the level of zoom. It starts at 1, i.e. 100%.
   double ratio;//This determines, along with the zoomlevel, the scaling of the image relative to the screen. At zoomlevel 1, the image should just fit within the screen.
 
   //Colouring and shading variables:
   bool heightcolour;//True if want to colour by height.
   bool heightbrightness;//True if want to shade by height.
   double zoffset;//A minimum height brightness value that also scales higher values.
   double zfloor;//As above, but does not scale anything.
   bool intensitycolour;//True if want to colour by intensity.
   bool intensitybrightness;//True if want to shade by intensity.
   double intensityoffset;//A minimum intensity brightness value that also scales higher values.
   double intensityfloor;//As above, but does not scale anything.
   bool linecolour;//Whether to colour by flightline.
   bool classcolour;//Whether to colour by classification.
   bool returncolour;//Whether to colour by return.
   double rmaxz,rminz;//The maximum and minimum heights, for colour by elevation etc..
   int rmaxintensity,rminintensity;//The maximum and minimum intensity, for brightness by intensity etc..
   double* colourheightarray;//Array containing colours for different heights.
   double* colourintensityarray;//Array containing colours for different intensities.
   double* brightnessheightarray;//Array containing shades for different heights.
   double* brightnessintensityarray;//Array containing shades for different intensities.
   
   //Methods:

   //Drawing:
   void on_realize();//Realises drawing area and calls prepare_image().
   bool on_expose_event(GdkEventExpose* event);//Calls draw on an expose event.
 
   //Colouring and shading:
   void colour_by(double value,double maxvalue,double minvalue,double& col1,double& col2,double& col3);//Colours by a numeric variable.
   double brightness_by(double value,double maxvalue,double minvalue,double offsetvalue,double floorvalue);//Shades by a numeric variable.
 
   //Positioning methods:
   virtual void resetview() = 0;//Determines what part of the image is displayed with orthographic projection.
   bool on_configure_event(GdkEventConfigure* event);//Handles resizing of the window. Calls resetview().
};
#endif
