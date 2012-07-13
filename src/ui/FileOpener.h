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
 * File: FileOpener.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: June-July 2010
 *
 * */

#ifndef _FILEOPENER_H_
#define _FILEOPENER_H_

#include <gtkmm.h>
#include <gtkglmm.h>
#include <vector>
#include "../TwoDeeOverview.h"
#include "../Profile.h"
#include "LasLoader.h"
#include "AdvancedOptionsWindow.h"
#include "FileSaver.h"
#include "TwoDeeOverviewWindow.h"
#include "../LoadWorker.h"
#include "AdvancedLoadDialog.h"

class FileOpener
{
public:
   FileOpener(TwoDeeOverview*,
              Profile*,
              const Glib::RefPtr<Gtk::Builder>&,
              AdvancedOptionsWindow*,
              FileSaver*,
              int,
              Gtk::EventBox*,
              Gtk::EventBox*,
              TwoDeeOverviewWindow*,
              AdvancedLoadDialog*);

   ~FileOpener();

   void show()
   {
      filechooserdialog->present();
   }

   void set_lidardata(Quadtree* lidardata)
   {
	   Glib::Mutex::Lock lock (mutex);
	   this->lidardata=lidardata;
   }
   Quadtree* get_lidardata()
   {
	   Glib::Mutex::Lock lock (mutex);
	   return this->lidardata;
   }
   void delete_lidardata()
   {
	   Glib::Mutex::Lock lock (mutex);
	   if (lidardata != NULL)
		   delete lidardata;
	   lidardata = NULL;
   }

   void set_newQuadtree(bool newqt)
   {
	   Glib::Mutex::Lock lock (mutex);
	   this->newQuadtree = newqt;
   }

   void set_loadedanyfiles(bool loaded)
   {
	   Glib::Mutex::Lock lock (mutex);
	   this->loadedanyfiles = loaded;
   }
   bool get_loadedanyfiles()
   {
	   Glib::Mutex::Lock lock (mutex);
	   return this->loadedanyfiles;
   }

   void set_thread_message(std::string message)
   {
	   // Glib::Mutex::Lock lock (mutex);
	   this->thread_message = message;
   }

   void set_minZ(double z)
   {
	   Glib::Mutex::Lock lock (mutex);
	   this->minZ = z;
   }
   double get_minZ()
   {
	   Glib::Mutex::Lock lock (mutex);
	   return this->minZ;
   }

   void set_maxZ(double z)
   {
	   Glib::Mutex::Lock lock (mutex);
   	   this->maxZ = z;
   }
   double get_maxZ()
   {
   	   Glib::Mutex::Lock lock (mutex);
   	   return this->maxZ;
   }

   void set_utm_zone(std::string zone)
   {
	   Glib::Mutex::Lock lock (mutex);
	   this->utm_zone = zone;
   }

   double minZ, maxZ;
   std::string utm_zone;

private:
   Quadtree *lidardata;
   int numlines;
   TwoDeeOverview *tdo;
   Profile* prof;
   TwoDeeOverviewWindow *tdow;
   AdvancedOptionsWindow *aow;
   FileSaver *fs;
   AdvancedLoadDialog* ald;

   //For opening files.
   Gtk::FileChooserDialog *filechooserdialog;

   //How many points to skip while loading one.
   Gtk::SpinButton *pointskipselect;

   // Check button determining whether the (overview) fence is used for 
   // loading flightlines.
   Gtk::CheckButton *fenceusecheck;

   //The type code for opening ASCII files.
   Gtk::Entry *asciicodeentry;

   // Scale factor entries and check box
   Gtk::Entry* scaleFactorEntryX;
   Gtk::Entry* scaleFactorEntryY;
   Gtk::Entry* scaleFactorEntryZ;
   Gtk::CheckButton* btnUseDefault;

   //The maximumum number of points to hold in cache.
   Gtk::SpinButton *cachesizeselect;

   //This displays the cache size in terms of gigabytes, approximately.
   Gtk::Label *cachesizeGBlabel;

   //This displays the errors and so on that occur in loading.
   Gtk::Label *loadoutputlabel;

   //Contains the overview.
   Gtk::EventBox *eventboxtdo;

   //Contains the profile.
   Gtk::EventBox *eventboxprof;
   Gtk::SpinButton *resbaseselect;
   Gtk::SpinButton *resdepthselect;

   Gtk::MenuItem* openfilemenuitem;
   Gtk::ToolButton* openbutton;

   //Stringstream getting error messages from the quadtree.
   ostringstream *loaderrorstream;

   Gtk::FileChooserButton* cache_folder_select;

   //How many points to hold in cache. 1 GB ~= 25000000 points.
   int cachelimit;

   //How many points in each bucket, maximum.
   int bucketlimit;

   // Advanced loading options
   Gtk::Dialog* loadadvanceddialog;
   Gtk::Button* loadadvancedbutton;
   Gtk::Button* loadadvancedcancel;

   // Threading
   LoadWorker* loadworker;
   Glib::Mutex mutex;

   // Loading dialog
   Gtk::Dialog* loaddialog;
   Gtk::Label* filelabel;
   Gtk::ProgressBar* fileprogressbar;
   Gtk::Label* totallabel;
   Gtk::ProgressBar* totalprogressbar;
   Gtk::Button* loadcancelbutton;
   int num_files_loading;

   // Members accessed from other thread (need thread-safe get/set methods)
   bool newQuadtree;
   std::string thread_message;

   //Whether or not any files have already been loaded in this session.
   bool loadedanyfiles;

   void load_xml(const Glib::RefPtr<Gtk::Builder>&);

   void connect_signals();

   void on_usedefault_changed();

   void on_filechooserdialogresponse(int response_id);
   void on_cachesize_changed();
   void on_resolutionbase_changed();

   //When selected from the menu, the file chooser opens.
   void on_openfilemenuactivated();

   //Clean up on quit.
   int on_quit();

   // Methods used called by another thread, through signals
   void show_thread_message();
   void add_line();
   void files_loaded();
   void load_failed();
   void on_progress();
   void on_loadcancelbutton_clicked();

   // Advanced dialog
   void on_loadadvancedbutton_clicked();
   void on_loadadvancedcancel_clicked();

};

#endif
