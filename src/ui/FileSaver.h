/*
 ===============================================================================

 FileSaver.h

 Created on: June-July 2010
 Authors: Haraldur Tristan Gunnarsson, Jan Holownia

 LIDAR Analysis GUI (LAG), viewer for LIDAR files in .LAS or ASCII format
 Copyright (C) 2009-2012 Plymouth Marine Laboratory (PML)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 ===============================================================================
 */

#ifndef FILESAVER_H
#define FILESAVER_H

#include <gtkmm.h>
#include <vector>
#include "../TwoDeeOverview.h"
#include "../Profile.h"
#include "../SaveWorker.h"

/*
 ===============================================================================

 FileSaver - a file chooser dialog for saving files.

 ===============================================================================
 */
class FileSaver
{
   public:
      FileSaver(TwoDeeOverview*, Profile*, const Glib::RefPtr<Gtk::Builder>&);

      ~FileSaver();

      void show()
      {
         filesaverdialog->present();
      }
      
      void setlinerange(int first, int second)
      {
         flightlinesaveselect->set_range(first, second);
      }
      
      void setlabeltext(string text)
      {
         flightlinelistlabel->set_text(text);
      }
      
      void on_flightlinesaveselected();

      void setlidardata(Quadtree *lidardata)
      {
         this->lidardata = lidardata;
      }
      
      // FIXME (make private and add thread safe get/set methods)
      Quadtree *lidardata;

   private:
      
      TwoDeeOverview *tdo;
      Profile *prof;

      //For opening files.
      Gtk::FileChooserDialog *filesaverdialog;

      Gtk::Label *flightlinelistlabel;
      Gtk::SpinButton *flightlinesaveselect;

      Gtk::RadioButton* utmselect;
      Gtk::RadioButton* latlongselect;

      Gtk::Entry* parsestringentry;
      Gtk::Entry* scaleFactorEntryX;
      Gtk::Entry* scaleFactorEntryY;
      Gtk::Entry* scaleFactorEntryZ;
      Gtk::CheckButton* btnUseDefault;

      // Threading
      SaveWorker* saveworker;

      // Saving dialog
      Gtk::Dialog* savedialog;
      Gtk::ProgressBar* saveprogressbar;
      Gtk::Button* savecancelbutton;

      // Waveform
      Gtk::Dialog* waveformdialog;
      Gtk::ProgressBar* waveformprogressbar;
      Gtk::Button* waveformcancelbutton;

      void load_xml(const Glib::RefPtr<Gtk::Builder>&);
      void connect_signals();
      void on_filesaverdialogresponse(int response_id);
      void on_usedefault_changed();
      void on_progress();
      void files_saved();
      void on_savecancelbutton_clicked();
      void waveform_started();
      void on_waveform_progress();
};

#endif
