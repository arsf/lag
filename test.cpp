#include <iostream>
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "quadtree.h"
#include "LASloader.h"
#include "TwoDeeOverview.h"
using namespace std;

quadtree* lidardata; 
Gtk::ToggleToolButton *profiletoggle = NULL;
TwoDeeOverview *tdo = NULL;

void on_profiletoggle(){
   if(profiletoggle->get_active())tdo->setupprofile();
   else tdo->unsetupprofile();
}

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
      TwoDeeOverview* tdo1 = new TwoDeeOverview(glconfig,lidardata);
      tdo = tdo1;
      tdo->set_size_request(200,200);
//      window2->add(tdo);
      Gtk::VBox *vboxtdo;
      refXml->get_widget("vboxtdo",vboxtdo);
      vboxtdo->pack_end(*tdo,true,true);
      window2->show_all();
      refXml->get_widget("profiletoggle",profiletoggle);
      profiletoggle->signal_toggled().connect(sigc::ptr_fun(&on_profiletoggle));
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
//    LASloader* loader = new LASloader("/users/rsg/arsf/workspace/GB08_12-2009_152a_Borth_Bog/leica/proclaser/LDR090601_110312_1.LAS");
//    lidardata = new quadtree(loader,10000,0);
    LASloader* loader = new LASloader("/users/rsg/arsf/workspace/GB08_12-2009_152a_Borth_Bog/leica/proclaser/LDR090601_110650_1.LAS");

    LASloader* loader2 = new LASloader("/users/rsg/arsf/workspace/GB08_12-2009_152a_Borth_Bog/leica/proclaser/LDR090601_111020_1.LAS");
    LASloader* loader3 = new LASloader("/users/rsg/arsf/workspace/GB08_12-2009_152a_Borth_Bog/leica/proclaser/LDR090601_110312_1.LAS");
    lidardata = new quadtree(loader,100000,0);
    lidardata->load(loader2,0);
    lidardata->load(loader3,0);
    
   return GUIset(argc, argv);
}

