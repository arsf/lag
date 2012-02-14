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
#include <gtkglmm.h>
#include "Quadtree.h"
#include "TwoDeeOverview.h"
#include "Profile.h"
#include "ui/TwoDeeOverviewWindow.h"
#include "ui/ProfileWindow.h"
#include "ui/FileOpener.h"
using namespace std;

bool glade_exists(const char *filename)
{
  ifstream ifile(filename);
  return ifile;
}

// Takes the path the executable was called with and uses it to find the 
// .glade file needed to make the GUI.
string findgladepath(char* programpath)
{
   //The path of the executable.
   string exename = programpath;

   //Find the last forward slash and make its index our index.
   unsigned int index = exename.rfind("/");

   // I.e. in the event that there is no forward slash (so must be calling 
   // from the same directory), just go from 0, where the forward slash would 
   // have been.
   //if(index==string::npos)
   //   index=0;
   //We do not actually want to include the forward slash.
   //else 
   index++;
   string gladename1 = exename;
   string gladename2 = exename;
   
   gladename1.replace(index,9,"../lag.ui");
   gladename2.replace(index,9,"lag.ui");
   
   cout << exename << endl;
   if(glade_exists(gladename1.c_str()))
   {
      cout << gladename1 << endl;
      return gladename1;
   }
   else if(glade_exists(gladename2.c_str()))
   {
      cout << gladename2 << endl;
      return gladename2;
   }
   else
   {
      std::cerr << "No lag.ui glade file found" << std::endl;
      exit(1);
      return "BANG!";
   }
}

int main(int argc, char** argv)
{
   cout << "Build number: 2012.1.33.7" << endl;

   //This allows the creation and running of threads.
   Glib::thread_init();

   // This is required for GTK to work. It must be the first GTK object 
   // created and may not be global.
   Gtk::Main gtkmain(argc, argv);

   //This will extract widgets from the glade file when directed.
   Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
   try
   {
	   builder->add_from_file(findgladepath(argv[0]));
   }
   catch(const Glib::FileError& ex)
   {
	   std::cerr << "FileError: " << ex.what() << std::endl;
   }
   catch(const Glib::MarkupError& ex)
   {
	   std::cerr << "MarkupError: " << ex.what() << std::endl;
   }
   catch(const Gtk::BuilderError& ex)
   {
	   std::cerr << "BuilderError: " << ex.what() << std::endl;
   }

   Gtk::GL::init(argc, argv);
   Glib::RefPtr<Gdk::GL::Config> glconfig;

   glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB | Gdk::GL::MODE_DEPTH | Gdk::GL::MODE_DOUBLE);
   if (glconfig==0)
   {
      glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB | Gdk::GL::MODE_DEPTH);
      if(glconfig==0)
      {
         cout << "Cannot intialise OpenGL. Exiting." << endl;
         std::exit(1);
      }
   }

   printf("%s\n", argv[0]);

   //How many points in each bucket, maximum.
   int bucketlimit = 100000;

   //The flightlines are stored here.
   Quadtree* lidardata = NULL;

   // Label displaying the distance along the ruler, in all dimensions etc. 
   // for the overview. Also other text output about the data and fences,
   // profiles etc. is put here.
   Gtk::Label *rulerlabelover = NULL;
   builder->get_widget("rulerlabelover",rulerlabelover);

   //The 2d overview.
   TwoDeeOverview *tdo = new TwoDeeOverview(argv[0], glconfig, lidardata,bucketlimit, rulerlabelover);

   // Label displaying the distance along the ruler, in all dimensions 
   // etc. for the profile.
   Gtk::Label *rulerlabel = NULL;
   builder->get_widget("rulerlabel",rulerlabel);

   //The profile.
   Profile *prof = new Profile(argv[0], glconfig, lidardata, bucketlimit,rulerlabel);

   //This contains the widgets of the advanced options window.
   AdvancedOptionsWindow *aow = new AdvancedOptionsWindow(tdo,prof,builder);

   //This contains the widgets of the file saver window.
   FileSaver *fs = new FileSaver(tdo,prof,builder,lidardata);

   // Contains the overview. It is used to simulate the 2d overview getting 
   // focus without causing it to be redrawn every time.
   Gtk::EventBox *eventboxtdo = NULL;
   builder->get_widget("eventboxtdo",eventboxtdo);

   // Contains the profile. It is used to simulate the profile getting focus 
   // without causing it to be redrawn every time.
   Gtk::EventBox *eventboxprof = NULL;
   builder->get_widget("eventboxprof",eventboxprof);
   Gtk::Window *overviewwindow = NULL;
   builder->get_widget("overviewwindow", overviewwindow);
   Gtk::Window *profilewindow = NULL;
   builder->get_widget("profilewindow", profilewindow);
   

   //This contains the widgets of the profile window.
   ProfileWindow *profwin = new ProfileWindow(prof, tdo, profilewindow, 
                                              overviewwindow, eventboxprof,
                                              builder, aow);

   //This contains the widgets of the 2D overview window.
   TwoDeeOverviewWindow *tdow = new TwoDeeOverviewWindow(tdo, aow, fs, 
                                                         overviewwindow,
                                                         profilewindow, builder, 
                                                         eventboxtdo,profwin);

   //This contains the widgets of the file opener window.
   FileOpener *fo = new FileOpener(tdo, prof, builder, aow, fs, lidardata, 
                                   bucketlimit,eventboxtdo,eventboxprof,tdow);
   
   //In case of command-line commands 
   fo->testfilename(argc,argv,true,false);
   
   gdk_threads_enter();

   gtkmain.run(*overviewwindow);

   gdk_threads_leave();

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
   if(tdo!=NULL)delete tdo;
   if(prof != NULL)delete prof;
   if(lidardata != NULL)delete lidardata;
   return 0;
}
