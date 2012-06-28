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
		lidardata	(NULL),
		tdo			(tdo),
		prof		(prof),
		saveworker	(NULL)

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
   delete savedialog;
   delete saveprogressbar;
   delete savecancelbutton;
   delete waveformdialog;
   delete waveformprogressbar;
   delete waveformcancelbutton;
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
	builder->get_widget("savedialog", savedialog);
	builder->get_widget("saveprogressbar", saveprogressbar);
	builder->get_widget("savecancelbutton", savecancelbutton);
	builder->get_widget("waveformdialog", waveformdialog);
	builder->get_widget("waveformprogressbar", waveformprogressbar);
	builder->get_widget("waveformcancelbutton", waveformcancelbutton);
}

void FileSaver::connect_signals()
{
	filesaverdialog->signal_response().connect(sigc::mem_fun(*this,&FileSaver::on_filesaverdialogresponse));
	btnUseDefault->signal_toggled().connect(sigc::mem_fun(*this, &FileSaver::on_usedefault_changed));
	//flightlinesaveselect->signal_value_changed().connect(sigc::mem_fun(*this,&FileSaver::on_flightlinesaveselected));
	savecancelbutton->signal_clicked().connect(sigc::mem_fun(*this,&FileSaver::on_savecancelbutton_clicked));
	waveformcancelbutton->signal_clicked().connect(sigc::mem_fun(*this,&FileSaver::on_savecancelbutton_clicked));
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
      //filesaverdialog->set_filename("");
      filesaverdialog->hide_all();
   }
   else if(response_id == 1)
   {
      if (lidardata==NULL)
    	  return;

      if (saveworker != NULL)
    	  return;

      double scale_factor[3];
      if (!btnUseDefault->get_active())
      {
    	  const char* temp;
    	  temp = scaleFactorEntryX->get_text().c_str();
    	  scale_factor[0] = atof(temp);
    	  temp = scaleFactorEntryY->get_text().c_str();
    	  scale_factor[1] = atof(temp);
    	  temp = scaleFactorEntryZ->get_text().c_str();
    	  scale_factor[2] = atof(temp);
      }
      else
      {
    	  scale_factor[0] = scale_factor[1] = scale_factor[2] = 0;
      }

      saveworker = new SaveWorker(this, filesaverdialog->get_filename(), lidardata->getFileName(flightlinesaveselect->get_value_as_int()), flightlinesaveselect->get_value_as_int(), parsestringentry->get_text(), latlongselect->get_active(), btnUseDefault->get_active(), scale_factor);
      saveworker->start();
      saveworker->sig_done.connect(sigc::mem_fun(*this, &FileSaver::files_saved));
      saveworker->sig_progress.connect(sigc::mem_fun(*this, &FileSaver::on_progress));
      saveworker->sig_waveform.connect(sigc::mem_fun(*this, &FileSaver::waveform_started));
      saveworker->sig_waveform_progress.connect(sigc::mem_fun(*this, &FileSaver::on_waveform_progress));

      // Show saving dialog
      savedialog->show_all();
      saveprogressbar->set_fraction(0);

	  // Change cursor to busy
	  GdkDisplay* display;
	  GdkCursor* cursor;
	  GdkWindow* window;

	  cursor = gdk_cursor_new(GDK_WATCH);
	  display = gdk_display_get_default();
	  window = (GdkWindow*) filesaverdialog->get_window()->gobj();

	  gdk_window_set_cursor(window, cursor);
	  gdk_display_sync(display);
	  gdk_cursor_unref(cursor);
   }
}

void FileSaver::on_progress()
{
	saveprogressbar->set_fraction((saveprogressbar->get_fraction() + 0.01) > 1 ? 1 : (saveprogressbar->get_fraction() + 0.01));
}

void FileSaver::waveform_started()
{
	waveformdialog->show_all();
	waveformprogressbar->set_fraction(0);
}

void FileSaver::on_waveform_progress()
{
	waveformprogressbar->set_fraction((waveformprogressbar->get_fraction() + 0.01) > 1 ? 1 : (waveformprogressbar->get_fraction() + 0.01));
}

void FileSaver::files_saved()
{
	delete saveworker;
	saveworker = NULL;

	savedialog->hide_all();

	if (waveformdialog->get_realized())
		waveformdialog->hide_all();

	// Set cursor back to normal
	GdkDisplay* display;
	GdkCursor* cursor;
	GdkWindow* window;

	cursor = gdk_cursor_new(GDK_LEFT_PTR);
	display = gdk_display_get_default();
	window = (GdkWindow*) filesaverdialog->get_window()->gobj();

	gdk_window_set_cursor(window, cursor);
	gdk_display_sync(display);
	gdk_cursor_unref(cursor);
}

void FileSaver::on_savecancelbutton_clicked()
{
	if (saveworker != NULL)
		saveworker->stop();
}


void FileSaver::on_flightlinesaveselected()
{
   if(lidardata!=NULL)
      filesaverdialog->set_filename(lidardata->getFileName(flightlinesaveselect->get_value_as_int()));
}

