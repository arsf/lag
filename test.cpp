#include <iostream>
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "quadtree.h"
#include "LASloader.h"
#include "TwoDeeOverview.h"
#include "Profile.h"
using namespace std;

quadtree* lidardata; 
Gtk::ToggleToolButton *profiletoggle = NULL;
Gtk::ToggleToolButton *showprofiletoggle = NULL;
TwoDeeOverview *tdo = NULL;
Profile *prof = NULL;
Gtk::SpinButton *profwidthselect = NULL;
int bucketlimit = 100000;

void on_profiletoggle(){
   if(profiletoggle->get_active())tdo->setupprofile();
   else{
   	tdo->unsetupprofile();
	double startx,starty,endx,endy,width;
	tdo->getprofile(startx,starty,endx,endy,width);
	prof->showprofile(startx,starty,endx,endy,width);
   }
}

void on_showprofiletoggle(){
   tdo->setshowprofile(showprofiletoggle->get_active());
   tdo->drawviewable(1);
}

void on_profwidthselected(){
   tdo->setprofwidth(profwidthselect->get_value());
   tdo->makeprofbox();
   tdo->drawviewable(2);
}

int GUIset(int argc,char *argv[]){
   Gtk::Main gtkmain(argc, argv);
   Glib::RefPtr<Gnome::Glade::Xml> refXml;
   try{ refXml = Gnome::Glade::Xml::create("test.glade"); }
   catch(const Gnome::Glade::XmlError& ex){ std::cerr << ex.what() << std::endl; return 1; }
   Gtk::GL::init(argc, argv);
   Gtk::Window *windowp = NULL;
   refXml->get_widget("window1", windowp);
   if (windowp) {
      windowp->set_title("Test");
      Gtk::Window *window2 = NULL;
      refXml->get_widget("window2", window2);
      if(window2){
         window2->set_title("Window2");
         window2->set_reallocate_redraws(true);
         Gtk::VBox *vboxtdo = NULL;
         refXml->get_widget("vboxtdo",vboxtdo);
         if(vboxtdo){
            refXml->get_widget("profiletoggle",profiletoggle);
            if(profiletoggle)profiletoggle->signal_toggled().connect(sigc::ptr_fun(&on_profiletoggle));
            refXml->get_widget("showprofiletoggle",showprofiletoggle);
            if(showprofiletoggle)showprofiletoggle->signal_toggled().connect(sigc::ptr_fun(&on_showprofiletoggle));
            refXml->get_widget("profwidthselect",profwidthselect);
            if(profwidthselect){
               profwidthselect->set_range(0,300);
               profwidthselect->set_value(5);
               profwidthselect->signal_value_changed().connect(sigc::ptr_fun(&on_profwidthselected));
            }
            Glib::RefPtr<Gdk::GL::Config> glconfig;
            glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |      Gdk::GL::MODE_DEPTH  |     Gdk::GL::MODE_DOUBLE);
            if (glconfig==NULL){
                glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |    Gdk::GL::MODE_DEPTH);
                if(glconfig==NULL)std::exit(1);
            }
            TwoDeeOverview* tdo1 = new TwoDeeOverview(glconfig,lidardata,bucketlimit);
            tdo = tdo1;
            tdo->set_size_request(200,200);
            tdo->setprofwidth(profwidthselect->get_value());
            vboxtdo->pack_end(*tdo,true,true);
         }
         window2->show_all();
      }
      Gtk::Window *window3 = NULL;
      refXml->get_widget("window3", window3);
      if(window3){
         window3->set_title("Window3");
         Gtk::VBox *vboxprof = NULL;
         refXml->get_widget("vboxprof",vboxprof);
         if(vboxprof){
            Glib::RefPtr<Gdk::GL::Config> glconfig2;
            glconfig2 = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |      Gdk::GL::MODE_DEPTH  |     Gdk::GL::MODE_DOUBLE);
            if (glconfig2==NULL){
               glconfig2 = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |    Gdk::GL::MODE_DEPTH);
               if(glconfig2==NULL)std::exit(1);
            }
            Profile* prof1 = new Profile(glconfig2,lidardata,bucketlimit);
            prof = prof1;
            prof->set_size_request(200,200);
            vboxprof->pack_end(*prof,true,true);
         }
         window3->show_all();
      }
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
    try{
//       LASloader* loader = new LASloader("MtStHelens.las");
       LASloader* loader = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_111020_1.LAS");
//       LASloader* loader2 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_110650_1.LAS");
       LASloader* loader3 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_110312_1.LAS");
//       LASloader* loader4 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_111411_1.LAS");
//       LASloader* loader5 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_110650_1.LAS");
       LASloader* loader6 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_111753_1.LAS");
       LASloader* loader7 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_112149_1.LAS");
       LASloader* loader8 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_112524_1.LAS");
       LASloader* loader9 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_112915_1.LAS");
       LASloader* loadera = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_113327_1.LAS");
//       LASloader* loaderb = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_114640_1.LAS");
    //   LASloader* loader = new LASloader("/users/rsg/arsf/workspace/IPY09_10-2009_217_Russell/leica/proc_laser/LDR090805_131613_1.LAS");
      // LASloader* loader2 = new LASloader("/users/rsg/arsf/workspace/IPY09_10-2009_217_Russell/leica/proc_laser/LDR090805_141441_1.LAS");
       lidardata = new quadtree(loader,bucketlimit,3);
//       lidardata->load(loader2,3);
       lidardata->load(loader3,3);
//       lidardata->load(loader4,3);
//       lidardata->load(loader5,3);
       lidardata->load(loader6,3);
       lidardata->load(loader7,3);
       lidardata->load(loader8,3);
       lidardata->load(loader9,3);
       lidardata->load(loadera,3);
//       lidardata->load(loaderb,0);
       delete loader;
//       delete loader2;
       delete loader3;
//       delete loader4;
//       delete loader5;
       delete loader6;
       delete loader7;
       delete loader8;
       delete loader9;
       delete loadera;
//       delete loaderb;
    }
    catch(char const* e){
       cout << e << endl;
       cout << "bugger" << endl;
    }
    
   return GUIset(argc, argv);
   delete lidardata;
}

