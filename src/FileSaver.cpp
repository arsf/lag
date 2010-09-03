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
 * File: FileSaver.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: June-July 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <vector>
#include "TwoDeeOverview.h"
#include "Profile.h"
#include "FileSaver.h"

FileSaver::
FileSaver(TwoDeeOverview *tdo,
          Profile *prof,
          Glib::RefPtr<Gnome::Glade::Xml> refXml,
          Quadtree *lidardata){
   this->tdo = tdo;
   this->prof = prof;
   this->lidardata = lidardata;
   refXml->get_widget("filesaverdialog",filesaverdialog);
   if(filesaverdialog)
      filesaverdialog->signal_response().
         connect(sigc::mem_fun(*this,&FileSaver::
                 on_filesaverdialogresponse));
   refXml->get_widget("flightlinelistlabel",flightlinelistlabel);
   refXml->get_widget("flightlinesaveselect",flightlinesaveselect);

   if(flightlinesaveselect)
      flightlinesaveselect->signal_value_changed().
         connect(sigc::mem_fun(*this,&FileSaver::
                 on_flightlinesaveselected));
}
FileSaver::~FileSaver(){
   delete flightlinelistlabel;
   delete flightlinesaveselect;
   //Have to delete parent after children?
   delete filesaverdialog;
}

void FileSaver::on_filesaverdialogresponse(int response_id){
   if(response_id == Gtk::RESPONSE_CLOSE)filesaverdialog->hide_all();
   else if(response_id == 1){
      if(lidardata==NULL)return;
      try{
         LasSaver *saver = new LasSaver(filesaverdialog->get_filename().c_str(),
                                        lidardata->getFileName(
                                        flightlinesaveselect->
                                        get_value_as_int()).c_str());

         lidardata->saveFlightLine(flightlinesaveselect->get_value_as_int(),
                                   saver);
         delete saver;
      }
      catch(DescriptiveException e){
         cout << "There has been an exception:" << endl;
         cout << "What: " << e.what() << endl;
         cout << "Why: " << e.why() << endl;
         return;
      }
   }
}
void FileSaver::on_flightlinesaveselected(){
   if(lidardata!=NULL)
      filesaverdialog->set_filename(lidardata->getFileName(
                                    flightlinesaveselect->get_value_as_int()));
}
