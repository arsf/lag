/*
 * File: AdvancedOptionsWindow.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: June 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <vector>
#include "TwoDeeOverview.h"
#include "Profile.h"
#ifndef ADVANCEDOPTIONSWINDOW_H
#define ADVANCEDOPTIONSWINDOW_H
class AdvancedOptionsWindow{
public:
   AdvancedOptionsWindow(TwoDeeOverview *tdo,Profile *prof,Glib::RefPtr<Gnome::Glade::Xml> refXml);
   ~AdvancedOptionsWindow();
   void show(){ advancedoptionsdialog->present(); }
   void resetcolouringandshading(){ on_drawingresetbutton_clicked(); }
protected:
   TwoDeeOverview *tdo;
   Profile *prof;
   //Advanced viewing options:
   Gtk::Dialog *advancedoptionsdialog;//Dialog window for advanced options.
   //False elevation:
   Gtk::CheckButton *classcheckbutton0;//Elevate classifications with the respective codes:
   Gtk::CheckButton *classcheckbutton2;//...
   Gtk::CheckButton *classcheckbutton3;//...
   Gtk::CheckButton *classcheckbutton4;//...
   Gtk::CheckButton *classcheckbutton5;//...
   Gtk::CheckButton *classcheckbutton6;//...
   Gtk::CheckButton *classcheckbutton7;//...
   Gtk::CheckButton *classcheckbutton8;//...
   Gtk::CheckButton *classcheckbutton9;//...
   Gtk::CheckButton *classcheckbutton12;//...
   Gtk::CheckButton *classcheckbuttonA;//"Anything else" classification elevator.
   //Colouring and shading:
   Gtk::SpinButton *heightmaxselect;//Determines the maximum height for colouring and shading.
   Gtk::SpinButton *heightminselect;//Determines the minimum height for colouring and shading.
   Gtk::HScrollbar *heightscrollbar;//Used for "stepping through" the height range (absolute minimum to absolute maximum) with a page size defined by the difference between heightmaxselect and heightminselect.
   Gtk::SpinButton *heightoffsetselect;//This increases all brightness levels by a fixed amount, if based on height. In analogy to heightfloorselect, this raises both the floor and the ceiling.
   Gtk::SpinButton *heightfloorselect;//This increases brightness levels by a varying amount such that the lowest level will be increased by the value and higher levels by by a declining value until the level of 1.0, which will not increase at all. In the analogy, the floor is raised but not the ceiling, and the "space" between them is "squashed".
   Gtk::SpinButton *intensitymaxselect;//Equivalent as for height:
   Gtk::SpinButton *intensityminselect;//...
   Gtk::HScrollbar *intensityscrollbar;//...
   Gtk::SpinButton *intensityoffsetselect;//...
   Gtk::SpinButton *intensityfloorselect;//...
   Gtk::Button *drawingresetbutton;//Resets all other colouring and shading widgets to initial values.
   //Detail level (i.e. how many points to skip for each point shown, worked out INDIRECTLY; see the profile and overview classes for details):
   Gtk::SpinButton *maindetailselect;//Determines how many points are skipped displaying the main overview image.
   Gtk::SpinButton *maindetailselectprof;//Determines how many points are skipped displaying the main profile image.
   Gtk::SpinButton *previewdetailselectprof;//Determines how many points are skipped displaying the profile preview.

   //Signals:
   sigc::connection heightminconn;//These connections are for controlling the interaction of the height and intensity colouring and shading scrollbars and spinbuttons, particularly so that they do not interfere with each other at the wrong moment.
   sigc::connection heightmaxconn;//...
   sigc::connection intensityminconn;//...
   sigc::connection intensitymaxconn;//...

   //Closes the advanced options dialog.
   void on_advancedoptionsdialog_response(int response_id);
   //The checkbuttons and their activations:
   void on_classcheckbutton0_toggled();
   void on_classcheckbutton2_toggled();
   void on_classcheckbutton3_toggled();
   void on_classcheckbutton4_toggled();
   void on_classcheckbutton5_toggled();
   void on_classcheckbutton6_toggled();
   void on_classcheckbutton7_toggled();
   void on_classcheckbutton8_toggled();
   void on_classcheckbutton9_toggled();
   void on_classcheckbutton12_toggled();
   void on_classcheckbuttonA_toggled();
   //The drawing settings:
   void changecoloursandshades();
   void on_heightmaxselect_changed();
   void on_heightminselect_changed();
   bool on_heightscrollbar_scrolled(Gtk::ScrollType scroll,double new_value);
   void on_heightoffsetselect_changed();
   void on_heightfloorselect_changed();
   void on_intensitymaxselect_changed();
   void on_intensityminselect_changed();
   bool on_intensityscrollbar_scrolled(Gtk::ScrollType scroll,double new_value);
   void on_intensityoffsetselect_changed();
   void on_intensityfloorselect_changed();
   //This resets the advanced colouring and shading options to the values indicated by the drawing objects.
   void on_drawingresetbutton_clicked();
   //This indirectly determines how many points are skipped when viewing the main overview image. I.e. this affects it as well as the number of visible buckets.
   void on_maindetailselected();
   //Does the same as on_maindetailselected, except for the profile.
   void on_maindetailselectedprof();
   //Does the same as on_maindetailselectedprof, except for the preview of the profile.
   void on_previewdetailselectedprof();
   //When toggled, the profile box is shown on the 2d overview regardless of whether profiling mode is active.
   void on_raiselinecheck();
};

#endif
