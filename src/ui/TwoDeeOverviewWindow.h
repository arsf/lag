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
 * File: TwoDeeOverviewWindow.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: June-July 2010
 *
 * */


#ifndef TWODEEOVERVIEWWINDOW_H
#define TWODEEOVERVIEWWINDOW_H

#include <gtkmm.h>
#include <gtkglmm.h>
#include <vector>
#include "../TwoDeeOverview.h"
#include "AdvancedOptionsWindow.h"
#include "FileSaver.h"
#include "ProfileWindow.h"

class TwoDeeOverviewWindow
{
public:
   TwoDeeOverviewWindow(TwoDeeOverview*,
                        AdvancedOptionsWindow*,
                        FileSaver*,
                        Gtk::Window*,
                        Gtk::Window*,
                        const Glib::RefPtr<Gtk::Builder>&,
                        Gtk::EventBox*,
                        ProfileWindow*);

   ~TwoDeeOverviewWindow();

   // Update so that all loaded flightlines can be raised without leaving 
   // a lot of useless options.
   void setraiselinerange(int first, int second)
   {
      raiselineselect->set_range(first,second); 
   }

   void set_slice_range(double min, double max);

   Gtk::Window* get_profilewindow();

private:
   TwoDeeOverview *tdo;
   ProfileWindow *profwin;

   //Contains the overview.
   Gtk::EventBox *eventboxtdo;

   Gtk::Window *tdowin;
   Gtk::Window *profilewindow;
   AdvancedOptionsWindow *aow;
   FileSaver *fs;

   // This variable prevents the image(s) from being drawn twice as a result 
   // of toggling a radio button (or similar), which deactivates (and therefore 
   // toggles again) another one in the same group. This variable must start 
   // as true, as the methods make it opposite before using it, so that things 
   // will de drawn after the second "toggling".
   bool drawwhentoggled;

   //Help for LAG.
   Gtk::Dialog *help;

   //Information about LAG.
   Gtk::AboutDialog *about;

   // Menu items
   Gtk::MenuItem* openfilemenuitem;
   Gtk::MenuItem* saveasfilemenuitem;
   Gtk::MenuItem* quitfilemenuitem;
   Gtk::MenuItem* helpmenu;
   Gtk::MenuItem* aboutmenu;

   // Check button determining whether the profile box is viewable on 
   // the 2d overview.
   Gtk::CheckMenuItem *showprofilecheck;

   // Check button determining whether the fence box is viewable on the 
   // 2d overview.
   Gtk::CheckMenuItem *showfencecheck;

   //Check button determining whether the distance scale is viewable 
   //on the 2d overview.
   Gtk::CheckMenuItem *showdistancescalecheck;

   //Check button determining whether the distance scale is viewable 
   //on the 2d overview.
   Gtk::CheckMenuItem *showlegendcheck;

   //Check button determining whether the distance scale is viewable on 
   //the 2d overview.
   Gtk::CheckMenuItem *reverseheightcheck;

   //Determines which flightline is to be raised above the others.
   Gtk::SpinButton *raiselineselect;

   //Determines whether any flightline at all is to be raised.
   Gtk::CheckMenuItem *raiselinecheckmenu;

   // Detemines whether the image is coloured.
   Gtk::RadioMenuItem* colourbynonemenu;
   Gtk::RadioMenuItem* brightnessbynonemenu;

   //Determines whether the image is coloured by intensity.
   Gtk::RadioMenuItem *colourbyintensitymenu;

   //Determines whether the image is coloured by height.
   Gtk::RadioMenuItem *colourbyheightmenu;

   //Determines whether the image is coloured by flightline.
   Gtk::RadioMenuItem *colourbyflightlinemenu;

   //Determines whether the image is coloured by classification.
   Gtk::RadioMenuItem *colourbyclassificationmenu;

   //Determines whether the image is coloured by return.
   Gtk::RadioMenuItem *colourbyreturnmenu;

   //Determines whether the image is shaded by intensity.
   Gtk::RadioMenuItem *brightnessbyintensitymenu;

   //Determines whether the image is shaded by height.
   Gtk::RadioMenuItem *brightnessbyheightmenu;

   //Toggle button determining whether mouse dragging selects the fence.
   Gtk::ToggleToolButton *fencetoggle;

   //Toggle button determining whether mouse dragging selects the profile.
   Gtk::ToggleToolButton *profiletoggle;

   //Toggle button determining whether or not to use a slanted selection.
   Gtk::ToggleToolButton *slantedrectshapetoggle;

   //Determines the width of the profile in metres.
   Gtk::SpinButton *slantwidthselect;

   //Determines the width of the points in the overview in pixels.
   Gtk::SpinButton *pointwidthselect;

   //Toggle button determining whether the ruler is viewable on the overview.
   Gtk::ToggleToolButton *rulertoggleover;

   Gtk::ToolButton* returnbutton;
   Gtk::ToolButton* advancedbutton;

   Gtk::ToolButton* refreshbutton;
   void on_refreshbuttonclicked();

   Gtk::ToolButton* saveasbutton;

   // Vertical slices
   Gtk::ToggleToolButton* slicebutton;
   Gtk::SpinButton* zminselect;
   Gtk::SpinButton* zmaxselect;


   Gtk::ToolButton* superzoombutton;
   void on_superzoomclicked();


   // Loads widgets from xml file
   void load_xml(const Glib::RefPtr<Gtk::Builder>&);

   // Connects signals
   void connect_signals();

   // When toggled, the profile box is shown on the 2d overview regardless of 
   // whether profiling mode is active.
   void on_showprofilecheck();

   //When toggled, the fence box is shown on the 2d overview regardless of 
   //whether fencing mode is active.
   void on_showfencecheck();

   //When toggled, the distance scale is shown on the 2d overview.
   void on_showdistancescalecheck();

   //When toggled, the legend is shown on the 2d overview.
   void on_showlegendcheck();

   //When toggled, the heights are reversed on the 2d overview.
   void on_reverseheightcheck();

   //If one of the colour radio menu items is selected (and, therefore, 
   //the others deselected) then set the values of the colour control 
   //variables in the overview to the values of the corresponding radio 
   //menu items.
   void on_colouractivated();

   //If one of the brightness radio menu items is selected (and, therefore, 
   //the others deselected) then set the values of the brightness control 
   //variables in the overview to the values of the corresponding radio 
   //menu items.
   void on_brightnessactivated();

   //This returns the overview to its original position.
   void on_returnbutton_clicked();

   //This changes the width of the points in pixels.
   void on_pointwidthselected();

   //When toggled, the profile view goes into rulering mode. When 
   //untoggled, rulering mode ends.
   void on_rulertoggleover();

   //Toggles whether clicking and dragging will select the fence in the 
   //overview.
   void on_fencetoggle();

   //When toggled, the 2d overview goes into profile selection mode. When 
   //untoggled, 2d overview goes out of profile selection mode and the profile 
   //parameters are sent to the profile area.
   void on_profiletoggle();

   //When toggled, this makes sure that the orthogonal toggle is in the 
   //opposite state and then sets the slanted shape on the overview and makes 
   //new profile and fence boundaries before redrawing the overview, possibly 
   //with the new boundaries for profile and/or fence displayed.
   void on_slantedrectshapetoggle();

   //When the value in the spinbutton for slanted shape width is changed, 
   //tell the 2d overview, then make the new slanted box and then draw it. 
   //This does NOT update the profile itself (or, at least, not yet) if the 
   //slanted box is for a profile. To update the profile after the width has 
   //been satisfactorily adjusted, the profiletoggle must be toggled and then 
   //untoggled.
   void on_slantwidthselected();

   //Opens the advanced options dialog.
   void on_advancedbutton_clicked();

   //Show the help dialog when respective menu item activated.
   void on_helpmenuactivated();

   //Hide the help dialog when close button activated.
   void on_helpresponse(int response_id);

   //Show the about dialog when respective menu item activated.
   void on_aboutmenuactivated();

   //Hide the about dialog when close button activated.
   void on_aboutresponse(int response_id);

   //When selected from the menu, the file chooser opens.
   void on_openfilemenuactivated();

   //When selected from the menu, the file saver opens.
   void on_savefilemenuactivated();

   //Intreprets keybaord signals from the EventBox.
   bool on_tdo_key_press(GdkEventKey* event);

   //Updates the line to be raised and redraws.
   void on_raiselineselected();

   //Updates whether any line is to be raised and redraws.
   void on_raiselinecheckmenu();

   //Slicing
   void on_slicebuttontoggled();
   void on_zminvaluechanged();
   void on_zmaxvaluechanged();

};
#endif
