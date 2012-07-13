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
 * File: lag.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: November 2009 - June 2010
 *
 * */
#include <iostream>

#include <gtkmm.h>
#include <gtkglmm.h>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include "Quadtree.h"
#include "TwoDeeOverview.h"
#include "Profile.h"
#include "ui/TwoDeeOverviewWindow.h"
#include "ui/ProfileWindow.h"
#include "ui/FileOpener.h"
#include "ui/AdvancedLoadDialog.h"

using namespace std;
namespace fs = boost::filesystem;

bool glade_exists(fs::path const& filename)
{
	return fs::exists(filename);
}

// Takes the path the executable was called with and uses it to find the 
// .glade file needed to make the GUI.
string findgladepath(char* programpath)
{
	// For lag installed in /usr/local/bin, /usr/local/share/lag
   // EDIT: Now supporting similar windows layouts
   //       i.e., C:\Program Files\PML\Lag\bin, now produces something
   //             like C:\Program Files\PML\Lag\share\lag\lag.ui

	string exepath;

	// Find the path of the executable
	char buff[1024];
   ssize_t len;
#ifdef _WIN32
   // windows only
   len = GetModuleFileName(NULL, buff, sizeof(buff)-1);
#else
   // linux only
	len = readlink("/proc/self/exe", buff, sizeof(buff)-1);
#endif
	if(len > 0) // len = -1 for linux failure, 0 for windows failure
	{
		buff[len] = '\0';
		exepath = string(buff);
	}

	fs::path gladepath(exepath);
	gladepath = fs::system_complete(gladepath).remove_filename();
	gladepath /= ".." / "share" / "lag" / "lag.ui";

	if (glade_exists(gladepath))
	{
		return gladepath.string();
	}

	// For lag not installed

	//The path of the executable.
	fs::path mypath(programpath);

	fs::path gladename1 = fs::system_complete(mypath).remove_filename();
	fs::path gladename2 = gladename1;

	gladename1 /= ("../lag.ui");
	gladename2 /= ("lag.ui");

	if (glade_exists(gladename1))
	{
		return gladename1.string();
	}
	else if (glade_exists(gladename2))
	{
		return gladename2.string();
	}
	else
	{
		std::cerr << "No lag.ui glade file found" << std::endl << "Tried: "
				<< std::endl << gladename1.string() << std::endl
				<< gladename2.string();
		exit(1);
	}
}

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
	TwoDeeOverview *tdo = new TwoDeeOverview(argv[0], glconfig, bucketlimit,
			rulerlabelover);

	// Label displaying the distance along the ruler, in all dimensions
	// etc. for the profile.
	Gtk::Label *rulerlabel = NULL;
	builder->get_widget("rulerlabel", rulerlabel);

	//The profile.
	Profile *prof = new Profile(argv[0], glconfig, bucketlimit, rulerlabel);

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

	//In case of command-line commands
	//fo->testfilename(argc,argv,true,false);

	gdk_threads_enter();

	gtkmain.run(*overviewwindow);

	gdk_threads_leave();

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
