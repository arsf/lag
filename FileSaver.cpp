/*
 * File: FileSaver.cpp
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
#include "FileSaver.h"

FileSaver::FileSaver(TwoDeeOverview *tdo,Profile *prof,Glib::RefPtr<Gnome::Glade::Xml> refXml,quadtree *lidardata){
   this->tdo = tdo;
   this->prof = prof;
   this->lidardata = lidardata;
   refXml->get_widget("filesaverdialog",filesaverdialog);
   if(filesaverdialog)filesaverdialog->signal_response().connect(sigc::mem_fun(*this,&FileSaver::on_filesaverdialogresponse));
   refXml->get_widget("flightlinelistlabel",flightlinelistlabel);
   refXml->get_widget("flightlinesaveselect",flightlinesaveselect);
   if(flightlinesaveselect)flightlinesaveselect->signal_value_changed().connect(sigc::mem_fun(*this,&FileSaver::on_flightlinesaveselected));
}
FileSaver::~FileSaver(){
   delete flightlinelistlabel;
   delete flightlinesaveselect;
   delete filesaverdialog;//Have to delete parent after children?
}

void FileSaver::on_filesaverdialogresponse(int response_id){
   if(response_id == Gtk::RESPONSE_CLOSE)filesaverdialog->hide_all();
   else if(response_id == 1){
      if(lidardata==NULL)return;
      try{
         LASsaver *saver = new LASsaver(filesaverdialog->get_filename().c_str(),lidardata->getfilename(flightlinesaveselect->get_value_as_int()).c_str());
         lidardata->saveflightline(flightlinesaveselect->get_value_as_int(),saver);
         delete saver;
      }
      catch(descriptiveexception e){
         cout << "There has been an exception:" << endl;
         cout << "What: " << e.what() << endl;
         cout << "Why: " << e.why() << endl;
         return;
      }
   }
}
void FileSaver::on_flightlinesaveselected(){
   if(lidardata!=NULL)filesaverdialog->set_filename(lidardata->getfilename(flightlinesaveselect->get_value_as_int()));
}
