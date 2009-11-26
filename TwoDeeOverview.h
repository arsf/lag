#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "quadtree.h"
#include "quadtreestructs.h"
#include <vector>
class TwoDeeOverview : public Gtk::GL::DrawingArea
{
public:
  TwoDeeOverview(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata);
  ~TwoDeeOverview();
   //Short, status changing methods:
   void setupprofile(){
      sigpanstart.block();
      sigpan.block();
      sigpanend.block();
      sigprofstart.unblock();
      sigprof.unblock();
      sigprofend.unblock();
      this->get_window()->set_cursor(*(new Gdk::Cursor(Gdk::CROSSHAIR)));
   }
   void unsetupprofile(){
      sigpanstart.unblock();
      sigpan.unblock();
      sigpanend.unblock();
      sigprofstart.block();
      sigprof.block();
      sigprofend.block();
      this->get_window()->set_cursor();
   }
protected:
  double profstartx, profstarty;
  double profendx, profendy;
  double profwidth;
  int previewindex;
  double maxz,minz;
  double maxintensity,minintensity;
  int numbuckets;
  //Point data and related stuff:
  quadtree* lidardata;//The point data is stored here.
  boundary* lidarboundary;//This stores the boundary of the file opened.
  bool showbuckets;//True if want to display bucket boundaries on image.

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
  double intensityoffset;//A minimum intensity brightness value that also scales higher values.
  double intensityfloor;//As above, but does not scale anything.

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

  void on_realize();//Realises drawing area and calls make_image().
  void make_image();//Reads from subset of quadtree and prepares image for drawing.
  void bucketimage(pointbucket** buckets,int numbuckets);
  void mainimage(pointbucket** buckets,int numbuckets,double maxz,double minz,double maxintensity,double minintensity);
  void previewimage(pointbucket** buckets,int numbuckets,double maxz,double minz,double maxintensity,double minintensity);
  bool draw(int listno);//Draws the image. listno determines which version of the image to draw.
  bool drawviewable();
  bool on_configure_event(GdkEventConfigure* event);//Handles resizing of the window. Calls resetview().
  bool on_expose_event(GdkEventExpose* event);//Calls draw on an exose event.

  //Positioning methods:
  void resetview();//Determines what part of the image is displayed with orthographic projection.
  bool on_zoom(GdkEventScroll* event);//Allows the user to zoom with the mouse wheel.
  //Panning control:   //These allow the user to pan by clicking and dragging.
  bool on_pan_start(GdkEventButton* event);
  bool on_pan(GdkEventMotion* event);
  bool on_pan_end(GdkEventButton* event);

  //Colouring and shading:
  void colour_by(double value,double maxvalue,double minvalue,double& col1,double& col2,double& col3);//Colours by a numeric variable.
  void brightness_by(double value,double maxvalue,double minvalue,double offsetvalue,double floorvalue,double& col1,double& col2,double& col3);//Shades by a numberic variable.

   //Profiling:
   bool on_prof_start(GdkEventButton* event);
   bool on_prof(GdkEventMotion* event);
   bool on_prof_end(GdkEventButton* event);

   
};
