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
//#include "../Quadtree.h"
#include "LasLoader.h"
#include "AsciiLoader.h"
#include "AdvancedOptionsWindow.h"
#include "FileSaver.h"
#include "TwoDeeOverviewWindow.h"
class FileOpener{
public:
   FileOpener(TwoDeeOverview*,
              Profile*,
              Glib::RefPtr<Gtk::Builder>,
              AdvancedOptionsWindow*,
              FileSaver*,
              Quadtree*,
              int,
              Gtk::EventBox*,
              Gtk::EventBox*,
              TwoDeeOverviewWindow*);

   ~FileOpener();

   int testfilename(int argc,char *argv[],bool start,bool usearea);

   void show(){ 
      filechooserdialog->present();
   }

protected:
   int numlines;
   TwoDeeOverview *tdo;
   Profile* prof;
   TwoDeeOverviewWindow *tdow;
   AdvancedOptionsWindow *aow;
   FileSaver *fs;
   Quadtree *lidardata;
   //For opening files.
   Gtk::FileChooserDialog *filechooserdialog;
   //How many points to skip while loading one.
   Gtk::SpinButton *pointskipselect;
   // Check button determining whether the (overview) fence is used for 
   // loading flightlines.
   Gtk::CheckButton *fenceusecheck;
   //The type code for opening ASCII files.
   Gtk::Entry *asciicodeentry;
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
   //Path of file for error message output.
   string loaderroroutputfile;
   //Stream outputting error messages from the quadtree to a file.
   ofstream loaderroroutput;
   //Stringstream getting error messages from the quadtree.
   ostringstream *loaderrorstream;
   //Whether or not any files have already been loaded in this session.
   bool loadedanyfiles;
   //How many points to hold in cache. 1 GB ~= 25000000 points.
   int cachelimit;
   //How many points in each bucket, maximum.
   int bucketlimit;

   void on_filechooserdialogresponse(int response_id);
   void on_cachesize_changed();
   void on_resolutionbase_changed();
   
   //When selected from the menu, the file chooser opens.
   void on_openfilemenuactivated();

};

#endif
