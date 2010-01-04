#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "quadtree.h"
#include "quadtreestructs.h"
#include <vector>
class TwoDeeOverview : public Gtk::GL::DrawingArea
{
public:
   TwoDeeOverview(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit);
   ~TwoDeeOverview();
   bool returntostart();
   bool drawviewable(int imagetype);//Draw the viewable part of the image.
   void makeprofbox();//Make the box showing the profile area.
   //Short, status changing methods:
   void setupprofile(){//Blocks pan signals and unblocks profile signals:
      sigpanstart.block();
      sigpan.block();
      sigpanend.block();
      sigprofstart.unblock();
      sigprof.unblock();
      sigprofend.unblock();
      this->get_window()->set_cursor(*(new Gdk::Cursor(Gdk::CROSSHAIR)));
      profiling=true;
      drawviewable(1);
   }
   void unsetupprofile(){//Blocks profile signals and unblocks pan signals:
      sigpanstart.unblock();
      sigpan.unblock();
      sigpanend.unblock();
      sigprofstart.block();
      sigprof.block();
      sigprofend.block();
      this->get_window()->set_cursor();
      profiling=false;
      drawviewable(1);
   }
   //Getters:
   void getprofile(double &startx,double &starty,double &endx,double &endy,double &width){//Get coordinates for profile.
      startx = profstartx;
      starty = profstarty;
      endx = profendx;
      endy = profendy;
      width = profwidth;
   }
   //Setters:
   void setprofwidth(double profwidth){this->profwidth = profwidth;}//Set width of the profile.
   void setshowprofile(double showprofile){this->showprofile = showprofile;}//Set whether profile box should be seen when not being modified.
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
protected:
   double zoompower;
   Glib::RefPtr<Gdk::GL::Context> glcontext;//Possibly part of solution to shared viewport problem.
   //Point data and related stuff:
   quadtree* lidardata;//The point data is stored here.
   boundary* lidarboundary;//This stores the boundary of the file opened.
   int bucketlimit;//This is the maximum number of points a single bucket can contain.
   double maindetailmod,previewdetailmod;//These modify the amount of points skipped for each point not, when drawing. Lower is means more detail, higher means less.
   double pointsize;//The diameter of the points.
 
   //Position variables:
   double zoomlevel;//This is the level of zoom. It starts at 1, i.e. 100%.
   double centrex,centrey;//These give the centre of the viewport in image terms, rather than screen terms.
   double ratio;//This determines, along with the zoomlevel, the scaling of the image relative to the screen. At zoomlevel 1, the image should just fit within the screen.
   double panstartx,panstarty;//Coordinates of the start of the pan move.
 
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
   
   //Profiling:
   double profstartx, profstarty;//The start coordinates for the profile.
   double profendx, profendy;//The end coordinates for the profile.
   double profwidth;//The width of the profile.
   bool profiling;//Determines whether or not the profile should be drawn.
   bool showprofile;//Whether to display the profile box when not modifying it.
 
   //Signal handlers:
   //Panning:
   sigc::connection sigpanstart;
   sigc::connection sigpan;
   sigc::connection sigpanend;
   //Profiling:
   sigc::connection sigprofstart;
   sigc::connection sigprof;
   sigc::connection sigprofend;
 
   //Methods:
 
   //Drawing:
   void on_realize();//Realises drawing area and calls prepare_image().
   void prepare_image();//Reads from subset of quadtree and prepares variables for colouring etc..
   bool mainimage(pointbucket** buckets,int numbuckets,int detail);//Draw the main image
   bool previewimage(pointbucket** buckets,int numbuckets,int detail);//Draw the preview (for panning etc.).
   bool on_expose_event(GdkEventExpose* event);//Calls draw on an expose event.
 
   //Positioning methods:
   void resetview();//Determines what part of the image is displayed with orthographic projection.
   bool on_zoom(GdkEventScroll* event);//Allows the user to zoom with the mouse wheel.
   bool on_configure_event(GdkEventConfigure* event);//Handles resizing of the window. Calls resetview().
   //Panning control:   //These allow the user to pan by clicking and dragging.
   bool on_pan_start(GdkEventButton* event);
   bool on_pan(GdkEventMotion* event);
   bool on_pan_end(GdkEventButton* event);
 
   //Colouring and shading:
   void colour_by(double value,double maxvalue,double minvalue,double& col1,double& col2,double& col3);//Colours by a numeric variable.
   double brightness_by(double value,double maxvalue,double minvalue,double offsetvalue,double floorvalue);//Shades by a numeric variable.
   void coloursandshades(double maxz,double minz,int maxintensity,int minintensity);//Prepare colour and brightness arrays.
 
    //Profiling:
   bool on_prof_start(GdkEventButton* event);
   bool on_prof(GdkEventMotion* event);
   bool on_prof_end(GdkEventButton* event);
 
   
};
