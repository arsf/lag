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
   virtual bool returntostart() = 0;
   void prepare_image();//Reads from subset of quadtree and prepares variables for colouring etc..
   virtual bool drawviewable(int imagetype) = 0;
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
   void setpreviewdetail(double previewdetailmod){this->previewdetailmod=previewdetailmod;}
   void setskipNonC(bool skipNonC){this->skipNonC = skipNonC;}
   void setskipGround(bool skipGround){this->skipGround = skipGround;}
   void setskipLowVeg(bool skipLowVeg){this->skipLowVeg = skipLowVeg;}
   void setskipMedVeg(bool skipMedVeg){this->skipMedVeg = skipMedVeg;}
   void setskipHighVeg(bool skipHighVeg){this->skipHighVeg = skipHighVeg;}
   void setskipBuildings(bool skipBuildings){this->skipBuildings = skipBuildings;}
   void setskipNoise(bool skipNoise){this->skipNoise = skipNoise;}
   void setskipMass(bool skipMass){this->skipMass = skipMass;}
   void setskipWater(bool skipWater){this->skipWater = skipWater;}
   void setskipOverlap(bool skipOverlap){this->skipOverlap = skipOverlap;}
   void setskipUndefined(bool skipUndefined){this->skipUndefined = skipUndefined;}
protected:
   double zoompower;//The zoomlevel's change is determined by a pow(a,b) call. This variable stores the power.

   //Point data and related stuff:
   quadtree* lidardata;//The point data is stored here.
   int bucketlimit;//This is the maximum number of points a single bucket can contain.
   double maindetailmod,previewdetailmod;//These modify the amount of points skipped for each point not, when drawing. Lower is means more detail, higher means less.
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
   int intensityoffset;//A minimum intensity brightness value that also scales higher values.
   int intensityfloor;//As above, but does not scale anything.
   bool linecolour;//Whether to colour by flightline.
   bool classcolour;//Whether to colour by classification.
   bool returncolour;//Whether to colour by return.
   double rmaxz,rminz;//The maximum and minimum heights, for colour by elevation etc..
   int rmaxintensity,rminintensity;//The maximum and minimum intensity, for brightness by intensity etc..
   double* colourheightarray;//Array containing colours for different heights.
   double* colourintensityarray;//Array containing colours for different intensities.
   double* brightnessheightarray;//Array containing shades for different heights.
   double* brightnessintensityarray;//Array containing shades for different intensities.
   bool skipNonC;
   bool skipGround;
   bool skipLowVeg;
   bool skipMedVeg;
   bool skipHighVeg;
   bool skipBuildings;
   bool skipNoise;
   bool skipMass;
   bool skipWater;
   bool skipOverlap;
   bool skipUndefined;
   
   //Drawing:
   void on_realize();//Realises drawing area and calls prepare_image().
   bool on_expose_event(GdkEventExpose* event);//Calls draw on an expose event.
//   virtual bool mainimage(pointbucket** buckets,int numbuckets,int detail) = 0;//Draw the main image
   virtual bool previewimage(pointbucket** buckets,int numbuckets,int detail) = 0;//Draw the preview (for panning etc.).
 
   //Colouring and shading:
   void colour_by(double value,double maxvalue,double minvalue,double& col1,double& col2,double& col3);//Colours by a numeric variable.
   double brightness_by(double value,double maxvalue,double minvalue,double offsetvalue,double floorvalue);//Shades by a numeric variable.
   void coloursandshades(double maxz,double minz,int maxintensity,int minintensity);//Prepare colour and brightness arrays.
 
   //Positioning methods:
   virtual void resetview() = 0;//Determines what part of the image is displayed with orthographic projection.
   virtual bool on_zoom(GdkEventScroll* event) = 0;//Allows the user to zoom with the mouse wheel.
   bool on_configure_event(GdkEventConfigure* event);//Handles resizing of the window. Calls resetview().
};
#endif
