/*
 * File: lag.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: November 2009 - June 2010
 *
 * */
#include <iostream>
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "Quadtree.h"
#include "TwoDeeOverview.h"
#include "Profile.h"
#include "TwoDeeOverviewWindow.h"
#include "ProfileWindow.h"
#include "FileOpener.h"
using namespace std;

//Takes the path the executable was called with and uses it to find the .glade file needed to make the GUI.
string findgladepath(char* programpath){
   string exename = programpath;//The path of the executable.
   unsigned int index = exename.rfind("/");//Find the last forward slash and make its index our index.
   if(index==string::npos)index=0;//I.e. in the event that there is no forward slash (so must be calling from the same directory), just go from 0, where the forward slash would have been.
   else index++;//We do not actually want to include the forward slash.
   string gladename = exename;
   gladename.replace(index,9,"lag.glade");
   cout << exename << endl;
   cout << gladename << endl;
   return gladename;
}

int main(int argc, char** argv) {
   cout << "Build number: 2010.07.06.1" << endl;
   Glib::thread_init();//This allows the creation and running of threads.
   Gtk::Main gtkmain(argc, argv);//This is required for GTK to work. It must be the first GTK object created and may not be global.
   Glib::RefPtr<Gnome::Glade::Xml> refXml;//This will extract widgets from the glade file when directed.
   try{
      refXml = Gnome::Glade::Xml::create(findgladepath(argv[0]));//Assign the glade file used to build the GUI.
   }
   catch(const Gnome::Glade::XmlError& ex){ 
      cerr << ex.what() << std::endl;
      cerr << "The file lag.glade must be located in the same directory as the lag executable." << endl;
      return 1;
   }
   Gtk::GL::init(argc, argv);
   Glib::RefPtr<Gdk::GL::Config> glconfig;
   glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |      Gdk::GL::MODE_DEPTH  |     Gdk::GL::MODE_DOUBLE);
   if (glconfig==NULL){
      glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |    Gdk::GL::MODE_DEPTH);
      if(glconfig==NULL){
         cout << "Cannot intialise OpenGL. Exiting." << endl;
         std::exit(1);
      }
   }
   int bucketlimit = 100000;//How many points in each bucket, maximum.
   Quadtree* lidardata = NULL;//The flightlines are stored here.
   Gtk::Label *rulerlabelover = NULL;//Label displaying the distance along the ruler, in all dimensions etc. for the overview. Also other text output about the data and fences, profiles etc. is put here.
   refXml->get_widget("rulerlabelover",rulerlabelover);
   TwoDeeOverview *tdo = new TwoDeeOverview(glconfig,lidardata,bucketlimit,rulerlabelover);//The 2d overview.
   Gtk::Label *rulerlabel = NULL;//Label displaying the distance along the ruler, in all dimensions etc. for the profile.
   refXml->get_widget("rulerlabel",rulerlabel);
   Profile *prof = new Profile(glconfig,lidardata,bucketlimit,rulerlabel);//The profile.
   AdvancedOptionsWindow *aow = new AdvancedOptionsWindow(tdo,prof,refXml);//This contains the widgets of the advanced options window.
   FileSaver *fs = new FileSaver(tdo,prof,refXml,lidardata);//This contains the widgets of the file saver window.
   Gtk::EventBox *eventboxtdo = NULL;//Contains the overview. It is used to simulate the 2d overview getting focus without causing it to be redrawn every time.
   refXml->get_widget("eventboxtdo",eventboxtdo);
   Gtk::EventBox *eventboxprof = NULL;//Contains the profile. It is used to simulate the profile getting focus without causing it to be redrawn every time.
   refXml->get_widget("eventboxprof",eventboxprof);
   Gtk::Window *overviewwindow = NULL;
   refXml->get_widget("overviewwindow", overviewwindow);
   Gtk::Window *profilewindow = NULL;
   refXml->get_widget("profilewindow", profilewindow);
   ProfileWindow *profwin = new ProfileWindow(prof,tdo,profilewindow,eventboxprof,refXml,aow);//This contains the widgets of the profile window.
   TwoDeeOverviewWindow *tdow = new TwoDeeOverviewWindow(tdo,aow,fs,overviewwindow,profilewindow,refXml,eventboxtdo,profwin);//This contains the widgets of the 2D overview window.
   FileOpener *fo = new FileOpener(tdo,prof,refXml,aow,fs,lidardata,bucketlimit,eventboxtdo,eventboxprof,tdow);//This contains the widgets of the file opener window.
   fo->testfilename(argc,argv,true,false);//In case of command-line commands.
   gtkmain.run(*overviewwindow);
   delete tdow;
   delete profwin;
   delete fo;
   delete fs;
   delete aow;
   delete eventboxtdo;
   delete eventboxprof;
   delete rulerlabelover;
   delete overviewwindow;
   delete profilewindow;
   if(lidardata != NULL)delete lidardata;
   if(tdo!=NULL)delete tdo;
   if(prof != NULL)delete prof;
   return 0;
}
