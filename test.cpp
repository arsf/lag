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
#include "LASloader.h"
#include "liblas/laspoint.hpp"
#include "liblas/lasreader.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "TwoDeeOverview.h"
using namespace std;

quadtree* lidardata; 

int GUIset(int argc,char *argv[]){
   Gtk::Main gtkmain(argc, argv);
   Glib::RefPtr<Gnome::Glade::Xml> refXml;
   try{ refXml = Gnome::Glade::Xml::create("test.glade"); }
   catch(const Gnome::Glade::XmlError& ex){ std::cerr << ex.what() << std::endl; return 1; }
   Gtk::GL::init(argc, argv);
   Glib::RefPtr<Gdk::GL::Config> glconfig;
   glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |      Gdk::GL::MODE_DEPTH  |     Gdk::GL::MODE_DOUBLE);
   if (glconfig==NULL){
       glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |    Gdk::GL::MODE_DEPTH);
       if(glconfig==NULL)std::exit(1);
   }
   Gtk::Window *windowp = NULL;
   refXml->get_widget("window1", windowp);
   if (windowp) {
      windowp->set_title("Test");
      Gtk::Window *window2 = NULL;
      refXml->get_widget("window2", window2);
      window2->set_title("Window2");
      window2->set_reallocate_redraws(true);
      TwoDeeOverview tdo(glconfig,lidardata);
      tdo.set_size_request(200,200);
      window2->add(tdo);
      window2->show_all();
//      Gtk::Window *window3 = NULL;
//      refXml->get_widget("window3", window3);
//      window3->set_title("Window3");
//      window3->show();
//      Gtk::Window *window4 = NULL;
//      refXml->get_widget("window4", window4);
//      window4->set_title("Window4");
//      window4->show();
//      gtkmain.run(*windowp);   
      gtkmain.run(*window2);
   } else {
      std::cerr << "eep, no main window?" << std::endl;
      return 1;
   }
   return 0;
}

int main(int argc, char** argv) {
    //glutInit(&argc, argv);
    LASloader* loader = new LASloader("/users/rsg/arsf/workspace/GB08_12-2009_152a_Borth_Bog/leica/proclaser/LDR090601_110650_1.LAS");

    LASloader* loader2 = new LASloader("/home/scratch/LDR090601_112524_1.LAS");
    lidardata = new quadtree(loader,10000,0);
    lidardata->load(loader2,0);
    
//    lidardata->load("/users/rsg/arsf/workspace/GB08_12-2009_152a_Borth_Bog/leica/proclaser/LDR090601_110650_1.LAS",0);
//    lidardata->load("/users/rsg/arsf/workspace/GB08_12-2009_152a_Borth_Bog/leica/proclaser/LDR090601_111020_1.LAS",0);
   return GUIset(argc, argv);
}

