/*
 * LIDAR Analysis GUI (LAG), viewer for LIDAR files in .LAS or ASCII format
 * Copyright (C) 2009-2010 Plymouth Marine Laboratory (PML)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
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

// Takes the path the executable was called with and uses it to find the 
// .glade file needed to make the GUI.
string findgladepath(char* programpath){
   //The path of the executable.
   string exename = programpath;
   //Find the last forward slash and make its index our index.
   unsigned int index = exename.rfind("/");
   // I.e. in the event that there is no forward slash (so must be calling 
   // from the same directory), just go from 0, where the forward slash would 
   // have been.
   if(index==string::npos)
      index=0;
   //We do not actually want to include the forward slash.
   else 
      index++;
   string gladename = exename;
   gladename.replace(index,9,"lag.glade");
   cout << exename << endl;
   cout << gladename << endl;
   return gladename;
}

int main(int argc, char** argv) {
   cout << "Build number: 2010.07.29.1" << endl;
   //This allows the creation and running of threads.
   Glib::thread_init();
   // This is required for GTK to work. It must be the first GTK object 
   // created and may not be global.
   Gtk::Main gtkmain(argc, argv);
   //This will extract widgets from the glade file when directed.
   Glib::RefPtr<Gnome::Glade::Xml> refXml;
   try{
      //Assign the glade file used to build the GUI.
      refXml = Gnome::Glade::Xml::create(findgladepath(argv[0]));
   }
   catch(const Gnome::Glade::XmlError& ex){ 
      cerr << ex.what() << std::endl;
      cerr << "The file lag.glade must be located in the same directory as \
               the lag executable." << endl;
      return 1;
   }
   Gtk::GL::init(argc, argv);
   Glib::RefPtr<Gdk::GL::Config> glconfig;
   glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                      Gdk::GL::MODE_DEPTH |
                                      Gdk::GL::MODE_DOUBLE);
   if (glconfig==NULL){
      glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                         Gdk::GL::MODE_DEPTH);
      if(glconfig==NULL){
         cout << "Cannot intialise OpenGL. Exiting." << endl;
         std::exit(1);
      }
   }

   //How many points in each bucket, maximum.
   int bucketlimit = 100000;
   //The flightlines are stored here.
   Quadtree* lidardata = NULL;
   // Label displaying the distance along the ruler, in all dimensions etc. 
   // for the overview. Also other text output about the data and fences,
   // profiles etc. is put here.
   Gtk::Label *rulerlabelover = NULL;
   refXml->get_widget("rulerlabelover",rulerlabelover);
   //The 2d overview.
   TwoDeeOverview *tdo = new TwoDeeOverview(glconfig, lidardata, 
                                            bucketlimit, rulerlabelover);
   // Label displaying the distance along the ruler, in all dimensions 
   // etc. for the profile.
   Gtk::Label *rulerlabel = NULL;
   refXml->get_widget("rulerlabel",rulerlabel);

   //The profile.
   Profile *prof = new Profile(glconfig, lidardata, 
                               bucketlimit,rulerlabel);
   //This contains the widgets of the advanced options window.
   AdvancedOptionsWindow *aow = new AdvancedOptionsWindow(tdo,prof,refXml);
   //This contains the widgets of the file saver window.
   FileSaver *fs = new FileSaver(tdo,prof,refXml,lidardata);
   // Contains the overview. It is used to simulate the 2d overview getting 
   // focus without causing it to be redrawn every time.
   Gtk::EventBox *eventboxtdo = NULL;
   refXml->get_widget("eventboxtdo",eventboxtdo);
   // Contains the profile. It is used to simulate the profile getting focus 
   // without causing it to be redrawn every time.
   Gtk::EventBox *eventboxprof = NULL;
   refXml->get_widget("eventboxprof",eventboxprof);
   Gtk::Window *overviewwindow = NULL;
   refXml->get_widget("overviewwindow", overviewwindow);
   Gtk::Window *profilewindow = NULL;
   refXml->get_widget("profilewindow", profilewindow);
   //This contains the widgets of the profile window.
   ProfileWindow *profwin = new ProfileWindow(prof, tdo, profilewindow, 
                                              overviewwindow, eventboxprof,
                                              refXml, aow);

   //This contains the widgets of the 2D overview window.
   TwoDeeOverviewWindow *tdow = new TwoDeeOverviewWindow(tdo, aow, fs, 
                                                         overviewwindow,
                                                         profilewindow, refXml, 
                                                         eventboxtdo,profwin);
   //This contains the widgets of the file opener window.
   FileOpener *fo = new FileOpener(tdo, prof, refXml, aow, fs, lidardata, 
                                   bucketlimit,eventboxtdo,eventboxprof,tdow);
   
   //In case of command-line commands 
   fo->testfilename(argc,argv,true,false);
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
