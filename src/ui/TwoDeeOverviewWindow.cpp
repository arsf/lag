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
 * File: TwoDeeOverviewWindow.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: June-July 2010
 *
 * */
#include <gtkmm.h>
#include <gtkglmm.h>
#include <vector>
#include "TwoDeeOverviewWindow.h"

TwoDeeOverviewWindow::
TwoDeeOverviewWindow(TwoDeeOverview *tdo,
                     AdvancedOptionsWindow *aow,
                     FileSaver *fs,
                     Gtk::Window *tdowin,
                     Gtk::Window *profilewindow,
                     Glib::RefPtr<Gtk::Builder> builder,
                     Gtk::EventBox *eventboxtdo,
                     ProfileWindow *profwin){
   this->tdo = tdo;
   this->aow = aow;
   this->fs = fs;
   this->tdowin = tdowin;
   this->profilewindow = profilewindow;
   this->profwin = profwin;
   this->eventboxtdo = eventboxtdo;
   eventboxtdo->signal_key_press_event().
      connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
              on_tdo_key_press));
   drawwhentoggled = true;
   if(tdowin){//The overview window:
      tdowin->set_title("LAG Overview");

      //Menues:
      //File menu
      //For saving files:
      //For selecting to get file-saving menu.
      Gtk::MenuItem *savefilemenuitem = NULL;
      builder->get_widget("savefilemenuitem",
                         savefilemenuitem);
      if(savefilemenuitem)
         savefilemenuitem->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_savefilemenuactivated));

     
      // Quit
      Gtk::MenuItem *quitfilemenuitem = NULL;
      builder->get_widget("quitfilemenuitem",
                          quitfilemenuitem);
      if(quitfilemenuitem)
         quitfilemenuitem->signal_activate().
            connect(sigc::ptr_fun(gtk_main_quit));

      
      //Viewing options:
      builder->get_widget("showprofilecheck",
                         showprofilecheck);
      if(showprofilecheck)
         showprofilecheck->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_showprofilecheck));

      builder->get_widget("showfencecheck",
                         showfencecheck);
      if(showfencecheck)
         showfencecheck->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_showfencecheck));

      builder->get_widget("showdistancescalecheck",
                         showdistancescalecheck);
      if(showdistancescalecheck)
         showdistancescalecheck->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_showdistancescalecheck));

      builder->get_widget("showlegendcheck",
                         showlegendcheck);
      if(showlegendcheck)
         showlegendcheck->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_showlegendcheck));

      builder->get_widget("reverseheightcheck",
                         reverseheightcheck);
      if(reverseheightcheck)
         reverseheightcheck->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_reverseheightcheck));

      //For determining how to colour the overview:
      Gtk::RadioMenuItem *colourbynonemenu = NULL;
      builder->get_widget("colourbynonemenu",
                         colourbynonemenu);
      if(colourbynonemenu)
         colourbynonemenu->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_colouractivated));

      builder->get_widget("colourbyintensitymenu",
                         colourbyintensitymenu);
      if(colourbyintensitymenu)
         colourbyintensitymenu->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_colouractivated));

      builder->get_widget("colourbyheightmenu",
                         colourbyheightmenu);
      if(colourbyheightmenu)
         colourbyheightmenu->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_colouractivated));

      builder->get_widget("colourbyflightlinemenu",
                         colourbyflightlinemenu);
      if(colourbyflightlinemenu)
         colourbyflightlinemenu->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_colouractivated));

      builder->get_widget("colourbyclassificationmenu",
                         colourbyclassificationmenu);
      if(colourbyclassificationmenu)
         colourbyclassificationmenu->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_colouractivated));

      builder->get_widget("colourbyreturnmenu",
                         colourbyreturnmenu);
      if(colourbyreturnmenu)
         colourbyreturnmenu->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_colouractivated));

      //For determining how to shade the overview:
      Gtk::RadioMenuItem *brightnessbynonemenu = NULL;
      builder->get_widget("brightnessbynonemenu",
                         brightnessbynonemenu);
      if(brightnessbynonemenu)
         brightnessbynonemenu->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_brightnessactivated));

      builder->get_widget("brightnessbyintensitymenu",
                         brightnessbyintensitymenu);
      if(brightnessbyintensitymenu)
         brightnessbyintensitymenu->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_brightnessactivated));

      builder->get_widget("brightnessbyheightmenu",
                         brightnessbyheightmenu);
      if(brightnessbyheightmenu)
         brightnessbyheightmenu->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_brightnessactivated));
      //Help menu:
      Gtk::MenuItem *helpmenu = NULL;
      builder->get_widget("helpmenu",
                         helpmenu);
      if(helpmenu)
         helpmenu->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_helpmenuactivated));

      builder->get_widget("help",
                         help);
      if(help)
         help->signal_response().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_helpresponse));

      Gtk::MenuItem *aboutmenu = NULL;
      builder->get_widget("aboutmenu",
                         aboutmenu);
      if(aboutmenu)
         aboutmenu->signal_activate().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_aboutmenuactivated));

      builder->get_widget("about",
                         about);
      if(about)
         about->signal_response().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_aboutresponse));

      // Toolbar:
      // This returns the viewpoint to the initial one for that file or 
      // selection of files.
      Gtk::ToolButton *returnbutton = NULL;
      builder->get_widget("returnbutton",
                         returnbutton);
      if(returnbutton)
         returnbutton->signal_clicked().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_returnbutton_clicked));

      // Advanced viewing options:
      Gtk::ToolButton *advancedbutton = NULL;
      builder->get_widget("advancedbutton",
                         advancedbutton);
      if(advancedbutton)
         advancedbutton->signal_clicked().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_advancedbutton_clicked));

      //For overview image viewing attributes:
      builder->get_widget("pointwidthselect",
                         pointwidthselect);
      if(pointwidthselect){
         pointwidthselect->set_value(1);
         pointwidthselect->signal_value_changed().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_pointwidthselected));
      }
      //The ruler:
      builder->get_widget("rulertoggleover",
                         rulertoggleover);
      if(rulertoggleover)
         rulertoggleover->signal_toggled().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_rulertoggleover));

      //Fence and profile toggles and attribute selectors:
      builder->get_widget("fencetoggle",
                         fencetoggle);
      if(fencetoggle)
         fencetoggle->signal_toggled().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_fencetoggle));

      builder->get_widget("profiletoggle",
                         profiletoggle);
      if(profiletoggle)
         profiletoggle->signal_toggled().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_profiletoggle));

      builder->get_widget("orthogonalrectshapetoggle",
                         orthogonalrectshapetoggle);
      if(orthogonalrectshapetoggle)
         orthogonalrectshapetoggle->signal_toggled().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_orthogonalrectshapetoggle));

      builder->get_widget("slantedrectshapetoggle",
                         slantedrectshapetoggle);
      if(slantedrectshapetoggle)
         slantedrectshapetoggle->signal_toggled().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_slantedrectshapetoggle));

      builder->get_widget("slantwidthselect",
                         slantwidthselect);
      if(slantwidthselect){
         // Essentially arbitrary. Would there be any situation where a width 
         // greater than 30 km would be wanted?
         //slantwidthselect->set_range(0,30000);
         slantwidthselect->set_value(5);
         slantwidthselect->signal_value_changed().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_slantwidthselected));
      }
      builder->get_widget("raiselinecheckmenu",raiselinecheckmenu);
      if(raiselinecheckmenu)
         raiselinecheckmenu->signal_toggled().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_raiselinecheckmenu));

      builder->get_widget("raiselineselect",raiselineselect);
      if(raiselineselect)
         raiselineselect->signal_value_changed().
            connect(sigc::mem_fun(*this,&TwoDeeOverviewWindow::
                    on_raiselineselected));

      tdowin->show_all();
      //tdo->set_size_request(200,200);
      //Initialisations:
      tdo->setshowprofile(showprofilecheck->get_active());
      tdo->setshowfence(showfencecheck->get_active());
      tdo->setshowdistancescale(showdistancescalecheck->get_active());
      tdo->setshowlegend(showlegendcheck->get_active());
      tdo->setreversez(reverseheightcheck->get_active());
  
      //tdo->setintensitycolour(colourbyintensitymenu->get_active());
      //tdo->setheightcolour(colourbyheightmenu->get_active());
      //tdo->setlinecolour(colourbyflightlinemenu->get_active());
      //tdo->setclasscolour(colourbyclassificationmenu->get_active());
      //tdo->setreturncolour(colourbyreturnmenu->get_active());
      //tdo->setintensitybrightness(brightnessbyintensitymenu->get_active());
      //tdo->setheightbrightness(brightnessbyheightmenu->get_active());
      tdo->setpointwidth(pointwidthselect->get_value());
      tdo->getprofbox()->
            setslantwidth(slantwidthselect->get_value());
      tdo->getfencebox()->
            setslantwidth(slantwidthselect->get_value());
      tdo->getprofbox()->
            setslantedshape(slantedrectshapetoggle->get_active());
      tdo->getfencebox()->
            setslantedshape(slantedrectshapetoggle->get_active());
      tdo->getprofbox()->
            setorthogonalshape(orthogonalrectshapetoggle->get_active());
      tdo->getfencebox()->
            setorthogonalshape(orthogonalrectshapetoggle->get_active());
      tdo->setraiseline(raiselinecheckmenu->get_active());
      tdo->setlinetoraise(raiselineselect->get_value_as_int());

      // This is to help prevent confusion when the user decides to show the 
      // legend and nothing happens because of there being no legend when 
      // colouring by flightline or by none.
      showlegendcheck->set_inconsistent(!colourbyintensitymenu->get_active() && 
         !colourbyheightmenu->get_active() && 
         !colourbyclassificationmenu->get_active() && 
         !colourbyreturnmenu->get_active());

      
   }
}
TwoDeeOverviewWindow::
~TwoDeeOverviewWindow(){
   delete raiselineselect;
   delete raiselinecheckmenu;
   delete help;
   delete about;
   delete showprofilecheck;
   delete showfencecheck;
   delete showdistancescalecheck;
   delete showlegendcheck;
   delete reverseheightcheck;
   delete colourbyintensitymenu;
   delete colourbyheightmenu;
   delete colourbyflightlinemenu;
   delete colourbyclassificationmenu;
   delete colourbyreturnmenu;
   delete brightnessbyintensitymenu;
   delete brightnessbyheightmenu;
   delete fencetoggle;
   delete profiletoggle;
   delete orthogonalrectshapetoggle;
   delete slantedrectshapetoggle;
   delete slantwidthselect;
   delete pointwidthselect;
   delete rulertoggleover;
}

// When toggled, the profile box is shown on the 2d overview regardless of 
// whether profiling mode is active.
void TwoDeeOverviewWindow::
on_showprofilecheck(){
   tdo->setshowprofile(showprofilecheck->get_active());
   if(tdo->is_realized())
      tdo->drawviewable(2);
}

// When toggled, the fence box is shown on the 2d overview regardless of 
// whether fencing mode is active.
void TwoDeeOverviewWindow::
on_showfencecheck(){
   tdo->setshowfence(showfencecheck->get_active());
   if(tdo->is_realized())
      tdo->drawviewable(2);
}

//When toggled, the distance scale is shown on the 2d overview.
void TwoDeeOverviewWindow::
on_showdistancescalecheck(){
   tdo->setshowdistancescale(showdistancescalecheck->get_active());
   if(tdo->is_realized())
      tdo->drawviewable(2);
}

//When toggled, the legend is shown on the 2d overview.
void TwoDeeOverviewWindow::
on_showlegendcheck(){
   tdo->setshowlegend(showlegendcheck->get_active());
   if(tdo->is_realized())
      tdo->drawviewable(2);
}

//When toggled, the heights are reversed on the 2d overview.
void TwoDeeOverviewWindow::
on_reverseheightcheck(){
   tdo->setreversez(reverseheightcheck->get_active());
   if(tdo->is_realized())
      tdo->drawviewable(1);
}

// If one of the colour radio menu items is selected (and, therefore, the 
// others deselected) then set the values of the colour control variables 
// in the overview to the values of the corresponding radio menu items.
void TwoDeeOverviewWindow::
on_colouractivated(){
   if (colourbyintensitymenu->get_active())
      tdo->setColourBy(tdo->colourByIntensity);
   else if (colourbyheightmenu->get_active())
      tdo->setColourBy(tdo->colourByHeight);
   else if (colourbyflightlinemenu->get_active())
      tdo->setColourBy(tdo->colourByFlightline);
   else if (colourbyclassificationmenu->get_active())
      tdo->setColourBy(tdo->colourByClassification);
   else if (colourbyreturnmenu->get_active())
      tdo->setColourBy(tdo->colourByReturn);
   else
      tdo->setColourBy(tdo->colourByNone);
 
//   tdo->setintensitycolour(colourbyintensitymenu->get_active());
//   tdo->setheightcolour(colourbyheightmenu->get_active());
//   tdo->setlinecolour(colourbyflightlinemenu->get_active());
//   tdo->setclasscolour(colourbyclassificationmenu->get_active());
//   tdo->setreturncolour(colourbyreturnmenu->get_active());

   // This is to help prevent confusion when the user decides to show the 
   // legend and nothing happens because of there being no legend when 
   // colouring by flightline or by none.
   showlegendcheck->set_inconsistent(!colourbyintensitymenu->get_active() && 
      !colourbyheightmenu->get_active() && 
      !colourbyclassificationmenu->get_active() && 
      !colourbyreturnmenu->get_active());
   // As the signal handler is called twice (each time a radio button is 
   // toggle or untoggled, and toggling one automatically untoggles another), 
   // and we only want to draw (which is slow when caching) once, draw half 
   // the time this handler is called.
   drawwhentoggled = !drawwhentoggled;
   if(tdo->is_realized()&&drawwhentoggled)
      tdo->drawviewable(1);
}

// If one of the brightness radio menu items is selected (and, therefore, 
// the others deselected) then set the values of the brightness control 
// variables in the overview to the values of the corresponding radio 
// menu items.
void TwoDeeOverviewWindow::
on_brightnessactivated(){

   if (brightnessbyintensitymenu->get_active())
      tdo->setBrightnessBy(tdo->brightnessByIntensity);
   else if (brightnessbyheightmenu->get_active())
      tdo->setBrightnessBy(tdo->brightnessByHeight);
   else
      tdo->setBrightnessBy(tdo->brightnessByNone);

//   tdo->setintensitybrightness(brightnessbyintensitymenu->get_active());
//   tdo->setheightbrightness(brightnessbyheightmenu->get_active());

   // As the signal handler is called twice (each time a radio button is 
   // toggle or untoggled, and toggling one automatically untoggles another), 
   // and we only want to draw (which is slow when caching) once, draw half 
   // the time this handler is called.
   drawwhentoggled = !drawwhentoggled;
   if(tdo->is_realized()&&drawwhentoggled)tdo->drawviewable(1);
}

//This returns the overview to its original position.
void TwoDeeOverviewWindow::
on_returnbutton_clicked(){
   if(tdo->is_realized())
      tdo->returntostart();
}

//This changes the width of the points in pixels.
void TwoDeeOverviewWindow::
on_pointwidthselected(){
   tdo->setpointwidth(pointwidthselect->get_value());
   if(tdo->is_realized())
      tdo->drawviewable(1);
}
 
// When toggled, the profile view goes into rulering mode. When untoggled, 
// rulering mode ends.
void TwoDeeOverviewWindow::
on_rulertoggleover(){
   if(rulertoggleover->get_active()){
      if(fencetoggle->get_active())
         fencetoggle->set_active(false);
      if(profiletoggle->get_active())
         profiletoggle->set_active(false);
      tdo->setupruler();
      if(tdo->is_realized())
         tdo->drawviewable(2);
   }
   else{
      tdo->unsetupruler();
      if(tdo->is_realized() &&
         !profiletoggle->get_active() &&
         !rulertoggleover->get_active() &&
         !fencetoggle->get_active())
         tdo->drawviewable(2);
   }
}
// Toggles whether clicking and dragging will select the fence in the 
// overview.
void TwoDeeOverviewWindow::
on_fencetoggle(){
   if(fencetoggle->get_active()){
      if(profiletoggle->get_active())
         profiletoggle->set_active(false);
      if(rulertoggleover->get_active())
         rulertoggleover->set_active(false);
      tdo->setupfence();
      if(tdo->is_realized())
         tdo->drawviewable(2);
   }
   else{
   	tdo->unsetupfence();
      if(tdo->is_realized() &&
         !profiletoggle->get_active() &&
         !rulertoggleover->get_active() &&
         !fencetoggle->get_active())
         tdo->drawviewable(2);
   }
}
// When toggled, the 2d overview goes into profile selection mode. When 
// untoggled, 2d overview goes out of profile selection mode and the profile 
// parameters are sent to the profile area.
void TwoDeeOverviewWindow::
on_profiletoggle(){
   if(profiletoggle->get_active()){
      if(fencetoggle->get_active())
         fencetoggle->set_active(false);
      if(rulertoggleover->get_active())
         rulertoggleover->set_active(false);
      tdo->setupprofile();
      if(tdo->is_realized())
         tdo->drawviewable(2);
   }
   else{
   	tdo->unsetupprofile();
      if(tdo->is_realized() &&
         !profilewindow->get_visible())
         profilewindow->show_all();
      if(tdo->is_realized() &&
         !profiletoggle->get_active() &&
         !rulertoggleover->get_active() &&
         !fencetoggle->get_active())
         tdo->drawviewable(2);
   }
}
// When toggled, this makes sure that the slant toggle is in the opposite state 
// and then sets the orthogonal shape on the overview and makes new profile and 
// fence boundaries before redrawing the overview, possibly with the new 
// boundaries for profile and/or fence displayed.
void TwoDeeOverviewWindow::
on_orthogonalrectshapetoggle(){
   if(orthogonalrectshapetoggle->get_active())
      if(slantedrectshapetoggle->get_active())
         slantedrectshapetoggle->set_active(false);
   if(!orthogonalrectshapetoggle->get_active())
      if(!slantedrectshapetoggle->get_active())
         slantedrectshapetoggle->set_active(true);
   tdo->getprofbox()->
      setorthogonalshape(orthogonalrectshapetoggle->get_active());
   tdo->getfencebox()->
      setorthogonalshape(orthogonalrectshapetoggle->get_active());
   tdo->getprofbox()->makeboundaries();
   tdo->getfencebox()->makeboundaries();
   if(tdo->is_realized())
      tdo->drawviewable(2);
}
// When toggled, this makes sure that the orthogonal toggle is in the opposite 
// state and then sets the slanted shape on the overview and makes new profile 
// and fence boundaries before redrawing the overview, possibly with the new 
// boundaries for profile and/or fence displayed.
void TwoDeeOverviewWindow::
on_slantedrectshapetoggle(){
   if(slantedrectshapetoggle->get_active())
      if(orthogonalrectshapetoggle->get_active())
         orthogonalrectshapetoggle->set_active(false);
   if(!slantedrectshapetoggle->get_active())
      if(!orthogonalrectshapetoggle->get_active())
         orthogonalrectshapetoggle->set_active(true);

   tdo->getprofbox()->
      setslantedshape(slantedrectshapetoggle->get_active());
   tdo->getfencebox()->
      setslantedshape(slantedrectshapetoggle->get_active());

   tdo->getprofbox()->makeboundaries();
   tdo->getfencebox()->makeboundaries();
   if(tdo->is_realized())tdo->
      drawviewable(2);
}
// When the value in the spinbutton for slanted shape width is changed, tell the 
// 2d overview, then make the new slanted box and then draw it. This does NOT 
// update the profile itself (or, at least, not yet) if the slanted box is for 
// a profile. To update the profile after the width has been satisfactorily 
// adjusted, the profiletoggle must be toggled and then untoggled.
void TwoDeeOverviewWindow::
on_slantwidthselected(){
   tdo->getprofbox()->
      setslantwidth(slantwidthselect->get_value());
   tdo->getfencebox()->
      setslantwidth(slantwidthselect->get_value());

   tdo->getprofbox()->makeboundaries();
   tdo->getfencebox()->makeboundaries();
   tdo->getprofbox()->drawinfo();
   tdo->getfencebox()->drawinfo();

   if(tdo->is_realized())
      tdo->drawviewable(2);
}
//Opens the advanced options dialog.
void TwoDeeOverviewWindow::
on_advancedbutton_clicked(){ 
   aow->show(); 
}

//Show the help dialog when respective menu item activated.
void TwoDeeOverviewWindow::
on_helpmenuactivated(){ 
   help->present(); 
}

//Hide the help dialog when close button activated.
void TwoDeeOverviewWindow::
on_helpresponse(int response_id){ 
   help->hide(); 
}

//Show the about dialog when respective menu item activated.
void TwoDeeOverviewWindow::
on_aboutmenuactivated(){ 
   about->present(); 
}

//Hide the about dialog when close button activated.
void TwoDeeOverviewWindow::
on_aboutresponse(int response_id){ 
   about->hide(); 
}

//When selected from the menu, the file saver opens.
void TwoDeeOverviewWindow::
on_savefilemenuactivated(){
   if(tdo->is_realized())
      fs->show();
   else 
      return;

   fs->on_flightlinesaveselected();
}

//Interprets the keybaord signals from the EventBox.
bool TwoDeeOverviewWindow::
on_tdo_key_press(GdkEventKey* event){
   if(!tdo->is_realized())
      //If the overview does not exist, prevent segfaults by doing nothing.
      return false;

   switch(event->keyval){
      //Extract a profile.
      case GDK_P:
      case GDK_p:
      case GDK_space:
         profwin->on_showprofilebutton_clicked();
         tdowin->present();
         return true;
         break;
      //Move overview view.
      case GDK_w:
      case GDK_s:
      case GDK_a:
      case GDK_d:
      case GDK_z:
      case GDK_Z:
         return tdo->on_pan_key(event,aow->getmovespeed());
         break;
      //Move one or none of the profile or the fence, depending 
      //on whether either of them are visible.
      case GDK_W:
      case GDK_S:
      case GDK_A:
      case GDK_D:
         if(profiletoggle->get_active())
            return tdo->on_prof_key(event,aow->getmovespeed(),
                                          aow->getfractionalshift());
         else if(fencetoggle->get_active())
            return tdo->on_fence_key(event,aow->getmovespeed());
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
         return tdo->on_zoom_key(event);
         break;
      //Toggle fencing mode.
      case GDK_f:
      case GDK_F:
         fencetoggle->set_active(!fencetoggle->get_active());
         return true;
         break;
      //Toggle profiling mode.
      case GDK_x:
      case GDK_X:
         profiletoggle->set_active(!profiletoggle->get_active());
         return true;
         break;
      // Toggle whether the profile and fence should be orthogonal 
      // or slanted.
      case GDK_t:
      case GDK_T:
         slantedrectshapetoggle->
            set_active(!slantedrectshapetoggle->get_active());
         return true;
         break;
      //Switch to profile window.
      case GDK_slash:
      case GDK_backslash:
         profilewindow->present();
         return true;
         break;
      default:
         return false;
         break;
   }
   return false;
}

//Determines which line is to be raised above the rest on the overview.
void TwoDeeOverviewWindow::
on_raiselineselected(){
   tdo->setlinetoraise(raiselineselect->get_value_as_int());

   if(tdo->is_realized())
      if(raiselinecheckmenu->get_active())
         tdo->drawviewable(1);
}

//Determines whether a line is to be raised.
void TwoDeeOverviewWindow::
on_raiselinecheckmenu(){
   tdo->setraiseline(raiselinecheckmenu->get_active());
   if(tdo->is_realized())
      tdo->drawviewable(1);
}
