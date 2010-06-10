/*
 * File: FileOpener.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: June 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <vector>
#include "TwoDeeOverview.h"
#include "Profile.h"
#include "quadtree.h"
#include "LASloader.h"
#include "ASCIIloader.h"
#include "AdvancedOptionsWindow.h"
#include "FileSaver.h"
class FileOpener{
public:
   FileOpener(TwoDeeOverview *tdo,Profile *prof,Glib::RefPtr<Gnome::Glade::Xml> refXml,AdvancedOptionsWindow *aow,FileSaver *fs,quadtree *lidardata,int bucketlimit,Gtk::EventBox *eventboxtdo,Gtk::EventBox *eventboxprof);
   ~FileOpener();
   int testfilename(int argc,char *argv[],bool start,bool usearea);
   void show(){ filechooserdialog->show_all();}
protected:
   TwoDeeOverview *tdo;
   Profile* prof;
   AdvancedOptionsWindow *aow;
   FileSaver *fs;
   quadtree *lidardata;
   Gtk::FileChooserDialog *filechooserdialog;//For opening files.
   Gtk::SpinButton *pointskipselect;//How many points to skip while loading one.
   Gtk::CheckButton *fenceusecheck;//Check button determining whether the (overview) fence is used for loading flightlines.
   Gtk::Entry *asciicodeentry;//The type code for opening ASCII files.
   Gtk::SpinButton *cachesizeselect;//The maximumum number of points to hold in cache.
   Gtk::Label *cachesizeGBlabel;//This displays the cache size in terms of gigabytes, approximately.
   Gtk::EventBox *eventboxtdo;//Contains the overview.
   Gtk::EventBox *eventboxprof;//Contains the profile.
   string loaderroroutputfile;//Path of file for error message output.
   ofstream loaderroroutput;//Stream outputting error messages from the quadtree to a file.
   ostringstream *loaderrorstream;//Stringstream getting error messages from the quadtree.
   bool loadedanyfiles;//Whether or not any files have already been loaded in this session.
   int cachelimit;//How many points to hold in cache. 1 GB ~= 25000000 points.
   int bucketlimit;//How many points in each bucket, maximum.
   void on_filechooserdialogresponse(int response_id);
   void on_cachesize_changed();
};
