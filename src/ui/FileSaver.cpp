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
#include <gtkglmm.h>
#include <vector>
#include "../TwoDeeOverview.h"
#include "../Profile.h"
#include "FileSaver.h"

FileSaver::FileSaver(TwoDeeOverview *tdo, Profile *prof, const Glib::RefPtr<Gtk::Builder>& builder)
:
		tdo			(tdo),
		prof		(prof),
		lidardata	(NULL)
{
  load_xml(builder);
  connect_signals();
  utmselect->set_active(true);
}

FileSaver::~FileSaver()
{
   delete flightlinelistlabel;
   delete flightlinesaveselect;
   delete parsestringentry;
   delete utmselect;
   delete latlongselect;
   delete scaleFactorEntryX;
   delete scaleFactorEntryY;
   delete scaleFactorEntryZ;
   delete btnUseDefault;
   //Have to delete parent after children?
   delete filesaverdialog;
}

void FileSaver::load_xml(const Glib::RefPtr<Gtk::Builder>& builder)
{
	builder->get_widget("filesaverdialog",filesaverdialog);
	builder->get_widget("flightlinelistlabel",flightlinelistlabel);
	builder->get_widget("flightlinesaveselect",flightlinesaveselect);
	builder->get_widget("parsestringentry", parsestringentry);
	builder->get_widget("scaleFactorEntryX1", scaleFactorEntryX);
	builder->get_widget("scaleFactorEntryY1", scaleFactorEntryY);
	builder->get_widget("scaleFactorEntryZ1", scaleFactorEntryZ);
	builder->get_widget("btnUseDefault1", btnUseDefault);
	builder->get_widget("utmselect", utmselect);
	builder->get_widget("latlongselect", latlongselect);

}

void FileSaver::connect_signals()
{
	filesaverdialog->signal_response().connect(sigc::mem_fun(*this,&FileSaver::on_filesaverdialogresponse));
	btnUseDefault->signal_toggled().connect(sigc::mem_fun(*this, &FileSaver::on_usedefault_changed));
	//flightlinesaveselect->signal_value_changed().connect(sigc::mem_fun(*this,&FileSaver::on_flightlinesaveselected));
}

void FileSaver::on_usedefault_changed()
{
	bool temp = !(btnUseDefault->get_active());

	scaleFactorEntryX->set_sensitive(temp);
	scaleFactorEntryY->set_sensitive(temp);
	scaleFactorEntryZ->set_sensitive(temp);
}

void FileSaver::on_filesaverdialogresponse(int response_id)
{
   if(response_id == Gtk::RESPONSE_CLOSE)
   {
      filesaverdialog->set_filename("");
      filesaverdialog->hide_all();
   }
   else if(response_id == 1)
   {
      if(lidardata==NULL)
    	  return;

      const char* filename = filesaverdialog->get_filename().c_str();
      LasSaver* saver = NULL;

      if (strstr(filename, ".txt") || strstr(filename, ".TXT"))
      {
          try
          {
        	 string parse_string = parsestringentry->get_text();

        	 if(btnUseDefault->get_active())
             {
            	 saver = new LasSaver(filename,lidardata->getFileName(flightlinesaveselect->get_value_as_int()).c_str(), parse_string.c_str(), latlongselect->get_active());
             }
             else
             {
            	 double scale_factor[3];
            	 const char* temp;
            	 temp = scaleFactorEntryX->get_text().c_str();
            	 scale_factor[0] = atof(temp);
            	 temp = scaleFactorEntryY->get_text().c_str();
            	 scale_factor[1] = atof(temp);
            	 temp = scaleFactorEntryZ->get_text().c_str();
            	 scale_factor[2] = atof(temp);

            	 saver = new LasSaver(filename, lidardata->getFileName(flightlinesaveselect->get_value_as_int()).c_str(), parse_string.c_str(), scale_factor, latlongselect->get_active());

                 temp = NULL;
             }

             lidardata->saveFlightLine(flightlinesaveselect->get_value_as_int(),saver);

             saver->close();
             delete saver;

             //filesaverdialog->hide_all();
          }
          catch(DescriptiveException e)
          {
             cout << "There has been an exception:" << endl;
             cout << "What: " << e.what() << endl;
             cout << "Why: " << e.why() << endl;
             return;
          }
      }
      else
      {
    	  try
    	  {
    		  saver = new LasSaver(filename,lidardata->getFileName(flightlinesaveselect->get_value_as_int()).c_str(), latlongselect->get_active());

    		  lidardata->saveFlightLine(flightlinesaveselect->get_value_as_int(),saver);

    		  saver->close();
    		  delete saver;
    		  filesaverdialog->unselect_all();

    		  //filesaverdialog->hide_all();
    	  }
    	  catch(DescriptiveException e)
    	  {
    		  cout << "There has been an exception:" << endl;
    		  cout << "What: " << e.what() << endl;
    		  cout << "Why: " << e.why() << endl;
    		  return;
    	  }
      }
   }
}


void FileSaver::on_flightlinesaveselected()
{
   if(lidardata!=NULL)
      filesaverdialog->set_filename(lidardata->getFileName(flightlinesaveselect->get_value_as_int()));
}

