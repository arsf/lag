/*
 * File: Profile.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: December 2009 - January 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "quadtree.h"
#include "quadtreestructs.h"
#include <vector>
#include <boost/bind.hpp>
class Profile : public Gtk::GL::DrawingArea
{
public:
   Profile(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit,Gtk::Label *rulerlabel);
   ~Profile();
   bool returntostart();//Return to the initial view of the image.
   void prepare_image();//Reads from subset of quadtree and prepares image for drawing.
   bool drawviewable(int imagetype);//Draw the viewable part of the image.
   //Public methods:
   void makerulerbox();//Make rectangle showing where the ruler is.
   bool showprofile(double startx,double starty,double endx,double endy,double width);//Gets the parameters of the profile and then draws it to the screen.
   void setupruler(){//Blocks pan signals and unblocks ruler signals:
      sigpanstart.block();
      sigpan.block();
      sigpanend.block();
      sigrulerstart.unblock();
      sigruler.unblock();
      sigrulerend.unblock();
      if(is_realized())get_window()->set_cursor(*(new Gdk::Cursor(Gdk::CROSSHAIR)));
      rulering=true;
   }
   void unsetupruler(){//Blocks ruler signals and unblocks pan signals:
      sigpanstart.unblock();
      sigpan.unblock();
      sigpanend.unblock();
      sigrulerstart.block();
      sigruler.block();
      sigrulerend.block();
      if(is_realized())get_window()->set_cursor();
      rulering=false;
   }
   //Setters:
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
   void setdrawpoints(bool drawpoints){this->drawpoints=drawpoints;}
   void setdrawmovingaverage(bool drawmovingaverage){this->drawmovingaverage=drawmovingaverage;}
   void setmavrgrange(int mavrgrange){this->mavrgrange=mavrgrange;}
protected:
   double zoompower;//Determines how much the image zooms, as it affects the call to pow(a,b).
   bool drawpoints;//Determines whether points are drawn.
   bool drawmovingaverage;//Determines whether the best fit line is drawn.
   int mavrgrange;//Defines the range of the moving average, with 0 meaning no averaging.
   bool imageexists;//Determines whether to draw anything, based on the existance or nonexistance of anything to draw.
   Gtk::Label *rulerlabel;//Label showing the distance, in various dimensions, covered by the ruler.
   Glib::RefPtr<Gdk::GL::Context> glcontext;//Possibly part of solution to shared viewport problem.
   //Point data and related stuff:
   quadtree* lidardata;//The point data is stored here.
   boundary* lidarboundary;//This stores the boundary of the file opened.
   pointbucket** buckets;//This stores the buckets containing the points to be drawn.
   int numbuckets;//The number of buckets in buckets.
   bool** correctpointsbuckets;//This stores, for each point in each bucket, whether the point is inside the boundaries of the profile and, therefore, whether the point should be drawn.
   int bucketlimit;//This is the maximum number of points a single bucket can contain.
   double maindetailmod,previewdetailmod;//These modify the amount of points skipped for each point not, when drawing. Lower is means more detail, higher means less.
   double pointsize;//The diameter of the points in pixels.
 
   //Position variables:
   double zoomlevel;//This is the level of zoom. It starts at 1, i.e. 100%.
   double centrex,centrey,centrez;//These give the centre of the viewport in image terms, rather than screen terms.
   double viewerx,viewery,viewerz;//These give the coordinates of the "eye", looking towards the centre.
   double ratio;//This determines, along with the zoomlevel, the scaling of the image relative to the screen. At zoomlevel 1, the image should just fit within the screen.
   double panstartx,panstarty;//Coordinates of the start of the pan move.
   double startx,starty;//The start coordinates of the profile.
   double endx,endy;//The end coordinates of the profile.
   double width;//The width of the profile.
 
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
   
   //Rulering:
   double rulerstartx, rulerstarty,rulerstartz;//The start coordinates for the ruler.
   double rulerendx, rulerendy,rulerendz;//The end coordinates for the ruler.
   double rulerwidth;//The width of the ruler.
   bool rulering;//Determines whether or not the ruler should be drawn.
 
   //Signal handlers:
   //Panning:
   sigc::connection sigpanstart;
   sigc::connection sigpan;
   sigc::connection sigpanend;
   //Rulering:
   sigc::connection sigrulerstart;
   sigc::connection sigruler;
   sigc::connection sigrulerend;
 
   //Methods:
   bool linecomp(point* a,point* b);
 
   //Drawing:
   void on_realize();//Realises drawing area and calls make_image().
   bool mainimage(pointbucket** buckets,int numbuckets,int detail);//Draw the main image
   bool previewimage(pointbucket** buckets,int numbuckets,int detail);//Draw the preview (for panning etc.).
   bool on_expose_event(GdkEventExpose* event);//Calls draw on an exose event.
 
   //Positioning methods:
   void resetview();//Determines what part of the image is displayed with orthographic projection.
   bool on_zoom(GdkEventScroll* event);//Allows the user to zoom with the mouse wheel.
   bool on_configure_event(GdkEventConfigure* event);//Handles resizing of the window. Calls resetview().
   //Panning control:   //These allow the user to pan by clicking and dragging.
   bool on_pan_start(GdkEventButton* event);
   bool on_pan(GdkEventMotion* event);
   bool on_pan_end(GdkEventButton* event);
   //Rulering control:   //These allow the user to ruler by clicking and dragging.
   bool on_ruler_start(GdkEventButton* event);
   bool on_ruler(GdkEventMotion* event);
   bool on_ruler_end(GdkEventButton* event);
 
   //Colouring and shading:
   void colour_by(double value,double maxvalue,double minvalue,double& col1,double& col2,double& col3);//Colours by a numeric variable.
   double brightness_by(double value,double maxvalue,double minvalue,double offsetvalue,double floorvalue);//Shades by a numeric variable.
   void coloursandshades(double maxz,double minz,int maxintensity,int minintensity);//Prepare colour and brightness arrays.

};
