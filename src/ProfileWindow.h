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
 * File: ProfileWindow.h
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
#include "AdvancedOptionsWindow.h"
#ifndef PROFILEWINDOW_H
#define PROFILEWINDOW_H

class ProfileWindow{
public:
   ProfileWindow(Profile *prof,
                 TwoDeeOverview *tdo,
                 Gtk::Window *profilewindow,
                 Gtk::Window *overviewwindow,
                 Gtk::EventBox *eventboxprof,
                 Glib::RefPtr<Gnome::Glade::Xml> refXml,
                 AdvancedOptionsWindow *aow);
   ~ProfileWindow();
   //This grabs the profile from the overview.
   void on_showprofilebutton_clicked();
protected:
   Profile *prof;
   TwoDeeOverview *tdo;
   AdvancedOptionsWindow *aow;
   // This contains the profile area and takes keyboard input for it.
   Gtk::EventBox *eventboxprof;
   Gtk::Window *profilewindow;
   Gtk::Window *overviewwindow;
   // Check button determining whether the height scale is viewable on 
   // the profile.
   Gtk::CheckMenuItem *showheightscalecheck;
   //Determines whether the profile is coloured by intensity.
   Gtk::RadioMenuItem *colourbyintensitymenuprof;
   //Determines whether the profile is coloured by height.
   Gtk::RadioMenuItem *colourbyheightmenuprof;
   //Determines whether the profile is coloured by flightline.
   Gtk::RadioMenuItem *colourbyflightlinemenuprof;
   //Determines whether the profile is coloured by classification.
   Gtk::RadioMenuItem *colourbyclassificationmenuprof;
   //Determines whether the profile is coloured by return.
   Gtk::RadioMenuItem *colourbyreturnmenuprof;
   //Determines whether the profile is shaded by intensity.
   Gtk::RadioMenuItem *brightnessbyintensitymenuprof;
   //Determines whether the profile is shaded by height.
   Gtk::RadioMenuItem *brightnessbyheightmenuprof;
   //Determines the width of the points in the profile in pixels.
   Gtk::SpinButton *pointwidthselectprof;
   //Whether to show the points on the profile.
   Gtk::ToggleToolButton *pointshowtoggle;
   //Whether to show the lines on the profile.
   Gtk::ToggleToolButton *lineshowtoggle;
   //The range of the moving average for the lines on the profile.
   Gtk::SpinButton *movingaveragerangeselect;
   //Toggle button determining whether mouse dragging selects the fence.
   Gtk::ToggleToolButton *fencetoggleprof;
   //Toggle button determining whether the ruler is viewable on the profile.
   Gtk::ToggleToolButton *rulertoggle;
   //Determines the width of the fence in metres when slanted.
   Gtk::SpinButton *slantwidthselectprof;
   //Determine whether the profile fence is orthogonal or slanted.
   Gtk::ToggleToolButton *orthogonalprof;
   Gtk::ToggleToolButton *slantedprof;
   // This determines what to classify points as when selected through the 
   // profile.
   Gtk::SpinButton *classificationselect;

   //When toggled, the height scale is shown on the profile.
   void on_showheightscalecheck();
   //Does the same as on_colouractivated, except for the profile.
   void on_colouractivatedprof();
   //Does the same as on_brightnessactivated, except for the profile.
   void on_brightnessactivatedprof();

   //This returns the profile to its original position.
   void on_returnbuttonprof_clicked();
   //Does the same as on_pointwidthselected, except for the profile.
   void on_pointwidthselectedprof();
   //Determines whether to display the points on the profile.
   void on_pointshowtoggle();
   //Determines whether to display the (best fit) lines on the profile.
   void on_lineshowtoggle();
   // The best fit is a moving average, and this changes the range, and 
   // therefore the shape of the line.
   void on_movingaveragerangeselect();
   //This classifies the points surrounded by the fence.
   void on_classbutton_clicked();
   //Toggles whether clicking and dragging will select the fence in the profile.
   void on_fencetoggleprof();
   //Updates the slant width and redraws the fence when the spinbutton has its 
   //value changed.
   void on_slantwidthselectedprof();
   //These update the fence when the shape to draw it is changed.
   void on_orthogonalprof();
   //...
   void on_slantedprof();
   // When toggled, the profile view goes into rulering mode. When untoggled, 
   // rulering mode ends.
   void on_rulertoggle();

   //Interpret keybaord signals from the EventBox.
   bool on_prof_key_press(GdkEventKey* event);
   //Manage scrolling of the profile with automatic redraw.
   bool on_profile_shift(GdkEventKey* event);

};

#endif