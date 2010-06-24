/*
 * File: lag.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: November 2009 - June 2010
 *
 * */
#include <iostream>
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "quadtree.h"
#include "TwoDeeOverview.h"
#include "Profile.h"
#include "AdvancedOptionsWindow.h"
#include "FileOpener.h"
#include "FileSaver.h"
using namespace std;

TwoDeeOverview *tdo = NULL;//The 2d overview.
Profile *prof = NULL;//The profile.
string exename = "";//The path of the executable.
bool drawwhentoggled = true;//This variable prevents the image(s) from being drawn twice as a result of toggling a radio button (or similar), which deactivates (and therefore toggles again) another one in the same group. This variable must start as true, as the methods make it opposite before using it, so that things will de drawn after the second "toggling".
AdvancedOptionsWindow *aow = NULL;
FileOpener *fo = NULL;
FileSaver *fs = NULL;


//Gtk objects:
Gtk::AboutDialog *about = NULL;//Information about LAG.
//Overview:
   //Menues:
      //View menu:
         Gtk::CheckMenuItem *showprofilecheck = NULL;//Check button determining whether the profile box is viewable on the 2d overview.
         Gtk::CheckMenuItem *showfencecheck = NULL;//Check button determining whether the fence box is viewable on the 2d overview.
         Gtk::CheckMenuItem *showdistancescalecheck = NULL;//Check button determining whether the distance scale is viewable on the 2d overview.
         Gtk::CheckMenuItem *showlegendcheck = NULL;//Check button determining whether the distance scale is viewable on the 2d overview.
         Gtk::CheckMenuItem *reverseheightcheck = NULL;//Check button determining whether the distance scale is viewable on the 2d overview.
      //"Colour by" menu:
         Gtk::RadioMenuItem *colourbyintensitymenu = NULL;//Determines whether the image is coloured by intensity.
         Gtk::RadioMenuItem *colourbyheightmenu = NULL;//Determines whether the image is coloured by height.
         Gtk::RadioMenuItem *colourbyflightlinemenu = NULL;//Determines whether the image is coloured by flightline.
         Gtk::RadioMenuItem *colourbyclassificationmenu = NULL;//Determines whether the image is coloured by classification.
         Gtk::RadioMenuItem *colourbyreturnmenu = NULL;//Determines whether the image is coloured by return.
      //"Brightness by" menu:
         Gtk::RadioMenuItem *brightnessbyintensitymenu = NULL;//Determines whether the image is shaded by intensity.
         Gtk::RadioMenuItem *brightnessbyheightmenu = NULL;//Determines whether the image is shaded by height.
   //Toolbar:
      Gtk::ToggleToolButton *fencetoggle = NULL;//Toggle button determining whether mouse dragging selects the fence.
      Gtk::ToggleToolButton *profiletoggle = NULL;//Toggle button determining whether mouse dragging selects the profile.
      Gtk::ToggleToolButton *orthogonalrectshapetoggle = NULL;//Toggle button determining whether or not to use an orthogonal selection.
      Gtk::ToggleToolButton *slantedrectshapetoggle = NULL;//Toggle button determining whether or not to use a slanted selection.
      Gtk::SpinButton *slantwidthselect = NULL;//Determines the width of the profile in metres.
      Gtk::SpinButton *pointwidthselect = NULL;//Determines the width of the points in the overview in pixels.
      Gtk::ToggleToolButton *rulertoggleover = NULL;//Toggle button determining whether the ruler is viewable on the overview.
   Gtk::Label *rulerlabelover = NULL;//Label displaying the distance along the ruler, in all dimensions etc. for the overview. Also other text output about the data and fences, profiles etc. is put here.
//Profile:
Gtk::Window *profilewindow = NULL;
   //Menues:
      //View menu:
         Gtk::CheckMenuItem *showheightscalecheck = NULL;//Check button determining whether the height scale is viewable on the profile.
      //"Colour by" menu:
         Gtk::RadioMenuItem *colourbyintensitymenuprof = NULL;//Determines whether the profile is coloured by intensity.
         Gtk::RadioMenuItem *colourbyheightmenuprof = NULL;//Determines whether the profile is coloured by height.
         Gtk::RadioMenuItem *colourbyflightlinemenuprof = NULL;//Determines whether the profile is coloured by flightline.
         Gtk::RadioMenuItem *colourbyclassificationmenuprof = NULL;//Determines whether the profile is coloured by classification.
         Gtk::RadioMenuItem *colourbyreturnmenuprof = NULL;//Determines whether the profile is coloured by return.
      //"Brightness by" menu:
         Gtk::RadioMenuItem *brightnessbyintensitymenuprof = NULL;//Determines whether the profile is shaded by intensity.
         Gtk::RadioMenuItem *brightnessbyheightmenuprof = NULL;//Determines whether the profile is shaded by height.
   //Toolbar:
      Gtk::SpinButton *pointwidthselectprof = NULL;//Determines the width of the points in the profile in pixels.
      Gtk::ToggleToolButton *pointshowtoggle = NULL;//Whether to show the points on the profile.
      Gtk::ToggleToolButton *lineshowtoggle = NULL;//Whether to show the lines on the profile.
      Gtk::SpinButton *movingaveragerangeselect = NULL;//The range of the moving average for the lines on the profile.
      Gtk::ToggleToolButton *fencetoggleprof = NULL;//Toggle button determining whether mouse dragging selects the fence.
      Gtk::ToggleToolButton *rulertoggle = NULL;//Toggle button determining whether the ruler is viewable on the profile.
      Gtk::Label *rulerlabel = NULL;//Label displaying the distance along the ruler, in all dimensions etc. for the profile.
      Gtk::SpinButton *slantwidthselectprof = NULL;//Determines the width of the fence in metres when slanted.
      Gtk::ToggleToolButton *orthogonalprof = NULL;//Determine whether the profile fence is orthogonal or slanted.
      Gtk::ToggleToolButton *slantedprof = NULL;//...

//Show the about dialog when respective menu item activated.
void on_aboutmenuactivated(){ about->show_all(); }
//Hide the about dialog when close button activated.
void on_aboutresponse(int response_id){ about->hide_all(); }

//When selected from the menu, the file chooser opens.
void on_openfilemenuactivated(){ fo->show(); }//Y'all!

//When selected from the menu, the file saver opens.
void on_savefilemenuactivated(){
   if(tdo->is_realized())fs->show();
   else return;
   fs->on_flightlinesaveselected();
}

//When toggled, the profile box is shown on the 2d overview regardless of whether profiling mode is active.
void on_showprofilecheck(){
   tdo->setshowprofile(showprofilecheck->get_active());
   if(tdo->is_realized())tdo->drawviewable(2);
}
//When toggled, the fence box is shown on the 2d overview regardless of whether fencing mode is active.
void on_showfencecheck(){
   tdo->setshowfence(showfencecheck->get_active());
   if(tdo->is_realized())tdo->drawviewable(2);
}
//When toggled, the distance scale is shown on the 2d overview.
void on_showdistancescalecheck(){
   tdo->setshowdistancescale(showdistancescalecheck->get_active());
   if(tdo->is_realized())tdo->drawviewable(2);
}
//When toggled, the legend is shown on the 2d overview.
void on_showlegendcheck(){
   tdo->setshowlegend(showlegendcheck->get_active());
   if(tdo->is_realized())tdo->drawviewable(2);
}
//When toggled, the heights are reversed on the 2d overview.
void on_reverseheightcheck(){
   tdo->setreversez(reverseheightcheck->get_active());
   if(tdo->is_realized())tdo->drawviewable(1);
}
//If one of the colour radio menu items is selected (and, therefore, the others deselected) then set the values of the colour control variables in the overview to the values of the corresponding radio menu items.
void on_colouractivated(){
   tdo->setintensitycolour(colourbyintensitymenu->get_active());
   tdo->setheightcolour(colourbyheightmenu->get_active());
   tdo->setlinecolour(colourbyflightlinemenu->get_active());
   tdo->setclasscolour(colourbyclassificationmenu->get_active());
   tdo->setreturncolour(colourbyreturnmenu->get_active());
   showlegendcheck->set_inconsistent(!colourbyintensitymenu->get_active() && !colourbyheightmenu->get_active() && !colourbyclassificationmenu->get_active() && !colourbyreturnmenu->get_active());//This is to help prevent confusion when the user decides to show the legend and nothing happens because of there being no legend when colouring by flightline or by none.
   drawwhentoggled = !drawwhentoggled;//As the signal handler is called twice (each time a radio button is toggle or untoggled, and toggling one automatically untoggles another), and we only want to draw (which is slow when caching) once, draw half the time this handler is called.
   if(tdo->is_realized()&&drawwhentoggled)tdo->drawviewable(1);
}
//If one of the brightness radio menu items is selected (and, therefore, the others deselected) then set the values of the brightness control variables in the overview to the values of the corresponding radio menu items.
void on_brightnessactivated(){
   tdo->setintensitybrightness(brightnessbyintensitymenu->get_active());
   tdo->setheightbrightness(brightnessbyheightmenu->get_active());
   drawwhentoggled = !drawwhentoggled;//As the signal handler is called twice (each time a radio button is toggle or untoggled, and toggling one automatically untoggles another), and we only want to draw (which is slow when caching) once, draw half the time this handler is called.
   if(tdo->is_realized()&&drawwhentoggled)tdo->drawviewable(1);
}

//This returns the overview to its original position.
void on_returnbutton_clicked(){
   if(tdo->is_realized())tdo->returntostart();
}
//This changes the width of the points in pixels.
void on_pointwidthselected(){
   tdo->setpointwidth(pointwidthselect->get_value());
   if(tdo->is_realized())tdo->drawviewable(1);
}
//When toggled, the profile view goes into rulering mode. When untoggled, rulering mode ends.
void on_rulertoggleover(){
   if(rulertoggleover->get_active()){
      if(fencetoggle->get_active())fencetoggle->set_active(false);
      if(profiletoggle->get_active())profiletoggle->set_active(false);
      tdo->setupruler();
      if(tdo->is_realized())tdo->drawviewable(2);
   }
   else{
      tdo->unsetupruler();
      if(tdo->is_realized()&&!profiletoggle->get_active()&&!rulertoggleover->get_active()&&!fencetoggle->get_active())tdo->drawviewable(2);
   }
}
//Toggles whether clicking and dragging will select the fence in the overview.
void on_fencetoggle(){
   if(fencetoggle->get_active()){
      if(profiletoggle->get_active())profiletoggle->set_active(false);
      if(rulertoggleover->get_active())rulertoggleover->set_active(false);
      tdo->setupfence();
      if(tdo->is_realized())tdo->drawviewable(2);
   }
   else{
   	tdo->unsetupfence();
      if(tdo->is_realized()&&!profiletoggle->get_active()&&!rulertoggleover->get_active()&&!fencetoggle->get_active())tdo->drawviewable(2);
   }
}
//When toggled, the 2d overview goes into profile selection mode. When untoggled, 2d overview goes out of profile selection mode and the profile parameters are sent to the profile area.
void on_profiletoggle(){
   if(profiletoggle->get_active()){
      if(fencetoggle->get_active())fencetoggle->set_active(false);
      if(rulertoggleover->get_active())rulertoggleover->set_active(false);
      tdo->setupprofile();
      if(tdo->is_realized())tdo->drawviewable(2);
   }
   else{
   	tdo->unsetupprofile();
      if(tdo->is_realized()&&!profilewindow->get_visible())profilewindow->show_all();
      if(tdo->is_realized()&&!profiletoggle->get_active()&&!rulertoggleover->get_active()&&!fencetoggle->get_active())tdo->drawviewable(2);
   }
}
//When toggled, this makes sure that the slant toggle is in the opposite state and then sets the orthogonal shape on the overview and makes new profile and fence boundaries before redrawing the overview, possibly with the new boundaries for profile and/or fence displayed.
void on_orthogonalrectshapetoggle(){
   if(orthogonalrectshapetoggle->get_active())if(slantedrectshapetoggle->get_active())slantedrectshapetoggle->set_active(false);
   if(!orthogonalrectshapetoggle->get_active())if(!slantedrectshapetoggle->get_active())slantedrectshapetoggle->set_active(true);
   tdo->getprofbox()->setorthogonalshape(orthogonalrectshapetoggle->get_active());
   tdo->getfencebox()->setorthogonalshape(orthogonalrectshapetoggle->get_active());
   tdo->getprofbox()->makeboundaries();
   tdo->getfencebox()->makeboundaries();
   if(tdo->is_realized())tdo->drawviewable(2);
}
//When toggled, this makes sure that the orthogonal toggle is in the opposite state and then sets the slanted shape on the overview and makes new profile and fence boundaries before redrawing the overview, possibly with the new boundaries for profile and/or fence displayed.
void on_slantedrectshapetoggle(){
   if(slantedrectshapetoggle->get_active())if(orthogonalrectshapetoggle->get_active())orthogonalrectshapetoggle->set_active(false);
   if(!slantedrectshapetoggle->get_active())if(!orthogonalrectshapetoggle->get_active())orthogonalrectshapetoggle->set_active(true);
   tdo->getprofbox()->setslantedshape(slantedrectshapetoggle->get_active());
   tdo->getfencebox()->setslantedshape(slantedrectshapetoggle->get_active());
   tdo->getprofbox()->makeboundaries();
   tdo->getfencebox()->makeboundaries();
   if(tdo->is_realized())tdo->drawviewable(2);
}
//When the value in the spinbutton for slanted shape width is changed, tell the 2d overview, then make the new slanted box and then draw it. This does NOT update the profile itself (or, at least, not yet) if the slanted box is for a profile. To update the profile after the width has been satisfactorily adjusted, the profiletoggle must be toggled and then untoggled.
void on_slantwidthselected(){
   tdo->getprofbox()->setslantwidth(slantwidthselect->get_value());
   tdo->getfencebox()->setslantwidth(slantwidthselect->get_value());
   tdo->getprofbox()->makeboundaries();
   tdo->getfencebox()->makeboundaries();
   tdo->getprofbox()->drawinfo();
   tdo->getfencebox()->drawinfo();
   if(tdo->is_realized())tdo->drawviewable(2);
}

//When toggled, the height scale is shown on the profile.
void on_showheightscalecheck(){
   prof->setshowheightscale(showheightscalecheck->get_active());
   if(prof->is_realized())prof->drawviewable(2);
}
//Does the same as on_colouractivated, except for the profile.
void on_colouractivatedprof(){
   prof->setintensitycolour(colourbyintensitymenuprof->get_active());
   prof->setheightcolour(colourbyheightmenuprof->get_active());
   prof->setlinecolour(colourbyflightlinemenuprof->get_active());
   prof->setclasscolour(colourbyclassificationmenuprof->get_active());
   prof->setreturncolour(colourbyreturnmenuprof->get_active());
   if(prof->is_realized())prof->drawviewable(1);
}
//Does the same as on_brightnessactivated, except for the profile.
void on_brightnessactivatedprof(){
   prof->setintensitybrightness(brightnessbyintensitymenuprof->get_active());
   prof->setheightbrightness(brightnessbyheightmenuprof->get_active());
   if(prof->is_realized())prof->drawviewable(1);
}

//This grabs the profile from the overview.
void on_showprofilebutton_clicked(){
   if(tdo->is_realized()&&!profilewindow->get_visible())profilewindow->show_all();
   double *profxs = NULL,*profys = NULL;//These are NOT to be deleted here as the arrays they will point to will be managed by the TwoDeeOVerview object.
   int profps = 0;
   if(tdo->is_realized())tdo->getprofile(profxs,profys,profps);
   if(profxs!=NULL&&profys!=NULL){
      tdo->setpausethread(true);//Showprofile uses the getpoint() method, and that must never be used by more than one thread at once.
//      while(tdo->getthread_running()){usleep(10);}
      tdo->waitforpause();
      prof->showprofile(profxs,profys,profps,true);
      tdo->setpausethread(false);
   }
}
//This returns the profile to its original position.
void on_returnbuttonprof_clicked(){
   if(prof->is_realized())prof->returntostart();
}
//Does the same as on_pointwidthselected, except for the profile.
void on_pointwidthselectedprof(){
   prof->setpointwidth(pointwidthselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(2);
}
//Determines whether to display the points on the profile.
void on_pointshowtoggle(){
   prof->setdrawpoints(pointshowtoggle->get_active());
   if(prof->is_realized())prof->drawviewable(1);
}
//Determines whether to display the (best fit) lines on the profile.
void on_lineshowtoggle(){
   prof->setdrawmovingaverage(lineshowtoggle->get_active());
   if(prof->is_realized())prof->drawviewable(1);
}
//The best fit is a moving average, and this changes the range, and therefore the shape of the line.
void on_movingaveragerangeselect(){
   prof->setmavrgrange(movingaveragerangeselect->get_value());
   prof->make_moving_average();
   if(prof->is_realized())prof->drawviewable(1);
}
//This classifies the points surrounded by the fence.
void on_classbutton_clicked(){
   tdo->setpausethread(true);//Nothing else must read the points (or indeed write to them!) while the classifier is writing to them. Also, it uses the getpoint() method.
//   while(tdo->getthread_running()){usleep(10);}
   tdo->waitforpause();
   if(prof->is_realized())prof->classify(aow->getclassificationvalue());
   tdo->setpausethread(false);
   tdo->drawviewable(1);
}
//Toggles whether clicking and dragging will select the fence in the profile.
void on_fencetoggleprof(){
   if(fencetoggleprof->get_active()){
      if(rulertoggle->get_active())rulertoggle->set_active(false);
      prof->setupfence();
      if(prof->is_realized())prof->drawviewable(1);
   }
   else{
      prof->unsetupfence();
      if(prof->is_realized()&&!rulertoggle->get_active()&&!fencetoggle->get_active())prof->drawviewable(1);
   }
}
void on_slantwidthselectedprof(){
   prof->setslantwidth(slantwidthselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(1);
}
void on_orthogonalprof(){
   if(orthogonalprof->get_active())if(slantedprof->get_active())slantedprof->set_active(false);
   if(!orthogonalprof->get_active())if(!slantedprof->get_active())slantedprof->set_active(true);
   prof->setslanted(!orthogonalprof->get_active());
   if(prof->is_realized())prof->drawviewable(1);
}
void on_slantedprof(){
   if(slantedprof->get_active())if(orthogonalprof->get_active())orthogonalprof->set_active(false);
   if(!slantedprof->get_active())if(!orthogonalprof->get_active())orthogonalprof->set_active(true);
   prof->setslanted(slantedprof->get_active());
   if(prof->is_realized())prof->drawviewable(1);
}
//When toggled, the profile view goes into rulering mode. When untoggled, rulering mode ends.
void on_rulertoggle(){
   if(rulertoggle->get_active()){
      if(fencetoggleprof->get_active())fencetoggleprof->set_active(false);
      prof->setupruler();
      if(prof->is_realized())prof->drawviewable(1);
   }
   else{
      prof->unsetupruler();
      if(prof->is_realized()&&!rulertoggle->get_active()&&!fencetoggle->get_active())prof->drawviewable(1);
   }
}

bool on_tdo_key_press(GdkEventKey* event){
   if(event->keyval == GDK_P || event->keyval == GDK_p || event->keyval == GDK_space)on_showprofilebutton_clicked();
   return true;
}
bool on_prof_key_press(GdkEventKey* event){
   if(event->keyval == GDK_P || event->keyval == GDK_p || event->keyval == GDK_space)on_showprofilebutton_clicked();
   if(event->keyval == GDK_C || event->keyval == GDK_c || event->keyval == GDK_K || event->keyval == GDK_k || event->keyval == GDK_Return)on_classbutton_clicked();
   return true;
}

//Opens the advanced options dialog.
void on_advancedbutton_clicked(){ aow->show(); }

//Sets up the GUI.
int GUIset(int argc,char *argv[]){
   Glib::thread_init();
   Gtk::Main gtkmain(argc, argv);
   Glib::RefPtr<Gnome::Glade::Xml> refXml;
   try{//Takes the path the executable was called with and uses it to find the .glade file needed to make the GUI.
      unsigned int index = exename.rfind("/");//Find the last forward slash and make its index our index.
      if(index==string::npos)index=0;//I.e. in the event that there is no forward slash (so must be calling from the same directory), just go from 0, where the forward slash would have been.
      else index++;//We do not actually want to include the forward slash.
      string gladename = exename;
      gladename.replace(index,9,"lag.glade");
      cout << exename << endl;
      cout << gladename << endl;
      refXml = Gnome::Glade::Xml::create(gladename);
   }
   catch(const Gnome::Glade::XmlError& ex){ 
      cerr << ex.what() << std::endl;
      cerr << "The file lag.glade must be located in the same directory as the lag executable." << endl;
      return 1;
   }
   Gtk::GL::init(argc, argv);
   Gtk::Window *overviewwindow = NULL;
   refXml->get_widget("overviewwindow", overviewwindow);
   if(overviewwindow){//The overview window:
      overviewwindow->set_title("LAG Overview");
      //Menues:
      //For opening files:
         Gtk::MenuItem *openfilemenuitem = NULL;//For selecting to get file-opening menu.
         refXml->get_widget("openfilemenuitem",openfilemenuitem);
         if(openfilemenuitem)openfilemenuitem->signal_activate().connect(sigc::ptr_fun(&on_openfilemenuactivated));
      //For saving files:
         Gtk::MenuItem *savefilemenuitem = NULL;//For selecting to get file-saving menu.
         refXml->get_widget("savefilemenuitem",savefilemenuitem);
         if(savefilemenuitem)savefilemenuitem->signal_activate().connect(sigc::ptr_fun(&on_savefilemenuactivated));
      //Viewing options:
         refXml->get_widget("showprofilecheck",showprofilecheck);
         if(showprofilecheck)showprofilecheck->signal_activate().connect(sigc::ptr_fun(&on_showprofilecheck));
         refXml->get_widget("showfencecheck",showfencecheck);
         if(showfencecheck)showfencecheck->signal_activate().connect(sigc::ptr_fun(&on_showfencecheck));
         refXml->get_widget("showdistancescalecheck",showdistancescalecheck);
         if(showdistancescalecheck)showdistancescalecheck->signal_activate().connect(sigc::ptr_fun(&on_showdistancescalecheck));
         refXml->get_widget("showlegendcheck",showlegendcheck);
         if(showlegendcheck)showlegendcheck->signal_activate().connect(sigc::ptr_fun(&on_showlegendcheck));
         refXml->get_widget("reverseheightcheck",reverseheightcheck);
         if(reverseheightcheck)reverseheightcheck->signal_activate().connect(sigc::ptr_fun(&on_reverseheightcheck));
         //For determining how to colour the overview:
            Gtk::RadioMenuItem *colourbynonemenu = NULL;
            refXml->get_widget("colourbynonemenu",colourbynonemenu);
            if(colourbynonemenu)colourbynonemenu->signal_activate().connect(sigc::ptr_fun(&on_colouractivated));
            refXml->get_widget("colourbyintensitymenu",colourbyintensitymenu);
            if(colourbyintensitymenu)colourbyintensitymenu->signal_activate().connect(sigc::ptr_fun(&on_colouractivated));
            refXml->get_widget("colourbyheightmenu",colourbyheightmenu);
            if(colourbyheightmenu)colourbyheightmenu->signal_activate().connect(sigc::ptr_fun(&on_colouractivated));
            refXml->get_widget("colourbyflightlinemenu",colourbyflightlinemenu);
            if(colourbyflightlinemenu)colourbyflightlinemenu->signal_activate().connect(sigc::ptr_fun(&on_colouractivated));
            refXml->get_widget("colourbyclassificationmenu",colourbyclassificationmenu);
            if(colourbyclassificationmenu)colourbyclassificationmenu->signal_activate().connect(sigc::ptr_fun(&on_colouractivated));
            refXml->get_widget("colourbyreturnmenu",colourbyreturnmenu);
            if(colourbyreturnmenu)colourbyreturnmenu->signal_activate().connect(sigc::ptr_fun(&on_colouractivated));
         //For determining how to shade the overview:
            Gtk::RadioMenuItem *brightnessbynonemenu = NULL;
            refXml->get_widget("brightnessbynonemenu",brightnessbynonemenu);
            if(brightnessbynonemenu)brightnessbynonemenu->signal_activate().connect(sigc::ptr_fun(&on_brightnessactivated));
            refXml->get_widget("brightnessbyintensitymenu",brightnessbyintensitymenu);
            if(brightnessbyintensitymenu)brightnessbyintensitymenu->signal_activate().connect(sigc::ptr_fun(&on_brightnessactivated));
            refXml->get_widget("brightnessbyheightmenu",brightnessbyheightmenu);
            if(brightnessbyheightmenu)brightnessbyheightmenu->signal_activate().connect(sigc::ptr_fun(&on_brightnessactivated));
      //Help menu:
         Gtk::MenuItem *aboutmenu = NULL;
         refXml->get_widget("aboutmenu",aboutmenu);
         if(aboutmenu)aboutmenu->signal_activate().connect(sigc::ptr_fun(&on_aboutmenuactivated));
         refXml->get_widget("about",about);
         if(about)about->signal_response().connect(sigc::ptr_fun(&on_aboutresponse));

      //Toolbar:
      Gtk::ToolButton *returnbutton = NULL;//This returns the viewpoint to the initial one for that file or selection of files.
      refXml->get_widget("returnbutton",returnbutton);
      if(returnbutton)returnbutton->signal_clicked().connect(sigc::ptr_fun(&on_returnbutton_clicked));
      //Advanced viewing options:
         Gtk::ToolButton *advancedbutton = NULL;
         refXml->get_widget("advancedbutton",advancedbutton);
         if(advancedbutton)advancedbutton->signal_clicked().connect(sigc::ptr_fun(&on_advancedbutton_clicked));
      //For overview image viewing attributes:
         refXml->get_widget("pointwidthselect",pointwidthselect);
         if(pointwidthselect){
            pointwidthselect->set_range(1,300);//Essentially arbitrary. Would there be any situation where a width greater than 300 pixels would be wanted? Very far future?
            pointwidthselect->set_value(1);
            pointwidthselect->signal_value_changed().connect(sigc::ptr_fun(&on_pointwidthselected));
         }
      //The ruler:
         refXml->get_widget("rulertoggleover",rulertoggleover);
         if(rulertoggleover)rulertoggleover->signal_toggled().connect(sigc::ptr_fun(&on_rulertoggleover));
         refXml->get_widget("rulerlabelover",rulerlabelover);
      //Fence and profile toggles and attribute selectors:
         refXml->get_widget("fencetoggle",fencetoggle);
         if(fencetoggle)fencetoggle->signal_toggled().connect(sigc::ptr_fun(&on_fencetoggle));
         refXml->get_widget("profiletoggle",profiletoggle);
         if(profiletoggle)profiletoggle->signal_toggled().connect(sigc::ptr_fun(&on_profiletoggle));
         refXml->get_widget("orthogonalrectshapetoggle",orthogonalrectshapetoggle);
         if(orthogonalrectshapetoggle)orthogonalrectshapetoggle->signal_toggled().connect(sigc::ptr_fun(&on_orthogonalrectshapetoggle));
         refXml->get_widget("slantedrectshapetoggle",slantedrectshapetoggle);
         if(slantedrectshapetoggle)slantedrectshapetoggle->signal_toggled().connect(sigc::ptr_fun(&on_slantedrectshapetoggle));
         refXml->get_widget("slantwidthselect",slantwidthselect);
         if(slantwidthselect){
            slantwidthselect->set_range(0,30000);//Essentially arbitrary. Would there be any situation where a width greater than 30 km would be wanted?
            slantwidthselect->set_value(5);
            slantwidthselect->signal_value_changed().connect(sigc::ptr_fun(&on_slantwidthselected));
         }

      overviewwindow->show_all();
   }
   else {
      std::cerr << "eep, no main window?" << std::endl;
      return 1;
   }
   refXml->get_widget("profilewindow", profilewindow);
   if(profilewindow){
      profilewindow->set_title("LAG Profile");
      refXml->get_widget("showheightscalecheck",showheightscalecheck);
      if(showheightscalecheck)showheightscalecheck->signal_activate().connect(sigc::ptr_fun(&on_showheightscalecheck));
      //For determining how to colour the profile:
      Gtk::RadioMenuItem *colourbynonemenuprof = NULL;
      refXml->get_widget("colourbynonemenuprof",colourbynonemenuprof);
      if(colourbynonemenuprof)colourbynonemenuprof->signal_activate().connect(sigc::ptr_fun(&on_colouractivatedprof));
      refXml->get_widget("colourbyintensitymenuprof",colourbyintensitymenuprof);
      if(colourbyintensitymenuprof)colourbyintensitymenuprof->signal_activate().connect(sigc::ptr_fun(&on_colouractivatedprof));
      refXml->get_widget("colourbyheightmenuprof",colourbyheightmenuprof);
      if(colourbyheightmenuprof)colourbyheightmenuprof->signal_activate().connect(sigc::ptr_fun(&on_colouractivatedprof));
      refXml->get_widget("colourbyflightlinemenuprof",colourbyflightlinemenuprof);
      if(colourbyflightlinemenuprof)colourbyflightlinemenuprof->signal_activate().connect(sigc::ptr_fun(&on_colouractivatedprof));
      refXml->get_widget("colourbyclassificationmenuprof",colourbyclassificationmenuprof);
      if(colourbyclassificationmenuprof)colourbyclassificationmenuprof->signal_activate().connect(sigc::ptr_fun(&on_colouractivatedprof));
      refXml->get_widget("colourbyreturnmenuprof",colourbyreturnmenuprof);
      if(colourbyreturnmenuprof)colourbyreturnmenuprof->signal_activate().connect(sigc::ptr_fun(&on_colouractivatedprof));

      //For determining how to shade the profile:
      Gtk::RadioMenuItem *brightnessbynonemenuprof = NULL;
      refXml->get_widget("brightnessbynonemenuprof",brightnessbynonemenuprof);
      if(brightnessbynonemenuprof)brightnessbynonemenuprof->signal_activate().connect(sigc::ptr_fun(&on_brightnessactivatedprof));
      refXml->get_widget("brightnessbyintensitymenuprof",brightnessbyintensitymenuprof);
      if(brightnessbyintensitymenuprof)brightnessbyintensitymenuprof->signal_activate().connect(sigc::ptr_fun(&on_brightnessactivatedprof));
      refXml->get_widget("brightnessbyheightmenuprof",brightnessbyheightmenuprof);
      if(brightnessbyheightmenuprof)brightnessbyheightmenuprof->signal_activate().connect(sigc::ptr_fun(&on_brightnessactivatedprof));

      //The fence:
      refXml->get_widget("fencetoggleprof",fencetoggleprof);
      if(fencetoggleprof)fencetoggleprof->signal_toggled().connect(sigc::ptr_fun(&on_fencetoggleprof));
      //The ruler:
      refXml->get_widget("rulertoggle",rulertoggle);
      if(rulertoggle)rulertoggle->signal_toggled().connect(sigc::ptr_fun(&on_rulertoggle));
      refXml->get_widget("rulerlabel",rulerlabel);

      //For overview image viewing attributes:
      refXml->get_widget("pointwidthselectprof",pointwidthselectprof);
      if(pointwidthselectprof){
         pointwidthselectprof->set_range(1,300);//Essentially arbitrary. Would there be any situation where a width greater than 300 pixels would be wanted? Very far future?
         pointwidthselectprof->set_value(2);
         pointwidthselectprof->signal_value_changed().connect(sigc::ptr_fun(&on_pointwidthselectedprof));
      }
      refXml->get_widget("pointshowtoggle",pointshowtoggle);
      if(pointshowtoggle)pointshowtoggle->signal_toggled().connect(sigc::ptr_fun(&on_pointshowtoggle));
      refXml->get_widget("lineshowtoggle",lineshowtoggle);
      if(lineshowtoggle)lineshowtoggle->signal_toggled().connect(sigc::ptr_fun(&on_lineshowtoggle));
      refXml->get_widget("movingaveragerangeselect",movingaveragerangeselect);
      if(movingaveragerangeselect){
         movingaveragerangeselect->set_range(0,30000);//Essentially arbitrary.
         movingaveragerangeselect->set_value(5);
         movingaveragerangeselect->signal_value_changed().connect(sigc::ptr_fun(&on_movingaveragerangeselect));
      }
      Gtk::ToolButton *returnbuttonprof = NULL;
      refXml->get_widget("returnbuttonprof",returnbuttonprof);
      if(returnbuttonprof)returnbuttonprof->signal_clicked().connect(sigc::ptr_fun(&on_returnbuttonprof_clicked));
      Gtk::ToolButton *showprofilebutton = NULL;
      refXml->get_widget("showprofilebutton",showprofilebutton);
      if(showprofilebutton)showprofilebutton->signal_clicked().connect(sigc::ptr_fun(&on_showprofilebutton_clicked));
      Gtk::ToolButton *classbutton = NULL;
      refXml->get_widget("classbutton",classbutton);
      if(classbutton)classbutton->signal_clicked().connect(sigc::ptr_fun(&on_classbutton_clicked));
      refXml->get_widget("orthogonalprof",orthogonalprof);
      if(orthogonalprof)orthogonalprof->signal_toggled().connect(sigc::ptr_fun(&on_orthogonalprof));
      refXml->get_widget("slantedprof",slantedprof);
      if(slantedprof)slantedprof->signal_toggled().connect(sigc::ptr_fun(&on_slantedprof));
      refXml->get_widget("slantwidthselectprof",slantwidthselectprof);
      if(slantwidthselectprof){
         slantwidthselectprof->set_range(0,30000);//Essentially arbitrary. Would there be any situation where a width greater than 30 km would be wanted?
         slantwidthselectprof->set_value(5);
         slantwidthselectprof->signal_value_changed().connect(sigc::ptr_fun(&on_slantwidthselectedprof));
      }

      profilewindow->show_all();
   }
   Glib::RefPtr<Gdk::GL::Config> glconfig;//Creating separate configs for each window. Is this really necessary? It does not do anything yet, but hopefully will form a nucleus to the solution to the shared viewport problem.
   glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |      Gdk::GL::MODE_DEPTH  |     Gdk::GL::MODE_DOUBLE);
   if (glconfig==NULL){
      glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |    Gdk::GL::MODE_DEPTH);
      if(glconfig==NULL){
         cout << "Cannot intialise OpenGL. Exiting." << endl;
         std::exit(1);
      }
   }
   int bucketlimit = 200000;//How many points in each bucket, maximum.
   quadtree* lidardata = NULL;//The flightlines are stored here.
   tdo = new TwoDeeOverview(glconfig,lidardata,bucketlimit,rulerlabelover);
   tdo->set_size_request(200,200);
   //Initialisations:
   tdo->setshowprofile(showprofilecheck->get_active());
   tdo->setshowfence(showfencecheck->get_active());
   tdo->setshowdistancescale(showdistancescalecheck->get_active());
   tdo->setshowlegend(showlegendcheck->get_active());
   tdo->setreversez(reverseheightcheck->get_active());
   tdo->setintensitycolour(colourbyintensitymenu->get_active());
   tdo->setheightcolour(colourbyheightmenu->get_active());
   tdo->setlinecolour(colourbyflightlinemenu->get_active());
   tdo->setclasscolour(colourbyclassificationmenu->get_active());
   tdo->setreturncolour(colourbyreturnmenu->get_active());
   tdo->setintensitybrightness(brightnessbyintensitymenu->get_active());
   tdo->setheightbrightness(brightnessbyheightmenu->get_active());
   tdo->setpointwidth(pointwidthselect->get_value());
   tdo->getprofbox()->setslantwidth(slantwidthselect->get_value());
   tdo->getfencebox()->setslantwidth(slantwidthselect->get_value());
   tdo->getprofbox()->setslantedshape(slantedrectshapetoggle->get_active());
   tdo->getfencebox()->setslantedshape(slantedrectshapetoggle->get_active());
   tdo->getprofbox()->setorthogonalshape(orthogonalrectshapetoggle->get_active());
   tdo->getfencebox()->setorthogonalshape(orthogonalrectshapetoggle->get_active());
   showlegendcheck->set_inconsistent(!colourbyintensitymenu->get_active() && !colourbyheightmenu->get_active() && !colourbyclassificationmenu->get_active() && !colourbyreturnmenu->get_active());//This is to help prevent confusion when the user decides to show the legend and nothing happens because of there being no legend when colouring by flightline or by none.
   Glib::RefPtr<Gdk::GL::Config> glconfig2;//Creating separate configs for each window. Is this really necessary? It does not do anything yet, but hopefully will form a nucleus to the solution to the shared viewport problem.
   glconfig2 = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |      Gdk::GL::MODE_DEPTH  |     Gdk::GL::MODE_DOUBLE);
   if (glconfig2==NULL){
      glconfig2 = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |    Gdk::GL::MODE_DEPTH);
      if(glconfig2==NULL){
         cout << "Cannot intialise OpenGL. Exiting." << endl;
         std::exit(1);
      }
   }
   prof = new Profile(glconfig2,lidardata,bucketlimit,rulerlabel);
   prof->set_size_request(200,200);
   //Initialisations:
   prof->setshowheightscale(showheightscalecheck->get_active());
   prof->setintensitycolour(colourbyintensitymenuprof->get_active());
   prof->setheightcolour(colourbyheightmenuprof->get_active());
   prof->setlinecolour(colourbyflightlinemenuprof->get_active());
   prof->setclasscolour(colourbyclassificationmenuprof->get_active());
   prof->setreturncolour(colourbyreturnmenuprof->get_active());
   prof->setintensitybrightness(brightnessbyintensitymenuprof->get_active());
   prof->setheightbrightness(brightnessbyheightmenuprof->get_active());
   prof->setpointwidth(pointwidthselectprof->get_value());
   prof->setdrawpoints(pointshowtoggle->get_active());
   prof->setdrawmovingaverage(lineshowtoggle->get_active());
   prof->setmavrgrange(movingaveragerangeselect->get_value());
   prof->setslantwidth(slantwidthselectprof->get_value());
   prof->setslantwidth(slantwidthselectprof->get_value());
   prof->setslanted(slantedprof->get_active());
   aow = new AdvancedOptionsWindow(tdo,prof,refXml);
   fs = new FileSaver(tdo,prof,refXml,lidardata);
   Gtk::EventBox *eventboxtdo = NULL;//Contains the overview.
   refXml->get_widget("eventboxtdo",eventboxtdo);
   eventboxtdo->signal_key_press_event().connect(sigc::ptr_fun(&on_tdo_key_press));
   Gtk::EventBox *eventboxprof = NULL;//Contains the profile.
   refXml->get_widget("eventboxprof",eventboxprof);
   eventboxprof->signal_key_press_event().connect(sigc::ptr_fun(&on_prof_key_press));
   fo = new FileOpener(tdo,prof,refXml,aow,fs,lidardata,bucketlimit,eventboxtdo,eventboxprof);
   fo->testfilename(argc,argv,true,false);//In case of command-line commands.
   gtkmain.run(*overviewwindow);
   delete fo;
   delete fs;
   delete aow;
   if(lidardata != NULL)delete lidardata;
   return 0;
}

/*
int main(int argc, char** argv) {
   cout << "Build number: 2010.06.10.1" << endl;
   exename.append(argv[0]);//Record the program name.
   int returnvalue = GUIset(argc, argv);//Make the GUI.
   if(tdo!=NULL)delete tdo;
   if(prof != NULL)delete prof;
   return returnvalue;
}
*/