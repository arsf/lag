/*
 ==================================
 AdvancedLoadDialog.h

 Created on: 25 Jun 2012
 Author: jaho
 ==================================
 */

#ifndef ADVANCEDLOADDIALOG_H_
#define ADVANCEDLOADDIALOG_H_

#include <gtkmm.h>
#include "../PointFilter.h"
#include <iostream>

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

