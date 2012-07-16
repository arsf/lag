/*
===============================================================================

 TwoDeeOverviewWindow.cpp

 Created on: June-July 2010
 Authors: Haraldur Tristan Gunnarsson, Jan Holownia

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

===============================================================================
*/

#include <gtkmm.h>
#include <gtkglmm.h>
#include <vector>
#include "TwoDeeOverviewWindow.h"

TwoDeeOverviewWindow::TwoDeeOverviewWindow(TwoDeeOverview *tdo, AdvancedOptionsWindow *aow, FileSaver *fs, Gtk::Window *tdowin,
		Gtk::Window *profilewindow, const Glib::RefPtr<Gtk::Builder>& builder, Gtk::EventBox *eventboxtdo, ProfileWindow *profwin)
:
		tdo				(tdo),
		profwin			(profwin),
		eventboxtdo		(eventboxtdo),
		tdowin			(tdowin),
		profilewindow 	(profilewindow),
		aow				(aow),
		fs				(fs),
		drawwhentoggled	(true)
{
   load_xml(builder);

   pointwidthselect->set_value(1);
   slantwidthselect->set_value(5);

   connect_signals();
      
   tdowin->show_all();

   //Initialisations:
   tdo->setshowdistancescale(showdistancescalecheck->get_active());
   tdo->setshowlegend(showlegendcheck->get_active());
   tdo->setreversez(reverseheightcheck->get_active());
   tdo->setpointwidth(pointwidthselect->get_value());

   tdo->getprofbox()->setslantwidth(slantwidthselect->get_value());
   tdo->getfencebox()->setslantwidth(slantwidthselect->get_value());
   tdo->getprofbox()->setslantedshape(slantedrectshapetoggle->get_active());
   tdo->getfencebox()->setslantedshape(slantedrectshapetoggle->get_active());
   tdo->setraiseline(raiselinecheckmenu->get_active());
   tdo->setlinetoraise(raiselineselect->get_value_as_int());

   // This is to help prevent confusion when the user decides to show the
   // legend and nothing happens because of there being no legend when
   // colouring by flightline or by none.
   showlegendcheck->set_inconsistent(!colourbyintensitymenu->get_active() &&
		   	   	   	   	   	   	   	 !colourbyheightmenu->get_active() &&
		   	   	   	   	   	   	   	 !colourbyclassificationmenu->get_active() &&
		   	   	   	   	   	   	   	 !colourbyreturnmenu->get_active());
}

TwoDeeOverviewWindow::~TwoDeeOverviewWindow()
{
   delete raiselineselect;
   delete raiselinecheckmenu;

   delete help;
   delete about;

   delete showdistancescalecheck;
   delete showlegendcheck;
   delete reverseheightcheck;

   delete colourbyintensitymenu;
   delete colourbyheightmenu;
   delete colourbyflightlinemenu;
   delete colourbyclassificationmenu;
   delete colourbyreturnmenu;
   delete brightnessbyintensitymenu;
   delete brightnessbyheightmenu;

   delete fencetoggle;
   delete profiletoggle;
   delete slantedrectshapetoggle;
   delete slantwidthselect;
   delete pointwidthselect;
   delete rulertoggleover;

   delete saveasfilemenuitem;
   delete quitfilemenuitem;
   delete helpmenu;
   delete aboutmenu;
   delete colourbynonemenu;
   delete brightnessbynonemenu;
   delete quadtreemenu;

   delete returnbutton;
   delete advancedbutton;

   delete superzoombutton;
   delete refreshbutton;
   delete saveasbutton;
   delete slicebutton;
   delete zminselect;
   delete zmaxselect;
   delete uselatlongcheck;
}

/*
=============================================
TwoDeeOverviewWindow::load_xml

Loads widgets from xml file.
=============================================
*/
void TwoDeeOverviewWindow::load_xml(const Glib::RefPtr<Gtk::Builder>& builder)
{
    builder->get_widget("saveasfilemenuitem",	 	saveasfilemenuitem);
    builder->get_widget("quitfilemenuitem",	 		quitfilemenuitem);
    builder->get_widget("showdistancescalecheck",	showdistancescalecheck);
    builder->get_widget("showlegendcheck",	 		showlegendcheck);
    builder->get_widget("reverseheightcheck", 		reverseheightcheck);
    builder->get_widget("colourbynonemenu", 		colourbynonemenu);
    builder->get_widget("colourbyintensitymenu", 	colourbyintensitymenu);
    builder->get_widget("colourbyheightmenu",	 	colourbyheightmenu);
    builder->get_widget("colourbyflightlinemenu", 	colourbyflightlinemenu);
    builder->get_widget("colourbyclassificationmenu", colourbyclassificationmenu);
    builder->get_widget("colourbyreturnmenu", 		colourbyreturnmenu);
    builder->get_widget("brightnessbynonemenu", 	brightnessbynonemenu);
    builder->get_widget("brightnessbyintensitymenu", brightnessbyintensitymenu);
    builder->get_widget("brightnessbyheightmenu", 	brightnessbyheightmenu);
    builder->get_widget("helpmenu", 				helpmenu);
    builder->get_widget("help", 					help);
    builder->get_widget("aboutmenu", 				aboutmenu);
    builder->get_widget("about", 					about);
    builder->get_widget("returnbutton", 			returnbutton);
    builder->get_widget("advancedbutton", 			advancedbutton);
    builder->get_widget("pointwidthselect",		 	pointwidthselect);
    builder->get_widget("rulertoggleover", 			rulertoggleover);
    builder->get_widget("fencetoggle", 				fencetoggle);
    builder->get_widget("profiletoggle", 			profiletoggle);
    builder->get_widget("slantedrectshapetoggle",	slantedrectshapetoggle);
    builder->get_widget("slantwidthselect", 		slantwidthselect);
    builder->get_widget("raiselinecheckmenu",		raiselinecheckmenu);
    builder->get_widget("raiselineselect", 			raiselineselect);
    builder->get_widget("superzoombutton",			superzoombutton);
    builder->get_widget("refreshbutton",			refreshbutton);
    builder->get_widget("saveasbutton", 			saveasbutton);
    builder->get_widget("slicebutton", 				slicebutton);
    builder->get_widget("zminselect", 				zminselect);
    builder->get_widget("zmaxselect",  				zmaxselect);
    builder->get_widget("quadtreemenu",				quadtreemenu);
    builder->get_widget("uselatlongcheck",			uselatlongcheck);
}

/*
================================================
TwoDeeOverviewWindow::connect_signals

Connects signals to widgets
================================================
*/
void TwoDeeOverviewWindow::connect_signals()
{
	eventboxtdo->signal_key_press_event().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_tdo_key_press));
	saveasfilemenuitem->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_savefilemenuactivated));
	quitfilemenuitem->signal_activate().connect(sigc::ptr_fun(gtk_main_quit));
	showdistancescalecheck->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_showdistancescalecheck));
	showlegendcheck->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_showlegendcheck));
	reverseheightcheck->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_reverseheightcheck));
	colourbynonemenu->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_colouractivated));
	colourbyintensitymenu->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_colouractivated));
	colourbyheightmenu->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_colouractivated));
	colourbyflightlinemenu->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_colouractivated));
	colourbyclassificationmenu->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_colouractivated));
    colourbyreturnmenu->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_colouractivated));
    brightnessbynonemenu->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_brightnessactivated));
    brightnessbyintensitymenu->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_brightnessactivated));
    brightnessbyheightmenu->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_brightnessactivated));
    helpmenu->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_helpmenuactivated));
    help->signal_response().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_helpresponse));
    aboutmenu->signal_activate().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_aboutmenuactivated));
    about->signal_response().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_aboutresponse));
    returnbutton->signal_clicked().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_returnbutton_clicked));
    advancedbutton->signal_clicked().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_advancedbutton_clicked));
    pointwidthselect->signal_value_changed().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_pointwidthselected));
    rulertoggleover->signal_toggled().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_rulertoggleover));
    fencetoggle->signal_toggled().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_fencetoggle));
    profiletoggle->signal_toggled().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_profiletoggle));
    slantedrectshapetoggle->signal_toggled().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_slantedrectshapetoggle));
    slantwidthselect->signal_value_changed().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_slantwidthselected));
    raiselinecheckmenu->signal_toggled().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_raiselinecheckmenu));
    raiselineselect->signal_value_changed().connect(sigc::mem_fun(this,&TwoDeeOverviewWindow::on_raiselineselected));
    quadtreemenu->signal_activate().connect(sigc::mem_fun(this, &TwoDeeOverviewWindow::on_quadtreemenu_activated));
    saveasbutton->signal_clicked().connect(sigc::mem_fun(this, &TwoDeeOverviewWindow::on_savefilemenuactivated));
    refreshbutton->signal_clicked().connect(sigc::mem_fun(this, &TwoDeeOverviewWindow::on_refreshbuttonclicked));
    superzoombutton->signal_toggled().connect(sigc::mem_fun(this, &TwoDeeOverviewWindow::on_superzoomclicked));
    slicebutton->signal_toggled().connect(sigc::mem_fun(this, &TwoDeeOverviewWindow::on_slicebuttontoggled));
    zminselect->signal_value_changed().connect(sigc::mem_fun(this, &TwoDeeOverviewWindow::on_zminvaluechanged));
    zmaxselect->signal_value_changed().connect(sigc::mem_fun(this, &TwoDeeOverviewWindow::on_zmaxvaluechanged));
    uselatlongcheck->signal_activate().connect(sigc::mem_fun(this, &TwoDeeOverviewWindow::on_uselatlongcheck));
}

void TwoDeeOverviewWindow::on_uselatlongcheck()
{
	tdo->set_latlong(uselatlongcheck->get_active());
}


void TwoDeeOverviewWindow::on_quadtreemenu_activated()
{
	Gtk::MessageDialog dialog("Quadtree information\t", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CLOSE);
	std::ostringstream message;
	Quadtree* qt;
	qt = tdo->getlidatdata();

	if (qt == NULL)
	{
		message << "No quadtree found.";
	}
	else
	{
		Boundary* b;
		b = qt->getBoundary();
		vector<double> xs(4);
		xs[0] = xs[1] = b->minX;
		xs[2] = xs[3] = b->maxX;
		vector<double> ys(4);
		ys[0] = ys[1] = b->minY;
		ys[2] = ys[3] = b->maxY;
		vector<PointBucket*>* pointvector = NULL;

		try
		{
			pointvector = qt->advSubset(xs,ys,4);
		}
		catch(DescriptiveException& e)
		{
			cout << "There has been an exception:" << endl;
			cout << "What: " << e.what() << endl;
			cout << "Why: " << e.why() << endl;
			cout << "No points returned." << endl;
		}

		if(pointvector == NULL || pointvector->size() == 0)
		{
			message << "No data found in the quandtree.";
			delete b;
		}
		else
		{
			int pointsInBuckets = 0;
			for (unsigned int i = 0; i < pointvector->size(); ++i)
			{
				pointsInBuckets +=(*pointvector)[i]->getNumberOfPoints(0);
			}

			double min[3];
			double max[3];
			min[0] = b->minX;
			min[1] = b->minY;
			min[2] = (*pointvector)[0]->getminZ();
			max[0] = b->maxX;
			max[1] = b->maxY;
			max[2] = (*pointvector)[0]->getmaxZ();

			delete b;

			if (uselatlongcheck->get_active())
			{
				tdo->convert_to_latlong(min);
				tdo->convert_to_latlong(max);
			}

			message << "Number of points: " << "\t" << qt->getNumberOfPoints() << "\n"
					<< "Points in buckets: " << "\t" << pointsInBuckets << "\n"
					<< "Number of buckets:  " << pointvector->size() << "\n"
					<< "Bucket size: " << "\t\t" << qt->get_capacity() << "\n"
					<< "Points in cache: " << "\t" << (*pointvector)[0]->get_cache_used() << "\n"
					<< "Size in memory: " << "\t" << (*pointvector)[0]->get_cache_used() * sizeof(LidarPoint) / 1048576 << " MB" << "\n"
					<< "\n"
					<< "Boundary:" << "\n"
					<< "Min X - Max X: " << "\t" << min[0] << " - " << max[0] << "\n"
					<< "Min Y - Max Y: " << "\t" << min[1] << " - " << max[1] << "\n"
					<< "Min Z - Max Z: " << "\t" << min[2] << " - " << max[2] << "\n"
					<< "\n"
					<< "UTM zone: " << "\t" << tdo->get_utm_zone() << "\n";
		}

		delete pointvector;
	}

	dialog.set_secondary_text(message.str());
	dialog.run();
}

void TwoDeeOverviewWindow::on_slicebuttontoggled()
{
	if (slicebutton->get_active())
	{
		tdo->set_slicing(true);
		tdo->set_slice_minz(zminselect->get_value());
		tdo->set_slice_maxz(zmaxselect->get_value());
	}
	else
	{
		tdo->set_slicing(false);
	}
	tdo->drawviewable(1);
}

void TwoDeeOverviewWindow::on_zminvaluechanged()
{
	if (slicebutton->get_active())
	{
		tdo->set_slice_minz(zminselect->get_value());
		tdo->drawviewable(1);
	}
}

void TwoDeeOverviewWindow::on_zmaxvaluechanged()
{
	if (slicebutton->get_active())
	{
		tdo->set_slice_maxz(zmaxselect->get_value());
		tdo->drawviewable(1);
	}
}

void TwoDeeOverviewWindow::set_slice_range(double min, double max)
{
   zminselect->set_range(min, max);
   zminselect->set_value(min);
   zmaxselect->set_range(min, max);
   zmaxselect->set_value(max);
}

void TwoDeeOverviewWindow::on_refreshbuttonclicked()
{
	if (tdo->get_realized())
		tdo->drawviewable(1);
}

void TwoDeeOverviewWindow::on_superzoomclicked()
{
	if (superzoombutton->get_active())
		pointwidthselect->set_value(2.0);
	else
		pointwidthselect->set_value(1.0);

	tdo->set_superzoom(superzoombutton->get_active());
}

//When toggled, the distance scale is shown on the 2d overview.
void TwoDeeOverviewWindow::on_showdistancescalecheck()
{
   tdo->setshowdistancescale(showdistancescalecheck->get_active());
   if(tdo->is_realized())
      tdo->drawviewable(2);
}

//When toggled, the legend is shown on the 2d overview.
void TwoDeeOverviewWindow::on_showlegendcheck()
{
   tdo->setshowlegend(showlegendcheck->get_active());
   if(tdo->is_realized())
      tdo->drawviewable(2);
}

//When toggled, the heights are reversed on the 2d overview.
void TwoDeeOverviewWindow::on_reverseheightcheck()
{
   tdo->setreversez(reverseheightcheck->get_active());
   if(tdo->is_realized())
      tdo->drawviewable(1);
}

// If one of the colour radio menu items is selected (and, therefore, the 
// others deselected) then set the values of the colour control variables 
// in the overview to the values of the corresponding radio menu items.
void TwoDeeOverviewWindow::on_colouractivated()
{
   if (colourbyintensitymenu->get_active())
      tdo->setColourBy(tdo->colourByIntensity);
   else if (colourbyheightmenu->get_active())
      tdo->setColourBy(tdo->colourByHeight);
   else if (colourbyflightlinemenu->get_active())
      tdo->setColourBy(tdo->colourByFlightline);
   else if (colourbyclassificationmenu->get_active())
      tdo->setColourBy(tdo->colourByClassification);
   else if (colourbyreturnmenu->get_active())
      tdo->setColourBy(tdo->colourByReturn);
   else
      tdo->setColourBy(tdo->colourByNone);
 
//   tdo->setintensitycolour(colourbyintensitymenu->get_active());
//   tdo->setheightcolour(colourbyheightmenu->get_active());
//   tdo->setlinecolour(colourbyflightlinemenu->get_active());
//   tdo->setclasscolour(colourbyclassificationmenu->get_active());
//   tdo->setreturncolour(colourbyreturnmenu->get_active());

   // This is to help prevent confusion when the user decides to show the 
   // legend and nothing happens because of there being no legend when 
   // colouring by flightline or by none.
   showlegendcheck->set_inconsistent(!colourbyintensitymenu->get_active() && 
		   	   	   	   	   	   	   	 !colourbyheightmenu->get_active() &&
		   	   	   	   	   	   	   	 !colourbyclassificationmenu->get_active() &&
		   	   	   	   	   	   	   	 !colourbyreturnmenu->get_active());

   // As the signal handler is called twice (each time a radio button is 
   // toggle or untoggled, and toggling one automatically untoggles another), 
   // and we only want to draw (which is slow when caching) once, draw half 
   // the time this handler is called.
   drawwhentoggled = !drawwhentoggled;
   if(tdo->is_realized()&&drawwhentoggled)
      tdo->drawviewable(1);
}

// If one of the brightness radio menu items is selected (and, therefore, 
// the others deselected) then set the values of the brightness control 
// variables in the overview to the values of the corresponding radio 
// menu items.
void TwoDeeOverviewWindow::on_brightnessactivated()
{

   if (brightnessbyintensitymenu->get_active())
      tdo->setBrightnessBy(tdo->brightnessByIntensity);
   else if (brightnessbyheightmenu->get_active())
      tdo->setBrightnessBy(tdo->brightnessByHeight);
   else
      tdo->setBrightnessBy(tdo->brightnessByNone);

//   tdo->setintensitybrightness(brightnessbyintensitymenu->get_active());
//   tdo->setheightbrightness(brightnessbyheightmenu->get_active());

   // As the signal handler is called twice (each time a radio button is 
   // toggle or untoggled, and toggling one automatically untoggles another), 
   // and we only want to draw (which is slow when caching) once, draw half 
   // the time this handler is called.
   drawwhentoggled = !drawwhentoggled;
   if(tdo->is_realized()&&drawwhentoggled)
	   tdo->drawviewable(1);
}

//This returns the overview to its original position.
void TwoDeeOverviewWindow::on_returnbutton_clicked()
{
	pointwidthselect->set_value(1.0);
	double zmin, zmax;
	zminselect->get_range(zmin, zmax);
	zminselect->set_value(zmin);
	zmaxselect->set_value(zmax);
	tdo->setpointwidth(pointwidthselect->get_value());
	if(tdo->is_realized())
		tdo->returntostart();
}

//This changes the width of the points in pixels.
void TwoDeeOverviewWindow::on_pointwidthselected()
{
   tdo->setpointwidth(pointwidthselect->get_value());
   if(tdo->is_realized())
      tdo->drawviewable(1);
}
 
// When toggled, the profile view goes into rulering mode. When untoggled, 
// rulering mode ends.
void TwoDeeOverviewWindow::on_rulertoggleover()
{
   if(rulertoggleover->get_active())
   {
      if(fencetoggle->get_active())
         fencetoggle->set_active(false);
      if(profiletoggle->get_active())
         profiletoggle->set_active(false);
      tdo->setupruler();
      if(tdo->is_realized())
         tdo->drawviewable(2);
   }
   else
   {
      tdo->unsetupruler();
      if(tdo->is_realized() &&
         !profiletoggle->get_active() &&
         !rulertoggleover->get_active() &&
         !fencetoggle->get_active())
         tdo->drawviewable(2);
   }
}
// Toggles whether clicking and dragging will select the fence in the 
// overview.
void TwoDeeOverviewWindow::on_fencetoggle()
{
   if(fencetoggle->get_active())
   {
      if(profiletoggle->get_active())
         profiletoggle->set_active(false);
      if(rulertoggleover->get_active())
         rulertoggleover->set_active(false);
      tdo->setupfence();
      if(tdo->is_realized())
         tdo->drawviewable(2);
   }
   else
   {
   	tdo->unsetupfence();
      if(tdo->is_realized() &&
         !profiletoggle->get_active() &&
         !rulertoggleover->get_active() &&
         !fencetoggle->get_active())
         tdo->drawviewable(2);
   }
}
// When toggled, the 2d overview goes into profile selection mode. When 
// untoggled, 2d overview goes out of profile selection mode and the profile 
// parameters are sent to the profile area.
void TwoDeeOverviewWindow::on_profiletoggle()
{
   if(profiletoggle->get_active())
   {
      if(fencetoggle->get_active())
         fencetoggle->set_active(false);
      if(rulertoggleover->get_active())
         rulertoggleover->set_active(false);
      tdo->setupprofile();
      if(tdo->is_realized())
    	  tdo->drawviewable(2);
   }
   else
   {
   	tdo->unsetupprofile();
      if(tdo->is_realized() &&
         !profilewindow->get_visible())
         profilewindow->show_all();
      if(tdo->is_realized() &&
         !profiletoggle->get_active() &&
         !rulertoggleover->get_active() &&
         !fencetoggle->get_active())
         tdo->drawviewable(2);
   }
}

// When toggled, this makes sure that the orthogonal toggle is in the opposite 
// state and then sets the slanted shape on the overview and makes new profile 
// and fence boundaries before redrawing the overview, possibly with the new 
// boundaries for profile and/or fence displayed.
void TwoDeeOverviewWindow::on_slantedrectshapetoggle()
{
	tdo->getprofbox()->setorthogonalshape(!slantedrectshapetoggle->get_active());
	tdo->getfencebox()->setorthogonalshape(!slantedrectshapetoggle->get_active());
	tdo->getprofbox()->setslantedshape(slantedrectshapetoggle->get_active());
	tdo->getfencebox()->setslantedshape(slantedrectshapetoggle->get_active());
	tdo->getprofbox()->makeboundaries();
	tdo->getfencebox()->makeboundaries();
	if(tdo->is_realized())tdo->drawviewable(2);
}

// When the value in the spinbutton for slanted shape width is changed, tell the 
// 2d overview, then make the new slanted box and then draw it. This does NOT 
// update the profile itself (or, at least, not yet) if the slanted box is for 
// a profile. To update the profile after the width has been satisfactorily 
// adjusted, the profiletoggle must be toggled and then untoggled.
void TwoDeeOverviewWindow::on_slantwidthselected()
{
   tdo->getprofbox()->
      setslantwidth(slantwidthselect->get_value());
   tdo->getfencebox()->
      setslantwidth(slantwidthselect->get_value());

   tdo->getprofbox()->makeboundaries();
   tdo->getfencebox()->makeboundaries();
   tdo->getprofbox()->drawinfo();
   tdo->getfencebox()->drawinfo();

   if(tdo->is_realized())
      tdo->drawviewable(2);
}

//Opens the advanced options dialog.
void TwoDeeOverviewWindow::on_advancedbutton_clicked()
{
   aow->show(); 
}

//Show the help dialog when respective menu item activated.
void TwoDeeOverviewWindow::on_helpmenuactivated()
{
   help->present(); 
}

//Hide the help dialog when close button activated.
void TwoDeeOverviewWindow::on_helpresponse(int response_id)
{
   if(response_id==0)
   {
   }
   help->hide(); 
}

//Show the about dialog when respective menu item activated.
void TwoDeeOverviewWindow::on_aboutmenuactivated()
{
   about->present(); 
}

//Hide the about dialog when close button activated.
void TwoDeeOverviewWindow::on_aboutresponse(int response_id)
{
   if(response_id==0)
   {
   }
   about->hide(); 
}

//When selected from the menu, the file saver opens.
void TwoDeeOverviewWindow::on_savefilemenuactivated()
{
   if(tdo->is_realized())
      fs->show();
   else 
      return;

   fs->on_flightlinesaveselected();
}

//Interprets the keybaord signals from the EventBox.
bool TwoDeeOverviewWindow::on_tdo_key_press(GdkEventKey* event)
{
   if(!tdo->is_realized())
      //If the overview does not exist, prevent segfaults by doing nothing.
      return false;

   switch(event->keyval)
   {
      //Extract a profile.
      case GDK_P:
      case GDK_p:
      case GDK_space:
         profwin->on_showprofilebutton_clicked();
         tdowin->present();
         return true;
         break;

      //Move overview view.
      case GDK_w:
      case GDK_s:
      case GDK_a:
      case GDK_d:
      case GDK_z:
      case GDK_Z:
         return tdo->on_pan_key(event,aow->getmovespeed());
         break;

      //Move one or none of the profile or the fence, depending 
      //on whether either of them are visible.
      case GDK_W:
      case GDK_S:
      case GDK_A:
      case GDK_D:
         if(profiletoggle->get_active())
            return tdo->on_prof_key(event,aow->getmovespeed(), aow->getfractionalshift());
         else if(fencetoggle->get_active())
            return tdo->on_fence_key(event,aow->getmovespeed());
         break;

      //Zoom in and out.
      case GDK_i:
      case GDK_o:
      case GDK_I:
      case GDK_O:
      case GDK_g:
      case GDK_b:
      case GDK_G:
      case GDK_B:
         return tdo->on_zoom_key(event);
         break;

      //Toggle fencing mode.
      case GDK_f:
      case GDK_F:
         fencetoggle->set_active(!fencetoggle->get_active());
         return true;
         break;

      //Toggle profiling mode.
      case GDK_x:
      case GDK_X:
         profiletoggle->set_active(!profiletoggle->get_active());
         return true;
         break;

      // Toggle whether the profile and fence should be orthogonal 
      // or slanted.
      case GDK_t:
      case GDK_T:
         slantedrectshapetoggle->set_active(!slantedrectshapetoggle->get_active());
         return true;
         break;

      //Switch to profile window.
      case GDK_slash:
      case GDK_backslash:
         profilewindow->present();
         return true;
         break;
      default:
         return false;
         break;
   }
   return false;
}

//Determines which line is to be raised above the rest on the overview.
void TwoDeeOverviewWindow::on_raiselineselected()
{
   tdo->setlinetoraise(raiselineselect->get_value_as_int());

   if(tdo->is_realized())
      if(raiselinecheckmenu->get_active())
         tdo->drawviewable(1);
}

//Determines whether a line is to be raised.
void TwoDeeOverviewWindow::on_raiselinecheckmenu()
{
   tdo->setraiseline(raiselinecheckmenu->get_active());
   if(tdo->is_realized())
      tdo->drawviewable(1);
}

Gtk::Window* TwoDeeOverviewWindow::get_profilewindow()
{
	return profilewindow;
}

void TwoDeeOverviewWindow::set_utm_zone(std::string zone)
{
	tdo->set_utm_zone(zone);
	if (zone == "" || zone == "unknown")
		uselatlongcheck->set_inconsistent(true);
	else
		uselatlongcheck->set_inconsistent(false);
}
