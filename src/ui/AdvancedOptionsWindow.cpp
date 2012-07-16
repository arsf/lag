/*
===============================================================================

 AdvancedLoadDialog.h

 Created on: 25 Jun 2012
 Authors: Jan Holownia

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
#include "../TwoDeeOverview.h"
#include "../Profile.h"
#include "AdvancedOptionsWindow.h"


/*
==================================
 AdvancedOptionsWindow::AdvancedOptionsWindow
==================================
*/
AdvancedOptionsWindow::AdvancedOptionsWindow(TwoDeeOverview *tdo, Profile *prof, const Glib::RefPtr<Gtk::Builder>& builder)
:
		tdo		(tdo),
		prof	(prof)
{
	load_xml(builder);
	connect_signals();

	movespeedselect->set_value(10.00);
    maindetailselect->set_value(1.50);
    previewdetailselectprof->set_value(2.0);

    tdo->setmaindetail(maindetailselect->get_value());
    prof->setmaindetail(maindetailselectprof->get_value());
    prof->setpreviewdetail(previewdetailselectprof->get_value());
}

/*
==================================
 AdvancedOptionsWindow::~AdvancedOptionsWindow
==================================
*/
AdvancedOptionsWindow::~AdvancedOptionsWindow()
{
   delete fullrefreshonpanning;
   delete backgroundcolorbutton;
   delete classcheckbutton0;
   delete classcheckbutton2;
   delete classcheckbutton3;
   delete classcheckbutton4;
   delete classcheckbutton5;
   delete classcheckbutton6;
   delete classcheckbutton7;
   delete classcheckbutton8;
   delete classcheckbutton9;
   delete classcheckbutton12;
   delete classcheckbuttonA;
   delete heightmaxselect;
   delete heightminselect;
   delete heightscrollbar;
   delete heightoffsetselect;
   delete heightfloorselect;
   delete intensitymaxselect;
   delete intensityminselect;
   delete intensityscrollbar;
   delete intensityoffsetselect;
   delete intensityfloorselect;
   delete drawingresetbutton;
   delete maindetailselect;
   delete maindetailselectprof;
   delete previewdetailselectprof;
   delete advancedoptionsdialog;
}

/*
==================================
 AdvancedOptionsWindow::load_xml
==================================
*/
void AdvancedOptionsWindow::load_xml(const Glib::RefPtr<Gtk::Builder>& builder)
{
	builder->get_widget("advancedoptionsdialog",advancedoptionsdialog);
	builder->get_widget("fractionalshiftcheck",fractionalshiftcheck);
	builder->get_widget("movespeedselect",movespeedselect);
	builder->get_widget("classcheckbutton0",classcheckbutton0);
	builder->get_widget("classcheckbutton2",classcheckbutton2);
	builder->get_widget("classcheckbutton3",classcheckbutton3);
	builder->get_widget("classcheckbutton4",classcheckbutton4);
	builder->get_widget("classcheckbutton5",classcheckbutton5);
	builder->get_widget("classcheckbutton6",classcheckbutton6);
	builder->get_widget("classcheckbutton7",classcheckbutton7);
	builder->get_widget("classcheckbutton8",classcheckbutton8);
	builder->get_widget("classcheckbutton9",classcheckbutton9);
	builder->get_widget("classcheckbutton12",classcheckbutton12);
	builder->get_widget("classcheckbuttonA",classcheckbuttonA);
	builder->get_widget("heightmaxselect",heightmaxselect);
	builder->get_widget("heightminselect",heightminselect);
	builder->get_widget("heightscrollbar",heightscrollbar);
	builder->get_widget("heightoffsetselect",heightoffsetselect);
	builder->get_widget("heightfloorselect",heightfloorselect);
	builder->get_widget("intensitymaxselect",intensitymaxselect);
	builder->get_widget("intensityminselect",intensityminselect);
	builder->get_widget("intensityscrollbar",intensityscrollbar);
	builder->get_widget("intensityoffsetselect",intensityoffsetselect);
	builder->get_widget("intensityfloorselect",intensityfloorselect);
	builder->get_widget("drawingresetbutton",drawingresetbutton);
	builder->get_widget("maindetailselect",maindetailselect);
	builder->get_widget("maindetailselectprof",maindetailselectprof);
	builder->get_widget("previewdetailselectprof",previewdetailselectprof);
	builder->get_widget("profdisplaynoise", profdisplaynoise);
	builder->get_widget("tdodisplaynoise", tdodisplaynoise);
	builder->get_widget("fullrefreshonpanning", fullrefreshonpanning);
	builder->get_widget("backgroundcolorbutton", backgroundcolorbutton);
}

/*
==================================
 AdvancedOptionsWindow::connect_signals
==================================
*/
void AdvancedOptionsWindow::connect_signals()
{
	advancedoptionsdialog->signal_response().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_advancedoptionsdialog_response));
	classcheckbutton0->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton0_toggled));
	classcheckbutton2->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton2_toggled));
	classcheckbutton3->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton3_toggled));
	classcheckbutton4->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton4_toggled));
	classcheckbutton5->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton5_toggled));
	classcheckbutton6->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton6_toggled));
	classcheckbutton7->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton7_toggled));
	classcheckbutton8->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton8_toggled));
	classcheckbutton9->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton9_toggled));
	classcheckbutton12->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton12_toggled));
	classcheckbuttonA->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbuttonA_toggled));
	heightmaxconn = heightmaxselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_heightmaxselect_changed));
	heightminconn = heightminselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_heightminselect_changed));
	heightscrollbar->signal_change_value().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_heightscrollbar_scrolled));
	heightoffsetselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_heightoffsetselect_changed));
	heightfloorselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_heightfloorselect_changed));
	intensitymaxconn = intensitymaxselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_intensitymaxselect_changed));
	intensityminconn = intensityminselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_intensityminselect_changed));
	intensityscrollbar->signal_change_value().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_intensityscrollbar_scrolled));
	intensityoffsetselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_intensityoffsetselect_changed));
	intensityfloorselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_intensityfloorselect_changed));
	drawingresetbutton->signal_clicked().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_drawingresetbutton_clicked));
	maindetailselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_maindetailselected));
	maindetailselectprof->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_maindetailselectedprof));
	previewdetailselectprof->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_previewdetailselectedprof));
	profdisplaynoise->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_profile_noise_toggle));
	tdodisplaynoise->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_tdo_noise_toggle));
	fullrefreshonpanning->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_fullrefresh_toggled));
	backgroundcolorbutton->signal_color_set().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_backgroundcolor_changed));
}

/*
==================================
 AdvancedOptionsWindow::on_advancedoptionsdialog_response

 Closes the advanced options dialog.
==================================
*/
void AdvancedOptionsWindow::on_advancedoptionsdialog_response(int response_id)
{
   response_id=0;
   advancedoptionsdialog->hide(); 
}

void AdvancedOptionsWindow::on_fullrefresh_toggled()
{
	if(fullrefreshonpanning->get_active())
		tdo->set_panning_refresh(1);
	else
		tdo->set_panning_refresh(2);
}

void AdvancedOptionsWindow::on_classcheckbutton0_toggled()
{
   tdo->setheightenNonC(classcheckbutton0->get_active()); 
   if(tdo->is_realized())
      tdo->drawviewable(1); 
}

void AdvancedOptionsWindow::on_classcheckbutton2_toggled()
{
   tdo->setheightenGround(classcheckbutton2->get_active()); 
   if(tdo->is_realized())
      tdo->drawviewable(1); 
}

void AdvancedOptionsWindow::on_classcheckbutton3_toggled()
{
   tdo->setheightenLowVeg(classcheckbutton3->get_active()); 
   if(tdo->is_realized())
      tdo->drawviewable(1); 
}

void AdvancedOptionsWindow::on_classcheckbutton4_toggled()
{
   tdo->setheightenMedVeg(classcheckbutton4->get_active()); 
   if(tdo->is_realized())
      tdo->drawviewable(1); 
}

void AdvancedOptionsWindow::on_classcheckbutton5_toggled()
{
   tdo->setheightenHighVeg(classcheckbutton5->get_active()); 
   if(tdo->is_realized())
      tdo->drawviewable(1); 
}

void AdvancedOptionsWindow::on_classcheckbutton6_toggled()
{
   tdo->setheightenBuildings(classcheckbutton6->get_active()); 
      if(tdo->is_realized())
         tdo->drawviewable(1); 
}

void AdvancedOptionsWindow::on_classcheckbutton7_toggled()
{
   tdo->setheightenNoise(classcheckbutton7->get_active()); 
   if(tdo->is_realized())
      tdo->drawviewable(1); 
}

void AdvancedOptionsWindow::on_classcheckbutton8_toggled()
{
   tdo->setheightenMass(classcheckbutton8->get_active()); 
   if(tdo->is_realized())
      tdo->drawviewable(1); 
}

void AdvancedOptionsWindow::on_classcheckbutton9_toggled()
{
   tdo->setheightenWater(classcheckbutton9->get_active()); 
   if(tdo->is_realized())
      tdo->drawviewable(1); 
}

void AdvancedOptionsWindow::on_classcheckbutton12_toggled()
{
   tdo->setheightenOverlap(classcheckbutton12->get_active()); 
   if(tdo->is_realized())
      tdo->drawviewable(1); 
}

void AdvancedOptionsWindow::on_classcheckbuttonA_toggled()
{
   tdo->setheightenUndefined(classcheckbuttonA->get_active()); 
   if(tdo->is_realized())
      tdo->drawviewable(1); 
}

void AdvancedOptionsWindow::on_profile_noise_toggle()
{
   prof->toggleNoise();
}

void AdvancedOptionsWindow::on_tdo_noise_toggle()
{
   tdo->toggleNoise();
}

// The drawing settings:
// =====================

void AdvancedOptionsWindow::changecoloursandshades()
{
   /*
    * Please note that there is a reason why the profile is updated before the
    * overview: if it is the other way around then the overview's drawing 
    * thread would be running so it will be unpredictable which part will 
    * execute opengl code first, which can sometimes mean that the overview 
    * will be drawn the same size as the profile, which might confuse users:
    */

   if(prof->is_realized())
      prof->coloursandshades(heightmaxselect->get_value(),
                             heightminselect->get_value(),
                             intensitymaxselect->get_value_as_int(),
                             intensityminselect->get_value_as_int());

   if(prof->is_realized())
      prof->drawviewable(1);

   if(tdo->is_realized())
      tdo->coloursandshades(heightmaxselect->get_value(),
                            heightminselect->get_value(),
                            intensitymaxselect->get_value_as_int(),
                            intensityminselect->get_value_as_int());

   if(tdo->is_realized())
      tdo->drawviewable(1);
}

void AdvancedOptionsWindow::on_heightmaxselect_changed()
{
   heightminselect->set_range(tdo->getrminz(), heightmaxselect->get_value()-0.01);
   
   //Want the scrollbar's slider length to correlate with the range we define.
   heightscrollbar->get_adjustment()->set_page_size(heightmaxselect->get_value() - heightminselect->get_value());
   heightscrollbar->set_increments(1, heightmaxselect->get_value() - heightminselect->get_value());

   changecoloursandshades();
}

void AdvancedOptionsWindow::on_heightminselect_changed()
{
   heightmaxselect->set_range(heightminselect->get_value()+0.01,
                              tdo->getrmaxz());
   // Value for the scrollbar only changed here as upper value is defined by 
   // this value and the page size.
   heightscrollbar->set_value(heightminselect->get_value());
   
   //Want the scrollbar's slider length to correlate with the range we define.
   heightscrollbar->get_adjustment()->
      set_page_size(heightmaxselect->get_value() - 
                    heightminselect->get_value());

   heightscrollbar->set_increments(1, heightmaxselect->get_value() - heightminselect->get_value());

   changecoloursandshades();
}

bool AdvancedOptionsWindow::on_heightscrollbar_scrolled(Gtk::ScrollType scroll,double new_value)
{
   if (scroll==0)
   {
   }

   if(new_value + heightscrollbar->get_adjustment()->get_page_size() > heightscrollbar->get_adjustment()->get_upper())
   // New upper value (new_value plus page size) must not exceed the maximum 
   // possible value, otherwise it might mess things up when used to set the 
   // ranges, below.     
      new_value = heightscrollbar->get_adjustment()->get_upper() - heightscrollbar->get_adjustment()->get_page_size();

   if(new_value == heightminselect->get_value())
      return true;

   // Letting these signals continue would complicate things significantly, as 
   // they would then try to set this scrollbar's properties.
   heightmaxconn.block();
   heightminconn.block();
   //Set the new ranges first so that the old ranges cannot clamp the new values.
   heightmaxselect->set_range(new_value+0.01,tdo->getrmaxz());
   heightminselect->set_range(tdo->getrminz(),new_value + heightscrollbar->get_adjustment()->get_page_size()-0.01);
   heightmaxselect->set_value(new_value + heightscrollbar->get_adjustment()->get_page_size());
   heightminselect->set_value(new_value);
   heightmaxconn.unblock();
   heightminconn.unblock();

   changecoloursandshades();

   return true;
}

void AdvancedOptionsWindow::on_heightoffsetselect_changed()
{
   tdo->setzoffset(heightoffsetselect->get_value());
   prof->setzoffset(heightoffsetselect->get_value());
   changecoloursandshades();
}

void AdvancedOptionsWindow::on_heightfloorselect_changed()
{
   tdo->setzfloor(heightfloorselect->get_value());
   prof->setzfloor(heightfloorselect->get_value());
   changecoloursandshades();
}

void AdvancedOptionsWindow::on_intensitymaxselect_changed()
{
   intensityminselect->set_range(tdo->getrminintensity(), intensitymaxselect->get_value()-1);

   //Want the scrollbar's slider length to correlate with the range we define.
   intensityscrollbar->get_adjustment()->set_page_size(intensitymaxselect->get_value() - intensityminselect->get_value());
   intensityscrollbar->set_increments(1, intensitymaxselect->get_value() - intensityminselect->get_value());

   changecoloursandshades();
}

void AdvancedOptionsWindow::on_intensityminselect_changed()
{
   intensitymaxselect->set_range(intensityminselect->get_value()+1,tdo->getrmaxintensity());

   // Value for the scrollbar only changed here as upper value is defined by 
   // this value and the page size.
   intensityscrollbar->set_value(intensityminselect->get_value());

   //Want the scrollbar's slider length to correlate with the range we define.
   intensityscrollbar->get_adjustment()->set_page_size(intensitymaxselect->get_value() - intensityminselect->get_value());
   intensityscrollbar->set_increments(1,intensitymaxselect->get_value() - intensityminselect->get_value());

   changecoloursandshades();
}

bool AdvancedOptionsWindow::on_intensityscrollbar_scrolled(Gtk::ScrollType scroll, double new_value)
{
   if(scroll==0)
   {
   }

   if(new_value + intensityscrollbar->get_adjustment()->get_page_size() > intensityscrollbar->get_adjustment()->get_upper())
   {
	  // New upper value (new_value plus page size) must not exceed the maximum
      // possible value, otherwise it might mess things up when used to set the 
      // ranges, below.
      new_value = intensityscrollbar->get_adjustment()->get_upper() - intensityscrollbar->get_adjustment()->get_page_size();
   }

   if(new_value == intensityminselect->get_value())
      return true;

   // Letting these signals continue would complicate things significantly, as 
   // they would then try to set this scrollbar's properties.
   intensitymaxconn.block();
   intensityminconn.block();

   //Set the new ranges first so that the old ranges cannot clamp the new values.
   intensitymaxselect->set_range(intensityminselect->get_value()+1, tdo->getrmaxintensity());
   intensityminselect->set_range(tdo->getrminintensity(), intensitymaxselect->get_value()-1);
   intensitymaxselect->set_value(new_value + intensityscrollbar->get_adjustment()->get_page_size());
   intensityminselect->set_value(new_value);
   intensitymaxconn.unblock();
   intensityminconn.unblock();

   changecoloursandshades();

   return true;
}

void AdvancedOptionsWindow::on_intensityoffsetselect_changed()
{
   tdo->setintensityoffset(intensityoffsetselect->get_value());
   prof->setintensityoffset(intensityoffsetselect->get_value());
   changecoloursandshades();
}

void AdvancedOptionsWindow::on_intensityfloorselect_changed()
{
   tdo->setintensityfloor(intensityfloorselect->get_value());
   prof->setintensityfloor(intensityfloorselect->get_value());
   changecoloursandshades();
}

/*
==================================
 AdvancedOptionsWindow::on_drawingresetbutton_clicked
 
 Draws as a result of the other callbacks, and only does so once because of
 threading, so it may be prudent to change this in the future so
 that there is only ever one call.
==================================
*/
void AdvancedOptionsWindow::on_drawingresetbutton_clicked()
{
   heightmaxselect->set_range(tdo->getrminz()+0.01,tdo->getrmaxz());
   heightmaxselect->set_value(tdo->getrmaxz());
   heightminselect->set_range(tdo->getrminz(),tdo->getrmaxz()-0.01);
   heightminselect->set_value(tdo->getrminz());
   heightscrollbar->set_range(tdo->getrminz(),tdo->getrmaxz());
   heightscrollbar->set_value(tdo->getrminz());
   heightscrollbar->get_adjustment()->set_page_size(tdo->getrmaxz() - tdo->getrminz());
   heightscrollbar->set_increments(0.01,tdo->getrmaxz()- tdo->getrminz());
   heightoffsetselect->set_value(0);
   heightfloorselect->set_value(0);
   intensitymaxselect->set_range(tdo->getrminintensity()+1, tdo->getrmaxintensity());
   intensitymaxselect->set_value(tdo->getrmaxintensity());
   intensityminselect->set_range(tdo->getrminintensity(), tdo->getrmaxintensity()-1);
   intensityminselect->set_value(tdo->getrminintensity());
   intensityscrollbar->set_range(tdo->getrminintensity(), tdo->getrmaxintensity());
   intensityscrollbar->set_value(tdo->getrminintensity());
   intensityscrollbar->get_adjustment()->set_page_size(tdo->getrmaxintensity()- tdo->getrminintensity());
   intensityscrollbar->set_increments(1,tdo->getrmaxintensity() - tdo->getrminintensity());
   intensityoffsetselect->set_value(0);
   intensityfloorselect->set_value(0);
}

/*
==================================
 AdvancedOptionsWindow::on_maindetailselected

 This indirectly determines how many points are skipped when viewing the
 main overview image. I.e. this affects it as well as the number of
 visible buckets.
==================================
*/
void AdvancedOptionsWindow::on_maindetailselected()
{
   tdo->setmaindetail(maindetailselect->get_value());
   if (tdo->is_realized())
      tdo->drawviewable(1);
}

/*
==================================
 AdvancedOptionsWindow::on_maindetailselectedprof

 Does the same as on_maindetailselected, except for the profile.
==================================
*/
void AdvancedOptionsWindow::on_maindetailselectedprof()
{
   prof->setmaindetail(maindetailselectprof->get_value());
   if(prof->is_realized())
      prof->drawviewable(1);
}

/*
==================================
 AdvancedOptionsWindow::on_previewdetailselectedprof

 Does the same as on_maindetailselectedprof, except for the preview of the
 profile.
==================================
*/
void AdvancedOptionsWindow::on_previewdetailselectedprof()
{
   prof->setpreviewdetail(previewdetailselectprof->get_value());
   if(prof->is_realized())
      prof->drawviewable(2);
}

/*
==================================
 AdvancedOptionsWindow::on_backgroundcolor_changed

 Sets the background colour for profile and the overview.
==================================
*/
void AdvancedOptionsWindow::on_backgroundcolor_changed()
{
	Gdk::Color c = backgroundcolorbutton->get_color();
	float red = (float)c.get_red() / 65535.0;
	float green = (float)c.get_green() / 65535.0;
	float blue = (float)c.get_blue() / 65535.0;
	float alpha = (float)backgroundcolorbutton->get_alpha() / 65535.0;
	tdo->set_background_colour(red, green, blue, alpha);
	prof->set_background_colour(red, green, blue, alpha);

	if (tdo->get_realized())
	{
		tdo->update_background_colour();
		tdo->drawviewable(1);
	}
	if (prof->get_realized())
	{
		prof->update_background_colour();
		prof->drawviewable(1);
	}
}
