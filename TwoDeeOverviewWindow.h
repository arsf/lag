/*
 * File: TwoDeeOverviewWindow.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: June 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <vector>
#include "TwoDeeOverview.h"
#include "AdvancedOptionsWindow.h"
#include "FileSaver.h"
#include "ProfileWindow.h"
#ifndef TWODEEOVERVIEWWINDOW_H
#define TWODEEOVERVIEWWINDOW_H

class TwoDeeOverviewWindow{
public:
   TwoDeeOverviewWindow(TwoDeeOverview *tdo,AdvancedOptionsWindow *aow,FileSaver *fs,Gtk::Window *overviewwindow,Gtk::Window *profilewindow,Glib::RefPtr<Gnome::Glade::Xml> refXml,Gtk::EventBox *eventboxtdo,ProfileWindow *profwin);
   ~TwoDeeOverviewWindow();
   void setraiselinerange(int first, int second){ raiselineselect->set_range(first,second); }
protected:
   TwoDeeOverview *tdo;
   ProfileWindow *profwin;
   Gtk::EventBox *eventboxtdo;//Contains the overview.
   Gtk::Window *overviewwindow;
   Gtk::Window *profilewindow;
   AdvancedOptionsWindow *aow;
   FileSaver *fs;
   bool drawwhentoggled;//This variable prevents the image(s) from being drawn twice as a result of toggling a radio button (or similar), which deactivates (and therefore toggles again) another one in the same group. This variable must start as true, as the methods make it opposite before using it, so that things will de drawn after the second "toggling".
   Gtk::AboutDialog *about;//Information about LAG.
   Gtk::CheckMenuItem *showprofilecheck;//Check button determining whether the profile box is viewable on the 2d overview.
   Gtk::CheckMenuItem *showfencecheck;//Check button determining whether the fence box is viewable on the 2d overview.
   Gtk::CheckMenuItem *showdistancescalecheck;//Check button determining whether the distance scale is viewable on the 2d overview.
   Gtk::CheckMenuItem *showlegendcheck;//Check button determining whether the distance scale is viewable on the 2d overview.
   Gtk::CheckMenuItem *reverseheightcheck;//Check button determining whether the distance scale is viewable on the 2d overview.
   Gtk::SpinButton *raiselineselect;
   Gtk::CheckMenuItem *raiselinecheckmenu;
   Gtk::RadioMenuItem *colourbyintensitymenu;//Determines whether the image is coloured by intensity.
   Gtk::RadioMenuItem *colourbyheightmenu;//Determines whether the image is coloured by height.
   Gtk::RadioMenuItem *colourbyflightlinemenu;//Determines whether the image is coloured by flightline.
   Gtk::RadioMenuItem *colourbyclassificationmenu;//Determines whether the image is coloured by classification.
   Gtk::RadioMenuItem *colourbyreturnmenu;//Determines whether the image is coloured by return.
   Gtk::RadioMenuItem *brightnessbyintensitymenu;//Determines whether the image is shaded by intensity.
   Gtk::RadioMenuItem *brightnessbyheightmenu;//Determines whether the image is shaded by height.
   Gtk::ToggleToolButton *fencetoggle;//Toggle button determining whether mouse dragging selects the fence.
   Gtk::ToggleToolButton *profiletoggle;//Toggle button determining whether mouse dragging selects the profile.
   Gtk::ToggleToolButton *orthogonalrectshapetoggle;//Toggle button determining whether or not to use an orthogonal selection.
   Gtk::ToggleToolButton *slantedrectshapetoggle;//Toggle button determining whether or not to use a slanted selection.
   Gtk::SpinButton *slantwidthselect;//Determines the width of the profile in metres.
   Gtk::SpinButton *pointwidthselect;//Determines the width of the points in the overview in pixels.
   Gtk::ToggleToolButton *rulertoggleover;//Toggle button determining whether the ruler is viewable on the overview.
   
   //When toggled, the profile box is shown on the 2d overview regardless of whether profiling mode is active.
   void on_showprofilecheck();
   //When toggled, the fence box is shown on the 2d overview regardless of whether fencing mode is active.
   void on_showfencecheck();
   //When toggled, the distance scale is shown on the 2d overview.
   void on_showdistancescalecheck();
   //When toggled, the legend is shown on the 2d overview.
   void on_showlegendcheck();
   //When toggled, the heights are reversed on the 2d overview.
   void on_reverseheightcheck();
   //If one of the colour radio menu items is selected (and, therefore, the others deselected) then set the values of the colour control variables in the overview to the values of the corresponding radio menu items.
   void on_colouractivated();
   //If one of the brightness radio menu items is selected (and, therefore, the others deselected) then set the values of the brightness control variables in the overview to the values of the corresponding radio menu items.
   void on_brightnessactivated();

   //This returns the overview to its original position.
   void on_returnbutton_clicked();
   //This changes the width of the points in pixels.
   void on_pointwidthselected();
   //When toggled, the profile view goes into rulering mode. When untoggled, rulering mode ends.
   void on_rulertoggleover();
   //Toggles whether clicking and dragging will select the fence in the overview.
   void on_fencetoggle();
   //When toggled, the 2d overview goes into profile selection mode. When untoggled, 2d overview goes out of profile selection mode and the profile parameters are sent to the profile area.
   void on_profiletoggle();
   //When toggled, this makes sure that the slant toggle is in the opposite state and then sets the orthogonal shape on the overview and makes new profile and fence boundaries before redrawing the overview, possibly with the new boundaries for profile and/or fence displayed.
   void on_orthogonalrectshapetoggle();
   //When toggled, this makes sure that the orthogonal toggle is in the opposite state and then sets the slanted shape on the overview and makes new profile and fence boundaries before redrawing the overview, possibly with the new boundaries for profile and/or fence displayed.
   void on_slantedrectshapetoggle();
   //When the value in the spinbutton for slanted shape width is changed, tell the 2d overview, then make the new slanted box and then draw it. This does NOT update the profile itself (or, at least, not yet) if the slanted box is for a profile. To update the profile after the width has been satisfactorily adjusted, the profiletoggle must be toggled and then untoggled.
   void on_slantwidthselected();
   //Opens the advanced options dialog.
   void on_advancedbutton_clicked();
   //Show the about dialog when respective menu item activated.
   void on_aboutmenuactivated();
   //Hide the about dialog when close button activated.
   void on_aboutresponse(int response_id);

   //When selected from the menu, the file saver opens.
   void on_savefilemenuactivated();

   bool on_tdo_key_press(GdkEventKey* event);

   void on_raiselineselected();
   void on_raiselinecheckmenu();

};
#endif
