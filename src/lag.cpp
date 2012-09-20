/*
==================================

 lag.cpp

 Written: November 2009 - July 2012
 Authors: Haraldur Tristan Gunnarsson, Jan Holownia, Berin Smaldon

 LIDAR Analysis GUI (LAG), viewer for LIDAR files in .LAS or ASCII format
 Copyright (C) 2009-2010 Plymouth Marine Laboratory (PML)

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

==================================
*/

#include <iostream>
#include <gtkmm.h>
#include <gtkglmm.h>
#include <sys/stat.h>

#include "Quadtree.h"
#include "TwoDeeOverview.h"
#include "Profile.h"
#include "ui/TwoDeeOverviewWindow.h"
#include "ui/ProfileWindow.h"
#include "ui/FileOpener.h"
#include "ui/AdvancedLoadDialog.h"

#ifdef __WIN32
#include <winbase.h>
#endif

using namespace std;

/*
==================================
 glade_exists

 Returns true if a file exists.
==================================
*/
bool glade_exists(string const& filename)
{
#ifndef __WIN32
   struct stat sb;

   if (stat(filename.c_str(), &sb) == 0)
      return true;
   else
      return false;
#else
   return false; // fix me
#endif
}

/*
==================================
 findgladepath

 Takes the path the executable was called with and uses it to find the
 .glade file needed to make the GUI.
==================================
*/
string findgladepath(char* programpath)
{
	// For lag installed in /usr/local/bin, /usr/local/share/lag
	// EDIT: Now supporting similar windows layouts
	//       i.e., C:\Program Files\PML\Lag\bin, now produces something
	//             like C:\Program Files\PML\Lag\share\lag\lag.ui

	// Find the path of the executable
	char buff[1024];
	ssize_t len;
#ifdef __WIN32
	// windows only
	len = GetModuleFileName(NULL, buff, sizeof(buff)-1);
#else
	// linux only
	len = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
#endif
	if (len > 0) // len = -1 for linux failure, 0 for windows failure
	{
		buff[len] = '\0';
	}

	string gladepath(buff);
#ifndef __WIN32
   gladepath.erase(gladepath.rfind("/")+1, gladepath.size());
	gladepath += "../share/lag/lag.ui";
#else
   gladepath.erase(gladepath.rfind("\\")+1, gladepath.size());
   gladepath += "lag.ui";
#endif

	if (glade_exists(gladepath))
	{
		return gladepath;
	}

	// For lag not installed:

	//The path of the executable.
	string mypath(programpath);

#ifndef __WIN32
   mypath.erase(mypath.rfind("/")+1, mypath.size());
#else
   mypath.erase(mypath.rfind("\\")+1, mypath.size());
#endif

	string gladename1(mypath);
	string gladename2(mypath);

	gladename1 += "../lag.ui";
	gladename2 += "lag.ui";

	if (glade_exists(gladename1))
	{
		return gladename1;
	}
	else if (glade_exists(gladename2))
	{
		return gladename2;
	}
	else
	{
		std::cerr << "No lag.ui glade file found" << std::endl << "Tried: "
				<< std::endl << gladename1 << std::endl
				<< gladename2;
		exit(1);
	}
}

/*
==================================

 main

==================================
*/
int main(int argc, char** argv)
{
	//This allows the creation and running of threads.
	Glib::thread_init();

	// This is required for GTK to work. It must be the first GTK object
	// created and may not be global.
	Gtk::Main gtkmain(argc, argv);

	//This will extract widgets from the glade file when directed.
	const Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();

	try
	{
		builder->add_from_file(findgladepath(argv[0]));
	} catch (const Glib::FileError& ex)
	{
		std::cerr << "FileError: " << ex.what() << std::endl;
		return 1;
	} catch (const Glib::MarkupError& ex)
	{
		std::cerr << "MarkupError: " << ex.what() << std::endl;
		return 1;
	} catch (const Gtk::BuilderError& ex)
	{
		std::cerr << "BuilderError: " << ex.what() << std::endl;
		return 1;
	}

	Gtk::GL::init(argc, argv);
	Glib::RefPtr<Gdk::GL::Config> glconfig;

	glconfig = Gdk::GL::Config::create(
			Gdk::GL::MODE_RGB | Gdk::GL::MODE_DEPTH | Gdk::GL::MODE_DOUBLE);
	if (glconfig == 0)
	{
		glconfig = Gdk::GL::Config::create(
				Gdk::GL::MODE_RGB | Gdk::GL::MODE_DEPTH);
		if (glconfig == 0)
		{
			cout << "Cannot intialise OpenGL. Exiting." << endl;
			std::exit(1);
		}
	}

	//How many points in each bucket, maximum.
	int bucketlimit = 65536;

	// Label displaying the distance along the ruler, in all dimensions etc.
	// for the overview. Also other text output about the data and fences,
	// profiles etc. is put here.
	Gtk::Label *rulerlabelover = NULL;
	builder->get_widget("rulerlabelover", rulerlabelover);

	//The 2d overview.
	TwoDeeOverview *tdo = new TwoDeeOverview(glconfig, bucketlimit,
			rulerlabelover);

	// Label displaying the distance along the ruler, in all dimensions
	// etc. for the profile.
	Gtk::Label *rulerlabel = NULL;
	builder->get_widget("rulerlabel", rulerlabel);

	//The profile.
	Profile *prof = new Profile(glconfig, bucketlimit, rulerlabel);

	//This contains the widgets of the advanced options window.
	AdvancedOptionsWindow *aow = new AdvancedOptionsWindow(tdo, prof, builder);

	//This contains the widgets of the file saver window.
	FileSaver *fs = new FileSaver(tdo, prof, builder);

	// Contains the overview. It is used to simulate the 2d overview getting
	// focus without causing it to be redrawn every time.
	Gtk::EventBox *eventboxtdo = NULL;
	builder->get_widget("eventboxtdo", eventboxtdo);

	// Contains the profile. It is used to simulate the profile getting focus
	// without causing it to be redrawn every time.
	Gtk::EventBox *eventboxprof = NULL;
	builder->get_widget("eventboxprof", eventboxprof);
	Gtk::Window *overviewwindow = NULL;
	builder->get_widget("overviewwindow", overviewwindow);
	Gtk::Window *profilewindow = NULL;
	builder->get_widget("profilewindow", profilewindow);

	//This contains the widgets of the profile window.
	ProfileWindow *profwin = new ProfileWindow(prof, tdo, profilewindow,
			overviewwindow, eventboxprof, builder, aow);

	//This contains the widgets of the 2D overview window.
	TwoDeeOverviewWindow *tdow = new TwoDeeOverviewWindow(tdo, aow, fs,
			overviewwindow, profilewindow, builder, eventboxtdo, profwin);

	//This contains the widgets of advanced loading options window
	AdvancedLoadDialog* ald = new AdvancedLoadDialog(builder);

	//This contains the widgets of the file opener window.
	FileOpener *fo = new FileOpener(tdo, prof, builder, aow, fs, bucketlimit,
			eventboxtdo, eventboxprof, tdow, ald);

	gdk_threads_enter();

	gtkmain.run(*overviewwindow);

	gdk_threads_leave();

   tdo->stopDrawingThread();
   prof->stopDrawingThread();

	delete tdow;
	delete profwin;
	delete fo;
	delete fs;
	delete aow;
	delete eventboxtdo;
	delete eventboxprof;
	delete rulerlabelover;
	delete overviewwindow;
	delete profilewindow;
	delete ald;
	if (tdo != NULL)
		delete tdo;
	if (prof != NULL)
		delete prof;

	return 0;
}
