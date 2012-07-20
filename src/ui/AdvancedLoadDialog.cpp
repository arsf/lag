/*
===============================================================================

 AdvancedLoadDialog.cpp

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

#include "AdvancedLoadDialog.h"


/*
==================================
 AdvancedLoadDialog::AdvancedLoadDialog
==================================
*/
AdvancedLoadDialog::AdvancedLoadDialog(const Glib::RefPtr<Gtk::Builder>& builder) :
	point_filter	()
{

	load_xml(builder);
	connect_signals();
}

/*
==================================
 AdvancedLoadDialog::~AdvancedLoadDialog
==================================
*/
AdvancedLoadDialog::~AdvancedLoadDialog()
{
	delete inside_rectangle_cb;
	delete inside_rectangle_min_x;
	delete inside_rectangle_min_y;
	delete inside_rectangle_max_x;
	delete inside_rectangle_max_y;

	delete inside_box_cb;
	delete inside_box_min_x;
	delete inside_box_min_y;
	delete inside_box_min_z;
	delete inside_box_max_x;
	delete inside_box_max_y;
	delete inside_box_max_z;

	delete inside_circle_cb;
	delete inside_circle_x;
	delete inside_circle_y;
	delete inside_circle_radius;

	delete clip_x_cb;
	delete clip_x_above;
	delete clip_x_below;

	delete clip_y_cb;
	delete clip_y_above;
	delete clip_y_below;

	delete clip_z_cb;
	delete clip_z_above;
	delete clip_z_below;

	delete return_cb;
	delete return_entry;

	delete intensity_cb;
	delete intensity_from;
	delete intensity_to;

	delete classification_cb;
	delete classification_entry;

	delete gps_time_cb;
	delete gps_time_from;
	delete gps_time_to;

	delete advanced_load_dialog;
}

/*
==================================
 AdvancedLoadDialog::load_xml
==================================
*/
void AdvancedLoadDialog::load_xml(const Glib::RefPtr<Gtk::Builder>& builder)
{
	builder->get_widget("loadadvanceddialog", advanced_load_dialog);

	builder->get_widget("insiderectanglecb", inside_rectangle_cb);
	builder->get_widget("insiderectangleminx", inside_rectangle_min_x);
	builder->get_widget("insiderectangleminy", inside_rectangle_min_y);
	builder->get_widget("insiderectanglemaxx", inside_rectangle_max_x);
	builder->get_widget("insiderectanglemaxy", inside_rectangle_max_y);

	builder->get_widget("insideboxcb", inside_box_cb);
	builder->get_widget("insideboxminx", inside_box_min_x);
	builder->get_widget("insideboxminy", inside_box_min_y);
	builder->get_widget("insideboxminz", inside_box_min_z);
	builder->get_widget("insideboxmaxx", inside_box_max_x);
	builder->get_widget("insideboxmaxy", inside_box_max_y);
	builder->get_widget("insideboxmaxz", inside_box_max_z);

	builder->get_widget("insidecirclecb", inside_circle_cb);
	builder->get_widget("insidecirclex", inside_circle_x);
	builder->get_widget("insidecircley", inside_circle_y);
	builder->get_widget("insidecircleradius", inside_circle_radius);

	builder->get_widget("clipxcb", clip_x_cb);
	builder->get_widget("clipxabove", clip_x_above);
	builder->get_widget("clipxbelow", clip_x_below);
	builder->get_widget("clipycb", clip_y_cb);
	builder->get_widget("clipyabove", clip_y_above);
	builder->get_widget("clipybelow", clip_y_below);
	builder->get_widget("clipzcb", clip_z_cb);
	builder->get_widget("clipzabove", clip_z_above);
	builder->get_widget("clipzbelow", clip_z_below);

	builder->get_widget("returncb", return_cb);
	builder->get_widget("returnentry", return_entry);
	builder->get_widget("returndropbtn", return_drop_btn);

	builder->get_widget("intensitycb", intensity_cb);
	builder->get_widget("intensityfrom", intensity_from);
	builder->get_widget("intensityto", intensity_to);
	builder->get_widget("intensitydropbtn", intensity_drop_btn);

	builder->get_widget("classificationcb", classification_cb);
	builder->get_widget("classificationentry", classification_entry);
	builder->get_widget("classificationdropbtn", classification_drop_btn);

	builder->get_widget("gpstimecb", gps_time_cb);
	builder->get_widget("gpstimefrom", gps_time_from);
	builder->get_widget("gpstimeto", gps_time_to);
	builder->get_widget("gpstimedropbtn", gps_time_drop_btn);

	builder->get_widget("loadadvancedok", ok_btn);
}

/*
==================================
 AdvancedLoadDialog::connect_signals
==================================
*/
void AdvancedLoadDialog::connect_signals()
{
	inside_rectangle_cb->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_inside_rectangle_cb_toggled));
	inside_box_cb->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_inside_box_cb_toggled));
	inside_circle_cb->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_inside_circle_cb_toggled));
	clip_x_cb->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_clip_x_cb_toggled));
	clip_y_cb->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_clip_y_cb_toggled));
	clip_z_cb->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_clip_z_cb_toggled));
	return_cb->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_return_cb_toggled));
	intensity_cb->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_intensity_cb_toggled));
	classification_cb->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_classification_cb_toggled));
	gps_time_cb->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_gps_time_cb_toggled));
	return_drop_btn->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_return_drop_btn_toggled));
	intensity_drop_btn->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_intensity_drop_btn_toggled));
	classification_drop_btn->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_classification_drop_btn_toggled));
	gps_time_drop_btn->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_gps_time_drop_btn_toggled));
	ok_btn->signal_clicked().connect(sigc::mem_fun(*this,&AdvancedLoadDialog::on_ok_btn_clicked));
}

/*
==================================
 AdvancedLoadDialog::on_inside_rectangle_cb_toggled
==================================
*/
void AdvancedLoadDialog::on_inside_rectangle_cb_toggled()
{
	bool toggled = inside_rectangle_cb->get_active();
	inside_rectangle_min_x->set_sensitive(toggled);
	inside_rectangle_min_y->set_sensitive(toggled);
	inside_rectangle_max_x->set_sensitive(toggled);
	inside_rectangle_max_y->set_sensitive(toggled);
}

/*
==================================
 AdvancedLoadDialog::on_inside_box_cb_toggled
==================================
*/
void AdvancedLoadDialog::on_inside_box_cb_toggled()
{
	bool toggled = inside_box_cb->get_active();
	inside_box_min_x->set_sensitive(toggled);
	inside_box_min_y->set_sensitive(toggled);
	inside_box_min_z->set_sensitive(toggled);
	inside_box_max_x->set_sensitive(toggled);
	inside_box_max_y->set_sensitive(toggled);
	inside_box_max_z->set_sensitive(toggled);
}

/*
==================================
 AdvancedLoadDialog::on_inside_circle_cb_toggled
==================================
*/
void AdvancedLoadDialog::on_inside_circle_cb_toggled()
{
	bool toggled = inside_circle_cb->get_active();
	inside_circle_x->set_sensitive(toggled);
	inside_circle_y->set_sensitive(toggled);
	inside_circle_radius->set_sensitive(toggled);
}

/*
==================================
 AdvancedLoadDialog::on_clip_x_cb_toggled
==================================
*/
void AdvancedLoadDialog::on_clip_x_cb_toggled()
{
	bool toggled = clip_x_cb->get_active();
	clip_x_above->set_sensitive(toggled);
	clip_x_below->set_sensitive(toggled);
}

/*
==================================
 AdvancedLoadDialog::on_clip_y_cb_toggled
==================================
*/
void AdvancedLoadDialog::on_clip_y_cb_toggled()
{
	bool toggled = clip_y_cb->get_active();
	clip_y_above->set_sensitive(toggled);
	clip_y_below->set_sensitive(toggled);
	clip_y_below->set_sensitive(toggled);
}

/*
==================================
 AdvancedLoadDialog::on_clip_z_cb_toggled
==================================
*/
void AdvancedLoadDialog::on_clip_z_cb_toggled()
{
	bool toggled = clip_z_cb->get_active();
	clip_z_above->set_sensitive(toggled);
	clip_z_below->set_sensitive(toggled);
}

/*
==================================
 AdvancedLoadDialog::on_return_cb_toggled
==================================
*/
void AdvancedLoadDialog::on_return_cb_toggled()
{
	bool toggled = return_cb->get_active();
	return_entry->set_sensitive(toggled);
	return_drop_btn->set_sensitive(toggled);
}

/*
==================================
 AdvancedLoadDialog::on_intensity_cb_toggled
==================================
*/
void AdvancedLoadDialog::on_intensity_cb_toggled()
{
	bool toggled = intensity_cb->get_active();
	intensity_from->set_sensitive(toggled);
	intensity_to->set_sensitive(toggled);
	intensity_drop_btn->set_sensitive(toggled);
}

/*
==================================
 AdvancedLoadDialog::on_classification_cb_toggled
==================================
*/
void AdvancedLoadDialog::on_classification_cb_toggled()
{
	bool toggled = classification_cb->get_active();
	classification_entry->set_sensitive(toggled);
	classification_drop_btn->set_sensitive(toggled);
}

/*
==================================
 AdvancedLoadDialog::on_gps_time_cb_toggled
==================================
*/
void AdvancedLoadDialog::on_gps_time_cb_toggled()
{
	bool toggled = gps_time_cb->get_active();
	gps_time_from->set_sensitive(toggled);
	gps_time_to->set_sensitive(toggled);
	gps_time_drop_btn->set_sensitive(toggled);
}

/*
==================================
 AdvancedLoadDialog::on_return_drop_btn_toggled
==================================
*/
void AdvancedLoadDialog::on_return_drop_btn_toggled()
{
	if (return_drop_btn->get_active())
		return_drop_btn->set_label("Drop");
	else
		return_drop_btn->set_label("Keep");
}

/*
==================================
 AdvancedLoadDialog::on_intensity_drop_btn_toggled
==================================
*/
void AdvancedLoadDialog::on_intensity_drop_btn_toggled()
{
	if (intensity_drop_btn->get_active())
			intensity_drop_btn->set_label("Drop");
		else
			intensity_drop_btn->set_label("Keep");
}

/*
==================================
 AdvancedLoadDialog::on_classification_drop_btn_toggled
==================================
*/
void AdvancedLoadDialog::on_classification_drop_btn_toggled()
{
	if (classification_drop_btn->get_active())
			classification_drop_btn->set_label("Drop");
		else
			classification_drop_btn->set_label("Keep");
}

/*
==================================
 AdvancedLoadDialog::on_gps_time_drop_btn_toggled
==================================
*/
void AdvancedLoadDialog::on_gps_time_drop_btn_toggled()
{
	if (gps_time_drop_btn->get_active())
			gps_time_drop_btn->set_label("Drop");
		else
			gps_time_drop_btn->set_label("Keep");
}

/*
==================================
 AdvancedLoadDialog::on_ok_btn_clicked

 This function builds a commandline-like string for
 a PointFilter, which is then passed to LASfilter.parse()
 method. If filters have not been set PointFilter::argc = 0.
==================================
*/
void AdvancedLoadDialog::on_ok_btn_clicked()
{
	point_filter.args.clear();

	// We need a dummy first element to emulate a command line
	point_filter.args.push_back("filter");

	if (inside_rectangle_cb->get_active())
	{
		point_filter.args.push_back("-clip ");
		point_filter.args.push_back(inside_rectangle_min_x->get_text());
		point_filter.args.push_back(inside_rectangle_min_y->get_text());
		point_filter.args.push_back(inside_rectangle_max_x->get_text());
		point_filter.args.push_back(inside_rectangle_max_y->get_text());
	}
	if (inside_circle_cb->get_active())
	{
		point_filter.args.push_back("-clip_circle");
		point_filter.args.push_back(inside_circle_x->get_text());
		point_filter.args.push_back(inside_circle_y->get_text());
		point_filter.args.push_back(inside_circle_radius->get_text());

	}
	if (inside_box_cb->get_active())
	{
		point_filter.args.push_back("-clip_box");
		point_filter.args.push_back(inside_box_min_x->get_text());
		point_filter.args.push_back(inside_box_min_y->get_text());
		point_filter.args.push_back(inside_box_min_z->get_text());
		point_filter.args.push_back(inside_box_max_x->get_text());
		point_filter.args.push_back(inside_box_max_y->get_text());
		point_filter.args.push_back(inside_box_max_z->get_text());
	}
	if (clip_x_cb->get_active())
	{
		if(clip_x_above->get_text() != "")
		{
			point_filter.args.push_back("-clip_x_above");
			point_filter.args.push_back(clip_x_above->get_text());
		}
		if(clip_x_below->get_text() != "")
		{
			point_filter.args.push_back("-clip_x_below");
			point_filter.args.push_back(clip_x_below->get_text());
		}
	}
	if (clip_y_cb->get_active())
	{
		if (clip_y_above->get_text() != "")
		{
			point_filter.args.push_back("-clip_y_above");
			point_filter.args.push_back(clip_y_above->get_text());
		}
		if (clip_y_below->get_text() != "")
		{
			point_filter.args.push_back("-clip_y_below");
			point_filter.args.push_back(clip_y_below->get_text());
		}
	}
	if (clip_z_cb->get_active())
	{
		if (clip_z_above->get_text() != "")
		{
			point_filter.args.push_back("-clip_z_above");
			point_filter.args.push_back(clip_z_above->get_text());
		}
		if (clip_z_below->get_text() != "")
		{
			point_filter.args.push_back("-clip_z_below");
			point_filter.args.push_back(clip_z_below->get_text());
		}
	}
	if (return_cb->get_active())
	{
		if(return_drop_btn->get_active())
		{
			point_filter.args.push_back("-drop_return");
		}
		else
		{
			point_filter.args.push_back("-keep_return");
		}

		point_filter.args.push_back(return_entry->get_text());
	}
	if (intensity_cb->get_active())
	{
		if(intensity_drop_btn->get_active())
		{
			if(intensity_from->get_text() != "")
			{
				point_filter.args.push_back("-drop_intensity_between");
				point_filter.args.push_back(intensity_from->get_text());
				point_filter.args.push_back(intensity_to->get_text());
			}
		}
		else
		{
			point_filter.args.push_back("-keep_intensity");
			point_filter.args.push_back(intensity_from->get_text());
			point_filter.args.push_back(intensity_to->get_text());
		}
	}
	if (classification_cb->get_active())
	{
		std::string entry = classification_entry->get_text();
		if(classification_drop_btn->get_active())
		{
			point_filter.args.push_back("-drop_class");
			point_filter.args.push_back(classification_entry->get_text());
		}
		else
		{
			point_filter.args.push_back("-keep_class");
			point_filter.args.push_back(classification_entry->get_text());
		}
	}
	if (gps_time_cb->get_active())
	{
		if(gps_time_drop_btn->get_active())
		{
			point_filter.args.push_back("-drop_gps_time_between");
			point_filter.args.push_back(gps_time_from->get_text());
			point_filter.args.push_back(gps_time_to->get_text());
		}
		else
		{
			point_filter.args.push_back("-keep_gps_time");
			point_filter.args.push_back(gps_time_from->get_text());
			point_filter.args.push_back(gps_time_to->get_text());
		}
	}

	point_filter.argc = point_filter.args.size() > 1 ? point_filter.args.size() : 0;

	advanced_load_dialog->hide_all();
}

