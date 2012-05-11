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
 * File: FileOpener.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: June-July 2010
 *
 * */
#include <gtkmm.h>
#include <gtkglmm.h>
#include <vector>
#include "../TwoDeeOverview.h"
#include "../Profile.h"
#include "../SelectionBox.h"
#include "FileOpener.h"

FileOpener::FileOpener(TwoDeeOverview *tdo, Profile *prof, const Glib::RefPtr<Gtk::Builder>& builder, AdvancedOptionsWindow *aow, FileSaver *fs,
           	   	   	   int bucketlimit, Gtk::EventBox *eventboxtdo, Gtk::EventBox *eventboxprof, TwoDeeOverviewWindow *tdow)
:
        lidardata		(NULL),
        tdo				(tdo),
        prof			(prof),
        tdow			(tdow),
        aow				(aow),
        fs				(fs),
        eventboxtdo		(eventboxtdo),
        eventboxprof	(eventboxprof),
        bucketlimit		(bucketlimit)
{
	loadworker = NULL;
	newQuadtree = true;

	loadedanyfiles = false;

	load_xml(builder);

	time_t starttime = time(NULL);

	char meh[80];
	strftime(meh, 80, "%Y.%m.%d(%j).%H-%M-%S.%Z", localtime(&starttime));
	ostringstream bleh;
	bleh << meh;

	loaderrorstream = new ostringstream();
	loadedanyfiles = false;
	cachelimit = 35000000;

	cachesizeselect->set_range(10e6,10e11);

	cachesizeselect->set_value(350e5);
	cachesizeselect->set_increments(10e5,10e5);

	resbaseselect->set_range(2,1000);
	resbaseselect->set_value(5);
	resbaseselect->set_increments(1,1);

	resdepthselect->set_range(1,10);
	resdepthselect->set_value(4);
	resdepthselect->set_increments(1,1);

	numlines = 0;

	connect_signals();

	on_cachesize_changed();
	on_usedefault_changed();
}


FileOpener::~FileOpener()
{
   delete loadoutputlabel;
   delete pointskipselect;
   delete fenceusecheck;
   delete asciicodeentry;
   delete cachesizeselect;
   delete cachesizeGBlabel;
   delete loaderrorstream;
   delete scaleFactorEntryX;
   delete scaleFactorEntryY;
   delete scaleFactorEntryZ;
   delete btnUseDefault;
   delete openbutton;
   //Have to delete parent after children?
   delete filechooserdialog;
}

void FileOpener::load_xml(const Glib::RefPtr<Gtk::Builder>& builder)
{
	builder->get_widget("openfilemenuitem",openfilemenuitem);
	builder->get_widget("filechooserdialog",filechooserdialog);
	builder->get_widget("pointskipselect",pointskipselect);
	builder->get_widget("fenceusecheck",fenceusecheck);
	builder->get_widget("asciicodeentry",asciicodeentry);
	builder->get_widget("cachesizeselect",cachesizeselect);
	builder->get_widget("scaleFactorEntryX", scaleFactorEntryX);
	builder->get_widget("scaleFactorEntryY", scaleFactorEntryY);
	builder->get_widget("scaleFactorEntryZ", scaleFactorEntryZ);
	builder->get_widget("btnUseDefault", btnUseDefault);
	builder->get_widget("cachesizeGBlabel",cachesizeGBlabel);
	builder->get_widget("loadoutputlabel",loadoutputlabel);
	builder->get_widget("resbaseselect",resbaseselect);
	builder->get_widget("resdepthselect",resdepthselect);
	builder->get_widget("openbutton",openbutton);
}

void FileOpener::connect_signals()
{
	openfilemenuitem->signal_activate().connect(sigc::mem_fun(*this,&FileOpener::on_openfilemenuactivated));
	openbutton->signal_clicked().connect(sigc::mem_fun(*this,&FileOpener::on_openfilemenuactivated));
	filechooserdialog->signal_response().connect(sigc::mem_fun(*this,&FileOpener::on_filechooserdialogresponse));
	btnUseDefault->signal_toggled().connect(sigc::mem_fun(*this,&FileOpener::on_usedefault_changed));
	cachesizeselect->signal_value_changed().connect(sigc::mem_fun(*this,&FileOpener::on_cachesize_changed));
	resbaseselect->signal_value_changed().connect(sigc::mem_fun(*this,&FileOpener::on_resolutionbase_changed));
    Gtk::Main::signal_quit().connect(sigc::mem_fun(*this, &FileOpener::on_quit));
}

int FileOpener::on_quit()
{
	Gtk::Window* profw = tdow->get_profilewindow();
	if (profw->get_realized())
		profw->hide_all();

	if (lidardata != NULL)
	{
		std::cout << "Cleaning up..." << std::endl;
		delete lidardata;
	}

	return 0;


}

void FileOpener::on_usedefault_changed()
{
	bool temp = !(btnUseDefault->get_active());

	scaleFactorEntryX->set_sensitive(temp);
	scaleFactorEntryY->set_sensitive(temp);
	scaleFactorEntryZ->set_sensitive(temp);
}


void FileOpener::on_resolutionbase_changed()
{
   //This is absolutely vital to prevent an overflow.
   resdepthselect->set_range(0,log(pow(2,sizeof(int)*8)/100)/log(resbaseselect->get_value_as_int()));
}

/*
======================================================================
 FileOpener::on_filechooserdialogresponse

 When Refresh or Add button is pressed this function takes arguments
 from FileOpener controls and creates a LoadWorker thread which loads
 selected files.
======================================================================
*/
void FileOpener::on_filechooserdialogresponse(int response_id)
{
   if(response_id == Gtk::RESPONSE_CLOSE)
   {
      filechooserdialog->hide_all();
   }
   else if(response_id == 1 || response_id == 2)
   {
	   // If the thread is already running - return
	   if (loadworker != NULL)
		   return;

	   if (response_id == 2 || !loadedanyfiles || lidardata==NULL)
		   numlines = 0;

	   // Get list of files from file chooser dialog
	   Glib::SListHandle<Glib::ustring> names=filechooserdialog->get_filenames();

	   // This seems to never change...
	   int bucketlevels = 0;

	   // Point offset
	   ostringstream pointoffset;
	   pointoffset << pointskipselect->get_value_as_int();
	   string poffs = pointoffset.str();

	   // Fence
	   SelectionBox fence = SelectionBox();
	   if (fenceusecheck->get_active())
	   {
		   if (tdo->get_realized())
			   fence = tdo->getFence();
	   }

	   // Scale factor
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
		   scale_factor[0] = 0;
		   scale_factor[1] = 0;
		   scale_factor[2] = 0;
	   }

	   // Create new quadtree if refreshing
	   bool create_new_quadtree;
	   if(response_id == 1)
	   {
		   create_new_quadtree = false;
	   }
	   if(response_id == 2)
	   {
		   create_new_quadtree = true;
	   }

	   // Launch LoadWorker thread
	   loadworker = new LoadWorker(this, pointskipselect->get_value_as_int(), names, create_new_quadtree, fenceusecheck->get_active(), resdepthselect->get_value_as_int(), resbaseselect->get_value_as_int(),
	  	  	  	  	  	  	  	  	  	  bucketlevels, bucketlimit, cachelimit, btnUseDefault->get_active(), scale_factor, asciicodeentry->get_text(), fence);

	   loadworker->sig_done.connect(sigc::mem_fun(*this, &FileOpener::files_loaded));
	   loadworker->sig_message.connect(sigc::mem_fun(*this, &FileOpener::show_thread_message));
	   loadworker->sig_file_loaded.connect(sigc::mem_fun(*this, &FileOpener::add_line));

	   loadworker->start();

	   loadoutputlabel->set_text("Loading files...\n");

	   // Change cursor to busy
	   GdkDisplay* display;
	   GdkCursor* cursor;
	   GdkWindow* window;

	   cursor = gdk_cursor_new(GDK_WATCH);
	   display = gdk_display_get_default();
	   window = (GdkWindow*) filechooserdialog->get_window()->gobj();

	   gdk_window_set_cursor(window, cursor);
	   gdk_display_sync(display);
	   gdk_cursor_unref(cursor);
   }
}

void FileOpener::show_thread_message()
{
	{
		Glib::Mutex::Lock lock (mutex);
		loadoutputlabel->set_text(loadoutputlabel->get_text() + thread_message + "\n");
	}

	Gdk::Window::process_all_updates();
}

void FileOpener::add_line()
{
	++numlines;
}

void FileOpener::files_loaded()
{
	delete loadworker;
	loadworker = NULL;

	loadoutputlabel->set_text(loadoutputlabel->get_text() + "Done." + "\n");
	Gdk::Window::process_all_updates();

	//Provide the drawing objects access to the quadtree:
	tdo->setlidardata(lidardata,bucketlimit);
	prof->setlidardata(lidardata,bucketlimit);

	if(newQuadtree)
	{
		tdo->setresolutionbase(resbaseselect->get_value_as_int());
	    tdo->setresolutiondepth(resdepthselect->get_value_as_int());
	    // This is absolutely vital to prevent an overflow in the drawing thread
	    // in the overview that then causes an infinite loop.
	    aow->setmaindetailrange(0,log(pow(2,sizeof(int)*8)/100) / log(resbaseselect->get_value_as_int()));
	}

	// If drawing areas are already visible, prepare the new images and draw
	// them.
	if(loadedanyfiles)
	{
		tdo->prepare_image();
		tdo->drawviewable(1);
		prof->prepare_image();
		prof->drawviewable(1);
	}

	// Otherwise, pack them into the vboxes and then show them, which will do
	// as the above block does.
	else
	{
		eventboxtdo->add(*tdo);
		tdo->show_all();
		eventboxprof->add(*prof);
		prof->show_all();
	}

	loadedanyfiles = true;
	string flightline,list="";

	for(int i = 0;i<numlines;i++)
	{
		flightline = lidardata->getFileName(i);
		ostringstream number;
		number << i;
		list += number.str() + ":  " + flightline + "\n";
	}

	fs->setlidardata(lidardata);
	fs->setlabeltext(list);
	fs->setlinerange(0,numlines-1);
	tdow->setraiselinerange(0,numlines-1);

	tdow->set_slice_range(minZ, maxZ);
	tdow->set_utm_zone(utm_zone);

	// (Re)Set the advanced colouring and shading options to the values
	// indicated by the recently loaded flightlines.
	aow->resetcolouringandshading();

	// Set cursor back to normal
	GdkDisplay* display;
	GdkCursor* cursor;
	GdkWindow* window;

	cursor = gdk_cursor_new(GDK_LEFT_PTR);
	display = gdk_display_get_default();
	window = (GdkWindow*) filechooserdialog->get_window()->gobj();

	gdk_window_set_cursor(window, cursor);
	gdk_display_sync(display);
	gdk_cursor_unref(cursor);
}

// When the cachesize (in points) is changed, this outputs the value in 
// Gigabytes (NOT Gibibytes) to a label next to it.
void FileOpener::on_cachesize_changed()
{
   ostringstream GB;
   GB << ((double)cachesizeselect->get_value()*sizeof(LidarPoint))/1073741824;
   string labelstring = "Approximately: " + GB.str() + " GB.";
   cachesizeGBlabel->set_text(labelstring);
}

//When selected from the menu, the file chooser opens.
void FileOpener::on_openfilemenuactivated()
{
   show(); 
}
