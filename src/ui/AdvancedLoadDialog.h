/*
 ===============================================================================

 AdvancedLoadDialog.h

 Created on: 25 Jun 2012
 Author: Jan Holownia

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

#ifndef ADVANCEDLOADDIALOG_H_
#define ADVANCEDLOADDIALOG_H_

#include <iostream>
#include <gtkmm.h>
#include "../PointFilter.h"

/*
 ===============================================================================

 AdvancedLoadDialog - represents a dialog with advanced load options (filters
 and quadtree options).

 ===============================================================================
 */
class AdvancedLoadDialog
{
   public:
      AdvancedLoadDialog(const Glib::RefPtr<Gtk::Builder>&);
      ~AdvancedLoadDialog();

      PointFilter get_point_filter()
      {
         return this->point_filter;
      }
      
   private:
      PointFilter point_filter;
      std::string arg;

      Gtk::Dialog* advanced_load_dialog;

      Gtk::CheckButton* inside_rectangle_cb;
      Gtk::Entry* inside_rectangle_min_x;
      Gtk::Entry* inside_rectangle_min_y;
      Gtk::Entry* inside_rectangle_max_x;
      Gtk::Entry* inside_rectangle_max_y;

      Gtk::CheckButton* inside_box_cb;
      Gtk::Entry* inside_box_min_x;
      Gtk::Entry* inside_box_min_y;
      Gtk::Entry* inside_box_min_z;
      Gtk::Entry* inside_box_max_x;
      Gtk::Entry* inside_box_max_y;
      Gtk::Entry* inside_box_max_z;

      Gtk::CheckButton* inside_circle_cb;
      Gtk::Entry* inside_circle_x;
      Gtk::Entry* inside_circle_y;
      Gtk::Entry* inside_circle_radius;

      Gtk::CheckButton* clip_x_cb;
      Gtk::Entry* clip_x_above;
      Gtk::Entry* clip_x_below;

      Gtk::CheckButton* clip_y_cb;
      Gtk::Entry* clip_y_above;
      Gtk::Entry* clip_y_below;

      Gtk::CheckButton* clip_z_cb;
      Gtk::Entry* clip_z_above;
      Gtk::Entry* clip_z_below;

      Gtk::CheckButton* return_cb;
      Gtk::Entry* return_entry;
      Gtk::ToggleButton* return_drop_btn;

      Gtk::CheckButton* intensity_cb;
      Gtk::Entry* intensity_from;
      Gtk::Entry* intensity_to;
      Gtk::ToggleButton* intensity_drop_btn;

      Gtk::CheckButton* classification_cb;
      Gtk::Entry* classification_entry;
      Gtk::ToggleButton* classification_drop_btn;

      Gtk::CheckButton* gps_time_cb;
      Gtk::Entry* gps_time_from;
      Gtk::Entry* gps_time_to;
      Gtk::ToggleButton* gps_time_drop_btn;

      Gtk::Button* ok_btn;

      void load_xml(const Glib::RefPtr<Gtk::Builder>&);
      void connect_signals();
      void on_inside_rectangle_cb_toggled();
      void on_inside_box_cb_toggled();
      void on_inside_circle_cb_toggled();
      void on_clip_x_cb_toggled();
      void on_clip_y_cb_toggled();
      void on_clip_z_cb_toggled();
      void on_return_cb_toggled();
      void on_intensity_cb_toggled();
      void on_classification_cb_toggled();
      void on_gps_time_cb_toggled();
      void on_return_drop_btn_toggled();
      void on_intensity_drop_btn_toggled();
      void on_classification_drop_btn_toggled();
      void on_gps_time_drop_btn_toggled();
      void on_ok_btn_clicked();
};
#endif /* ADVANCEDLOADDIALOG_H_ */

