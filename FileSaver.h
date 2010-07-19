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
 * File: FileSaver.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: June-July 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <vector>
#include "Quadtree.h"
#include "TwoDeeOverview.h"
#include "Profile.h"
#include "LasSaver.h"
#ifndef FILESAVER_H
#define FILESAVER_H
class FileSaver{
public:
   FileSaver(TwoDeeOverview *tdo,Profile *prof,Glib::RefPtr<Gnome::Glade::Xml> refXml,Quadtree *lidardata);
   ~FileSaver();
   void show(){ filesaverdialog->present(); }
   void setlinerange(int first, int second){ flightlinesaveselect->set_range(first,second); }
   void setlabeltext(string text){ flightlinelistlabel->set_text(text); }
   void on_flightlinesaveselected();
   void setlidardata(Quadtree *lidardata){ this->lidardata = lidardata; }
protected:
   TwoDeeOverview *tdo;
   Profile *prof;
   Gtk::FileChooserDialog *filesaverdialog;//For opening files.
   Gtk::Label *flightlinelistlabel;//This displays the cache size in terms of gigabytes, approximately.
   Gtk::SpinButton *flightlinesaveselect;
   Quadtree *lidardata;
   void on_filesaverdialogresponse(int response_id);
};

#endif
