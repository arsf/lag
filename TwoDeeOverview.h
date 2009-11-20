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
#include "quadtree.h"
#include "quadtreestructs.h"
#include "liblas/laspoint.hpp"
#include "liblas/lasreader.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
class TwoDeeOverview : public Gtk::GL::DrawingArea
{
public:
  TwoDeeOverview(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata);
  ~TwoDeeOverview();
protected:
  quadtree* lidardata;
  boundary* lidarboundary;
  double zoomlevel;
  double zoomcentrex;
  double zoomcentrey;
  double xoffset;
  double yoffset;
  double centrex,centrey;
  double xdif,ydif;
  double ratio;
  bool heightcolour;
  bool intensitybrightness;
  double intensityoffset;
  double panstartx,panstarty;
  void on_realize();
  bool on_configure_event(GdkEventConfigure* event);
  bool on_expose_event(GdkEventExpose* event);
  void make_image();
  void resetview();
  bool draw();
  bool on_zoom(GdkEventScroll* event);
  bool on_pan_start(GdkEventButton* event);
  bool on_pan(GdkEventMotion* event);
  bool on_pan_end(GdkEventButton* event);
};
