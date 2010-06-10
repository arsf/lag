/*
 * File: FileSaver.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: June 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <vector>
#include "quadtree.h"
#include "TwoDeeOverview.h"
#include "Profile.h"
#include "LASsaver.h"
#ifndef FILESAVER_H
#define FILESAVER_H
class FileSaver{
public:
   FileSaver(TwoDeeOverview *tdo,Profile *prof,Glib::RefPtr<Gnome::Glade::Xml> refXml,quadtree *lidardata);
   ~FileSaver();
   void show(){ filesaverdialog->show_all(); }
   void setlinerange(int first, int second){ flightlinesaveselect->set_range(first,second); }
   void setlabeltext(string text){ flightlinelistlabel->set_text(text); }
   void on_flightlinesaveselected();
   void setlidardata(quadtree *lidardata){ this->lidardata = lidardata; }
protected:
   TwoDeeOverview *tdo;
   Profile *prof;
   Gtk::FileChooserDialog *filesaverdialog;//For opening files.
   Gtk::Label *flightlinelistlabel;//This displays the cache size in terms of gigabytes, approximately.
   Gtk::SpinButton *flightlinesaveselect;
   quadtree *lidardata;
   void on_filesaverdialogresponse(int response_id);
};

#endif
