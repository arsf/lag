/*
===============================================================================

 ProfileWindow.cpp

 Created on: June-July 2012
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

===============================================================================
*/

#include "ProfileWindow.h"


/*
==================================
 ProfileWindow::ProfileWindow
==================================
*/
ProfileWindow::ProfileWindow(Profile *prof, TwoDeeOverview *tdo, Gtk::Window *profilewindow, Gtk::Window *overviewwindow,
							 Gtk::EventBox *eventboxprof, const Glib::RefPtr<Gtk::Builder>& builder, AdvancedOptionsWindow *aow)
:
		prof			(prof),
		tdo				(tdo),
		aow				(aow),
		eventboxprof	(eventboxprof),
		profilewindow	(profilewindow),
		overviewwindow	(overviewwindow)
{
	profileworker = NULL;

	classifyworker = new ClassifyWorker(this->prof);
	classifyworker->sig_done.connect(sigc::mem_fun(*this, &ProfileWindow::points_classified));
	classifyworker->start();

	load_xml(builder);

	profilewindow->set_title("LAG Profile");

	pointwidthselectprof->set_value(2);
	movingaveragerangeselect->set_value(5);
	slantwidthselectprof->set_value(5);
	classificationselect->set_value(7);

	connect_signals();

	profilewindow->show_all();

	set_statusbar_label("");

	prof->setshowheightscale(showheightscalecheck->get_active());
	prof->setpointwidth(pointwidthselectprof->get_value());
	prof->setdrawpoints(pointshowtoggle->get_active());
	prof->setdrawmovingaverage(lineshowtoggle->get_active());
	prof->setmavrgrange(movingaveragerangeselect->get_value());
	prof->setslantwidth(slantwidthselectprof->get_value());
	prof->setslantwidth(slantwidthselectprof->get_value());
	prof->setslanted(slantedprof->get_active());
}

/*
==================================
 ProfileWindow::~ProfileWindow
==================================
*/
ProfileWindow::~ProfileWindow()
{
   delete classificationselect;
   delete showheightscalecheck;
   delete colourbyintensitymenuprof;
   delete colourbyheightmenuprof;
   delete colourbyflightlinemenuprof;
   delete colourbyclassificationmenuprof;
   delete colourbyreturnmenuprof;
   delete brightnessbyintensitymenuprof;
   delete brightnessbyheightmenuprof;
   delete pointwidthselectprof;
   delete pointshowtoggle;
   delete lineshowtoggle;
   delete movingaveragerangeselect;
   delete fencetoggleprof;
   delete rulertoggle;
   delete slantwidthselectprof;
   delete slantedprof;
   delete refreshbuttonprof;
   delete heightsbuttonprof;
   delete profstatuslabel;
   delete heightsexcludenoise;
}

/*
==================================
 ProfileWindow::load_xml
==================================
*/
void ProfileWindow::load_xml(const Glib::RefPtr<Gtk::Builder>& builder)
{
	builder->get_widget("showheightscalecheck", 			showheightscalecheck);
	builder->get_widget("colourbynonemenuprof",				colourbynonemenuprof);
	builder->get_widget("colourbyintensitymenuprof",		colourbyintensitymenuprof);
	builder->get_widget("colourbyheightmenuprof",			colourbyheightmenuprof);
	builder->get_widget("colourbyflightlinemenuprof",		colourbyflightlinemenuprof);
	builder->get_widget("colourbyclassificationmenuprof",	colourbyclassificationmenuprof);
	builder->get_widget("colourbyreturnmenuprof",			colourbyreturnmenuprof);
	builder->get_widget("brightnessbynonemenuprof",			brightnessbynonemenuprof);
	builder->get_widget("brightnessbyintensitymenuprof",	brightnessbyintensitymenuprof);
	builder->get_widget("brightnessbyheightmenuprof",		brightnessbyheightmenuprof);
	builder->get_widget("fencetoggleprof",					fencetoggleprof);
	builder->get_widget("rulertoggle",						rulertoggle);
	builder->get_widget("pointwidthselectprof", 			pointwidthselectprof);
	builder->get_widget("pointshowtoggle",					pointshowtoggle);
	builder->get_widget("lineshowtoggle",	                lineshowtoggle);
	builder->get_widget("movingaveragerangeselect",	        movingaveragerangeselect);
	builder->get_widget("returnbuttonprof",	                returnbuttonprof);
	builder->get_widget("showprofilebutton",	            showprofilebutton);
	builder->get_widget("classbutton",	                    classbutton);
	builder->get_widget("slantedprof",	                    slantedprof);
	builder->get_widget("slantwidthselectprof",             slantwidthselectprof);
	builder->get_widget("classificationselect",	            classificationselect);
	builder->get_widget("refreshbuttonprof",			    refreshbuttonprof);
	builder->get_widget("heightsbuttonprof",	            heightsbuttonprof);
	builder->get_widget("profstatuslabel",		            profstatuslabel);
	builder->get_widget("heightsexcludenoise",		        heightsexcludenoise);
}

/*
==================================
 ProfileWindow::connect_signals
==================================
*/
void ProfileWindow::connect_signals()
{
	eventboxprof->signal_key_press_event().connect(sigc::mem_fun(*this,&ProfileWindow::on_prof_key_press));
	showheightscalecheck->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_showheightscalecheck));
	colourbynonemenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_colouractivatedprof));
	colourbyintensitymenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_colouractivatedprof));
	colourbyheightmenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_colouractivatedprof));
	colourbyflightlinemenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_colouractivatedprof));
	colourbyclassificationmenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_colouractivatedprof));
	colourbyreturnmenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_colouractivatedprof));
	brightnessbynonemenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_brightnessactivatedprof));
	brightnessbyintensitymenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_brightnessactivatedprof));
	brightnessbyheightmenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_brightnessactivatedprof));
	fencetoggleprof->signal_toggled().connect(sigc::mem_fun(*this,&ProfileWindow::on_fencetoggleprof));
	rulertoggle->signal_toggled().connect(sigc::mem_fun(*this,&ProfileWindow::on_rulertoggle));
	pointwidthselectprof->signal_value_changed().connect(sigc::mem_fun(*this,&ProfileWindow::on_pointwidthselectedprof));
	pointshowtoggle->signal_toggled().connect(sigc::mem_fun(*this,&ProfileWindow::on_pointshowtoggle));
	lineshowtoggle->signal_toggled().connect(sigc::mem_fun(*this,&ProfileWindow::on_lineshowtoggle));
	movingaveragerangeselect->signal_value_changed().connect(sigc::mem_fun(*this,&ProfileWindow::on_movingaveragerangeselect));
	returnbuttonprof->signal_clicked().connect(sigc::mem_fun(*this,&ProfileWindow::on_returnbuttonprof_clicked));
	showprofilebutton->signal_clicked().connect(sigc::mem_fun(*this,&ProfileWindow::on_showprofilebutton_clicked));
	classbutton->signal_clicked().connect(sigc::mem_fun(*this,&ProfileWindow::on_classbutton_clicked));
	slantedprof->signal_toggled().connect(sigc::mem_fun(*this,&ProfileWindow::on_slantedprof));
	slantwidthselectprof->signal_value_changed().connect(sigc::mem_fun(*this,&ProfileWindow::on_slantwidthselectedprof));
	refreshbuttonprof->signal_clicked().connect(sigc::mem_fun(*this,&ProfileWindow::on_refreshbutton_clicked));
	heightsbuttonprof->signal_clicked().connect(sigc::mem_fun(*this,&ProfileWindow::on_heightsbuttonprof_clicked));
}

/*
==================================
 ProfileWindow::on_refreshbutton_clicked
==================================
*/
void ProfileWindow::on_refreshbutton_clicked()
{
	if (prof->get_realized()) prof->drawviewable(1);
}

/*
==================================
 ProfileWindow::on_heightsbuttonprof_clicked
==================================
*/
void ProfileWindow::on_heightsbuttonprof_clicked()
{
	Gtk::MessageDialog dialog("Average heights for flightlines", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CLOSE);
	std::ostringstream message;
	std::vector<double> avgs = prof->get_averages(heightsexcludenoise->get_active());
	size_t size = avgs.size();

	for (size_t i = 0; i < size; ++i)
	{
		message << i << ": " << avgs.at(i) << "\n";
	}

	if (size > 1)
	{
		message << "\nElevation difference in centimetres:\n";

		for (size_t i = 0; i < size; ++i)
		{
			for (size_t j = size-1; j > i; --j)
			{
				message << i << " - " << j << ": "
						<< (avgs[i] - avgs[j]) * 100
						<< "\n";
			}
		}
	}

	dialog.set_secondary_text(message.str());
	dialog.run();
}

/*
==================================
 ProfileWindow::on_showheightscalecheck

 When toggled, the height scale is shown on the profile.
==================================
*/
void ProfileWindow::on_showheightscalecheck()
{
   prof->setshowheightscale(showheightscalecheck->get_active());
   if(prof->is_realized())prof->drawviewable(2);
}

/*
==================================
 ProfileWindow::on_colouractivatedprof

 Does the same as on_colouractivated, except for the profile.
==================================
*/
void ProfileWindow::on_colouractivatedprof()
{
   if (colourbyintensitymenuprof->get_active())
      prof->setColourBy(prof->colourByIntensity);
   else if (colourbyheightmenuprof->get_active())
      prof->setColourBy(prof->colourByHeight);
   else if (colourbyflightlinemenuprof->get_active())
      prof->setColourBy(prof->colourByFlightline);
   else if (colourbyclassificationmenuprof->get_active())
      prof->setColourBy(prof->colourByClassification);
   else if (colourbyreturnmenuprof->get_active())
      prof->setColourBy(prof->colourByReturn);
   else
      prof->setColourBy(prof->colourByNone);

   if(prof->is_realized())
	   prof->drawviewable(1);
}

/*
==================================
 ProfileWindow::on_brightnessactivatedprof

 Does the same as on_brightnessactivated, except for the profile.
==================================
*/
void ProfileWindow::on_brightnessactivatedprof()
{
   if (brightnessbyintensitymenuprof->get_active())
      prof->setBrightnessBy(prof->brightnessByIntensity);
   else if (brightnessbyheightmenuprof->get_active())
      prof->setBrightnessBy(prof->brightnessByHeight);
   else
      prof->setBrightnessBy(prof->brightnessByNone);

   if(prof->is_realized())prof->drawviewable(1);
}

/*
==================================
 ProfileWindow::on_showprofilebutton_clicked

 This grabs the profile from the overview.
==================================
*/
void ProfileWindow::on_showprofilebutton_clicked()
{
   if(tdo->is_realized())
	   profilewindow->present();

   if (profileworker != NULL)
	   return;

   set_statusbar_label("Loading profile...");

   // These are NOT to be deleted here as the arrays they will point to 
   // will be managed by the TwoDeeOVerview object.
   vector<double> profxs;
   vector<double> profys;
   int profps = 0;

   if(tdo->is_realized())
   {
	   tdo->getprofile(profxs,profys,profps);
	   if (tdo->get_slicing())
	   {
		   prof->set_slicing(true);
		   prof->set_minz(tdo->get_slice_minz());
		   prof->set_maxz(tdo->get_slice_maxz());
	   }
	   else
	   {
		   prof->set_slicing(false);
	   }
   }

   if(!profxs.empty()  && !profys.empty())
   {
      profileworker = new ProfileWorker(this->prof, profxs, profys, profps);
      profileworker->sig_done.connect(sigc::mem_fun(*this, &ProfileWindow::profile_loaded));
      profileworker->start();

      // Change cursor to busy
      make_busy_cursor(true);
   }
}

/*
==================================
 ProfileWindow::profile_loaded
==================================
*/
void ProfileWindow::profile_loaded()
{
	delete profileworker;
	profileworker = NULL;
	make_busy_cursor(false);

	set_statusbar_label("");

   prof->draw_profile(true);
}

/*
==================================
 ProfileWindow::make_busy_cursor
==================================
*/
void ProfileWindow::make_busy_cursor(bool busy)
{
	{
		Glib::Mutex::Lock lock (mutex);

		GdkDisplay* display;
		GdkCursor* cursor;
		GdkWindow* window;

		display = gdk_display_get_default();

		if (busy)
			cursor = gdk_cursor_new(GDK_WATCH);
		else
			cursor = gdk_cursor_new(GDK_LEFT_PTR);

		window = (GdkWindow*) profilewindow->get_window()->gobj();
		gdk_window_set_cursor(window, cursor);
		window = (GdkWindow*) prof->get_window()->gobj();
		if (!busy && (fencetoggleprof->get_active() || rulertoggle->get_active()))
		{
			gdk_cursor_unref(cursor);
			cursor = gdk_cursor_new(GDK_CROSSHAIR);
		}

		gdk_window_set_cursor(window, cursor);
		gdk_cursor_unref(cursor);

		gdk_display_sync(display);
	}
}

/*
==================================
 ProfileWindow::on_returnbuttonprof_clicked

 This returns the profile to its original position.
==================================
*/
void ProfileWindow::on_returnbuttonprof_clicked()
{
   if(prof->is_realized())
	   prof->returntostart();
}

/*
==================================
 ProfileWindow::on_pointwidthselectedprof

 Does the same as on_pointwidthselected, except for the profile.
==================================
*/
void ProfileWindow::on_pointwidthselectedprof()
{
   prof->setpointwidth(pointwidthselectprof->get_value());
   if(prof->is_realized())
	   prof->drawviewable(2);
}

/*
==================================
 ProfileWindow::on_pointshowtoggle

 Determines whether to display the points on the profile.
==================================
*/
void ProfileWindow::on_pointshowtoggle()
{
   prof->setdrawpoints(pointshowtoggle->get_active());
   if(prof->is_realized())
	   prof->drawviewable(1);
}

/*
==================================
 ProfileWindow::on_lineshowtoggle

 Determines whether to display the (best fit) lines on the profile.
==================================
*/
void ProfileWindow::on_lineshowtoggle()
{
   prof->setdrawmovingaverage(lineshowtoggle->get_active());
   if(prof->is_realized())
	   prof->drawviewable(1);
}

/*
==================================
 ProfileWindow::on_movingaveragerangeselect

 The best fit is a moving average, and this changes the range, and
 therefore the shape of the line.
==================================
*/
void ProfileWindow::on_movingaveragerangeselect()
{
   prof->setmavrgrange(movingaveragerangeselect->get_value());
   prof->make_moving_average();
   if(prof->is_realized())
	   prof->drawviewable(1);
}

/*
==================================
 ProfileWindow::on_classbutton_clicked

 This classifies the points surrounded by the fence.
==================================
*/
void ProfileWindow::on_classbutton_clicked()
{
	//if (classifyworker != NULL)
	//	return;

	set_statusbar_label("Classifying...");

	if(prof->is_realized())
	{
      prof->queueActiveFence(classificationselect->get_value_as_int());
      classifyworker->nudge();

      if (prof->hasClassifyJobs())
		   make_busy_cursor(true);
	}
}

/*
==================================
 ProfileWindow::points_classified
==================================
*/
void ProfileWindow::points_classified()
{
   prof->draw_profile(false);

	// Set cursor back to normal
   if (!(prof->hasClassifyJobs()))
	   make_busy_cursor(false);

	set_statusbar_label("");

	tdo->drawviewable(1);
}

void ProfileWindow::set_statusbar_label(std::string text)
{
	Glib::Mutex::Lock lock (mutex);
	profstatuslabel->set_text(text);
}

/*
==================================
 ProfileWindow::on_fencetoggleprof

 Toggles whether clicking and dragging will select the fence in the profile.
==================================
*/
void ProfileWindow::on_fencetoggleprof()
{
   if(fencetoggleprof->get_active())
   {
      if(rulertoggle->get_active())
         rulertoggle->set_active(false);

      prof->setupfence();

      if(prof->is_realized())
         prof->drawviewable(1);
   }
   else{
      prof->unsetupfence();
      if(prof->is_realized() && !rulertoggle->get_active() &&
         !fencetoggleprof->get_active()) 
         prof->drawviewable(1);
   }
}

/*
==================================
 ProfileWindow::on_slantwidthselectedprof
==================================
*/
void ProfileWindow::on_slantwidthselectedprof()
{
   prof->setslantwidth(slantwidthselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(1);
}

/*
==================================
 ProfileWindow::on_slantedprof
==================================
*/
void ProfileWindow::on_slantedprof()
{

   prof->setslanted(slantedprof->get_active());

   if(prof->is_realized())
      prof->drawviewable(1);
}

/*
==================================
 ProfileWindow::on_rulertoggle

 When toggled, the profile view goes into rulering mode. When untoggled,
 rulering mode ends.
==================================
*/
void ProfileWindow::on_rulertoggle()
{
   if(rulertoggle->get_active())
   {
      if(fencetoggleprof->get_active())
         fencetoggleprof->set_active(false);
      
      prof->setupruler();
      
      if(prof->is_realized())
         prof->drawviewable(1);
   }
   else
   {
      prof->unsetupruler();
      if(prof->is_realized()&&!rulertoggle->get_active()&&
         !fencetoggleprof->get_active())
         prof->drawviewable(1);
   }
}

/*
==================================
 ProfileWindow::set_classification
==================================
*/
void ProfileWindow::set_classification(double classification)
{
	classificationselect->set_value(classification);
}

/*
==================================
 ProfileWindow::on_prof_key_press

 This takes keyboard input from the event box around the profile area and
 interprets it.
==================================
*/
bool ProfileWindow::on_prof_key_press(GdkEventKey* event)
{
   // Do nothing unless the overview and profile are realized, otherwise 
   // segfaults may happen.
   if(!tdo->is_realized())
      return false;

   if(!prof->is_realized())
      return false;
  
   switch(event->keyval)
   {
      //Grab new profile.
      case GDK_P:
      case GDK_p:
      case GDK_space:
         on_showprofilebutton_clicked();
         return true;
         break;
      //Classify.
      case GDK_C:
      case GDK_c:
      case GDK_K:
      case GDK_k:
      case GDK_Return:
         on_classbutton_clicked();
         return true;
         break;
      // Set classification
      case GDK_1:
    	  set_classification(1);
    	  return true;
    	  break;
      case GDK_7:
    	  set_classification(7);
    	  return true;
    	  break;
      case GDK_2:
    	  set_classification(2);
    	  return true;
    	  break;
      case GDK_3:
    	  set_classification(3);
    	  return true;
    	  break;
      case GDK_4:
    	  set_classification(4);
    	  return true;
    	  break;
      case GDK_5:
    	  set_classification(5);
    	  return true;
    	  break;
      case GDK_6:
    	  set_classification(6);
    	  return true;
    	  break;
      //Move view.
      case GDK_w:
      case GDK_s:
      case GDK_a:
      case GDK_d:
         return prof->on_pan_key(event,aow->getmovespeed());
         break;
      //Move the fence if visible.
      case GDK_W:
      case GDK_S:
      case GDK_A:
      case GDK_D:
         if(fencetoggleprof->get_active())
            return prof->on_fence_key(event,aow->getmovespeed());
         else 
            return false;
         break;
      //Scroll the profile while updating automatically.
      case GDK_r:
      case GDK_v:
      case GDK_q:
      case GDK_e:
      case GDK_R:
      case GDK_V:
      case GDK_Q:
      case GDK_E:
         return on_profile_shift(event);
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
         return prof->on_zoom_key(event);
         break;
      //Redraw.
      case GDK_z:
      case GDK_Z:
         return prof->drawviewable(1);
         break;
      //Toggle fencing mode.
      case GDK_f:
      case GDK_F:
      fencetoggleprof->set_active(!fencetoggleprof->get_active());
      return true;
      break;
      //Toggle orthogonal or slanted fence.
      case GDK_t:
      case GDK_T:
         slantedprof->set_active(!slantedprof->get_active());
         return true;
         break;
      //Switch to overview window.
      case GDK_slash:
      case GDK_backslash:
         overviewwindow->present();
         return true;
         break;
      
      default:
         return false;
         break;
   }
}

/*
==================================
 ProfileWindow::on_profile_shift
==================================
*/
bool ProfileWindow::on_profile_shift(GdkEventKey* event)
{
   //Translate signal to be compatible with called methods.
   switch(event->keyval)
   {
      case GDK_r:
      case GDK_R:
         event->keyval = GDK_W;
         break;
      case GDK_v:
      case GDK_V:
         event->keyval = GDK_S;
         break;
      case GDK_q:
      case GDK_Q:
         event->keyval = GDK_A;
         break;
      case GDK_e:
      case GDK_E:
         event->keyval = GDK_D;
         break;
      default:
         return false;
         break;
   }
   //Move profile box on overview, using fractional shift.
   bool shifted = tdo->on_prof_key(event,aow->getmovespeed(),true);

   if(!shifted)
      return shifted;
   // Switch back to profile window (as tdo->on_prof_key switches to the 
   // overview window.
   if(tdo->is_realized())
      profilewindow->present();

   // These are NOT to be deleted here as the arrays they will point to will 
   // be managed by the TwoDeeOVerview object.
   vector<double> profxs , profys;
   int profps = 0;
   if(tdo->is_realized())
      tdo->getprofile(profxs,profys,profps);

   // Changed centre x and y values etc. so that points in profile remain 
   // within the viewing box (i.e. the clipping planes).
   if(!profxs.empty()&&!profys.empty())
   {
      shifted = prof->shift_viewing_parameters(event,aow->getmovespeed());
      if(!shifted)return shifted;

      // Showprofile uses the getpoint() method, and that must never be used 
      // by more than one thread at once.
      prof->loadprofile(profxs,profys,profps);
      prof->draw_profile(false);
      return true;
   }
   return false;
}

