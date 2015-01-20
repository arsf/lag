/*
 ===============================================================================

 AdvancedOptionsWindow.h

 Created on: June-July 2010
 Authors: Haraldur Tristan Gunnarsson, Jan Holownia

 LIDAR Analysis GUI (LAG), viewer for LIDAR files in .LAS or ASCII format
 Copyright (C) 2009-2012 Plymouth Marine Laboratory (PML)

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

#ifndef ADVANCEDOPTIONSWINDOW_H
#define ADVANCEDOPTIONSWINDOW_H

#include <gtkmm.h>
#include <vector>
#include "../TwoDeeOverview.h"
#include "../Profile.h"

/*
 ===============================================================================

 AdvancedOptionsWindow - represents a dialog with advanced options.

 ===============================================================================
 */
class AdvancedOptionsWindow
{
   public:
      AdvancedOptionsWindow(TwoDeeOverview*, Profile*, const Glib::RefPtr<Gtk::Builder>&);

      ~AdvancedOptionsWindow();

      void show()
      {
         advancedoptionsdialog->present();
      }
      
      void resetcolouringandshading()
      {
         on_drawingresetbutton_clicked();
      }
      
      bool getfractionalshift()
      {
         return fractionalshiftcheck->get_active();
      }
      
      double getmovespeed()
      {
         return movespeedselect->get_value();
      }
      
      void setmaindetailrange(double min, double max)
      {
         maindetailselect->set_range(min, max);
      }
      
   private:
      TwoDeeOverview *tdo;
      Profile *prof;

      //Advanced viewing options:
      
      //Dialog window for advanced options.
      Gtk::Dialog *advancedoptionsdialog;
      Gtk::SpinButton *movespeedselect;
      Gtk::CheckButton *fractionalshiftcheck;

      //False elevation:
      
      //Elevate classifications with the respective codes:
      Gtk::CheckButton *classcheckbutton0;
      Gtk::CheckButton *classcheckbutton2;
      Gtk::CheckButton *classcheckbutton3;
      Gtk::CheckButton *classcheckbutton4;
      Gtk::CheckButton *classcheckbutton5;
      Gtk::CheckButton *classcheckbutton6;
      Gtk::CheckButton *classcheckbutton7;
      Gtk::CheckButton *classcheckbutton8;
      Gtk::CheckButton *classcheckbutton9;
      Gtk::CheckButton *classcheckbutton12;

      //"Anything else" classification elevator.
      Gtk::CheckButton *classcheckbuttonA;
      Gtk::CheckButton *profdisplaynoise;
      Gtk::CheckButton *tdodisplaynoise;

      //Colouring and shading:
      
      //Determines the maximum height for colouring and shading.
      Gtk::SpinButton *heightmaxselect;

      //Determines the minimum height for colouring and shading.
      Gtk::SpinButton *heightminselect;

      // Used for "stepping through" the height range (absolute minimum to 
      // absolute maximum) with a page size defined by the difference between 
      // heightmaxselect and heightminselect.
      Gtk::HScrollbar *heightscrollbar;

      // This increases all brightness levels by a fixed amount, if based on 
      // height. In analogy to heightfloorselect, this raises both the floor 
      // and the ceiling.
      Gtk::SpinButton *heightoffsetselect;

      // This increases brightness levels by a varying amount such that the 
      // lowest level will be increased by the value and higher levels by by 
      // a declining value until the level of 1.0, which will not increase at 
      // all. In the analogy, the floor is raised but not the ceiling, and 
      // the "space" between them is "squashed".
      Gtk::SpinButton *heightfloorselect;

      //Equivalent as for height:
      Gtk::SpinButton *intensitymaxselect;
      Gtk::SpinButton *intensityminselect;
      Gtk::HScrollbar *intensityscrollbar;
      Gtk::SpinButton *intensityoffsetselect;
      Gtk::SpinButton *intensityfloorselect;

      //Resets all other colouring and shading widgets to initial values.
      Gtk::Button *drawingresetbutton;

      // Detail level (i.e. how many points to skip for each point shown, worked 
      // out INDIRECTLY; see the profile and overview classes for details):
      //Determines how many points are skipped displaying the main overview image.
      Gtk::SpinButton *maindetailselect;

      //Determines how many points are skipped displaying the main profile image.
      Gtk::SpinButton *maindetailselectprof;

      //Determines how many points are skipped displaying the profile preview.
      Gtk::SpinButton *previewdetailselectprof;

      Gtk::CheckButton *fullrefreshonpanning;
      Gtk::ColorButton *backgroundcolorbutton;

      //Signals:
      
      // These connections are for controlling the interaction of the height and 
      // intensity colouring and shading scrollbars and spinbuttons, particularly 
      // so that they do not interfere with each other at the wrong moment.
      sigc::connection heightminconn;
      sigc::connection heightmaxconn;
      sigc::connection intensityminconn;
      sigc::connection intensitymaxconn;

      void load_xml(const Glib::RefPtr<Gtk::Builder>&);
      void connect_signals();

      void on_fullrefresh_toggled();
      void on_backgroundcolor_changed();
      void on_advancedoptionsdialog_response(int response_id);

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

      void on_profile_noise_toggle();
      void on_tdo_noise_toggle();
      void changecoloursandshades();

      void on_heightmaxselect_changed();
      void on_heightminselect_changed();
      bool on_heightscrollbar_scrolled(Gtk::ScrollType scroll, double new_value);
      void on_heightoffsetselect_changed();
      void on_heightfloorselect_changed();
      void on_intensitymaxselect_changed();
      void on_intensityminselect_changed();
      bool on_intensityscrollbar_scrolled(Gtk::ScrollType scroll, double new_value);
      void on_intensityoffsetselect_changed();
      void on_intensityfloorselect_changed();

      // This resets the advanced colouring and shading options to the values 
      // indicated by the drawing objects.
      void on_drawingresetbutton_clicked();

      // This indirectly determines how many points are skipped when viewing the 
      // main overview image. I.e. this affects it as well as the number of 
      // visible buckets.
      void on_maindetailselected();

      // Does the same as on_maindetailselected, except for the profile.
      void on_maindetailselectedprof();

      // Does the same as on_maindetailselectedprof, except for the preview of 
      // the profile.
      void on_previewdetailselectedprof();

      // When toggled, the profile box is shown on the 2d overview regardless of 
      // whether profiling mode is active.
      void on_raiselinecheck();
};

#endif
