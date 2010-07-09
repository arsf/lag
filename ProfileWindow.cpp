/*
 * File: ProfileWindow.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: June 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <vector>
#include "ProfileWindow.h"

ProfileWindow::ProfileWindow(Profile *prof,TwoDeeOverview *tdo,Gtk::Window *profilewindow,Gtk::EventBox *eventboxprof,Glib::RefPtr<Gnome::Glade::Xml> refXml,AdvancedOptionsWindow *aow){
   this->prof = prof;
   this->tdo = tdo;
   this->profilewindow = profilewindow;
   this->eventboxprof = eventboxprof;
   this->aow = aow;
   eventboxprof->signal_key_press_event().connect(sigc::mem_fun(*this,&ProfileWindow::on_prof_key_press));
   profilewindow->set_title("LAG Profile");
   refXml->get_widget("showheightscalecheck",showheightscalecheck);
   if(showheightscalecheck)showheightscalecheck->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_showheightscalecheck));
   //For determining how to colour the profile:
   Gtk::RadioMenuItem *colourbynonemenuprof = NULL;
   refXml->get_widget("colourbynonemenuprof",colourbynonemenuprof);
   if(colourbynonemenuprof)colourbynonemenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_colouractivatedprof));
   refXml->get_widget("colourbyintensitymenuprof",colourbyintensitymenuprof);
   if(colourbyintensitymenuprof)colourbyintensitymenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_colouractivatedprof));
   refXml->get_widget("colourbyheightmenuprof",colourbyheightmenuprof);
   if(colourbyheightmenuprof)colourbyheightmenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_colouractivatedprof));
   refXml->get_widget("colourbyflightlinemenuprof",colourbyflightlinemenuprof);
   if(colourbyflightlinemenuprof)colourbyflightlinemenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_colouractivatedprof));
   refXml->get_widget("colourbyclassificationmenuprof",colourbyclassificationmenuprof);
   if(colourbyclassificationmenuprof)colourbyclassificationmenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_colouractivatedprof));
   refXml->get_widget("colourbyreturnmenuprof",colourbyreturnmenuprof);
   if(colourbyreturnmenuprof)colourbyreturnmenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_colouractivatedprof));

   //For determining how to shade the profile:
   Gtk::RadioMenuItem *brightnessbynonemenuprof = NULL;
   refXml->get_widget("brightnessbynonemenuprof",brightnessbynonemenuprof);
   if(brightnessbynonemenuprof)brightnessbynonemenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_brightnessactivatedprof));
   refXml->get_widget("brightnessbyintensitymenuprof",brightnessbyintensitymenuprof);
   if(brightnessbyintensitymenuprof)brightnessbyintensitymenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_brightnessactivatedprof));
   refXml->get_widget("brightnessbyheightmenuprof",brightnessbyheightmenuprof);
   if(brightnessbyheightmenuprof)brightnessbyheightmenuprof->signal_activate().connect(sigc::mem_fun(*this,&ProfileWindow::on_brightnessactivatedprof));

   //The fence:
   refXml->get_widget("fencetoggleprof",fencetoggleprof);
   if(fencetoggleprof)fencetoggleprof->signal_toggled().connect(sigc::mem_fun(*this,&ProfileWindow::on_fencetoggleprof));
   //The ruler:
   refXml->get_widget("rulertoggle",rulertoggle);
   if(rulertoggle)rulertoggle->signal_toggled().connect(sigc::mem_fun(*this,&ProfileWindow::on_rulertoggle));

   //For overview image viewing attributes:
   refXml->get_widget("pointwidthselectprof",pointwidthselectprof);
   if(pointwidthselectprof){
      pointwidthselectprof->set_range(1,300);//Essentially arbitrary. Would there be any situation where a width greater than 300 pixels would be wanted? Very far future?
      pointwidthselectprof->set_value(2);
      pointwidthselectprof->signal_value_changed().connect(sigc::mem_fun(*this,&ProfileWindow::on_pointwidthselectedprof));
   }
   refXml->get_widget("pointshowtoggle",pointshowtoggle);
   if(pointshowtoggle)pointshowtoggle->signal_toggled().connect(sigc::mem_fun(*this,&ProfileWindow::on_pointshowtoggle));
   refXml->get_widget("lineshowtoggle",lineshowtoggle);
   if(lineshowtoggle)lineshowtoggle->signal_toggled().connect(sigc::mem_fun(*this,&ProfileWindow::on_lineshowtoggle));
   refXml->get_widget("movingaveragerangeselect",movingaveragerangeselect);
   if(movingaveragerangeselect){
      movingaveragerangeselect->set_range(0,30000);//Essentially arbitrary.
      movingaveragerangeselect->set_value(5);
      movingaveragerangeselect->signal_value_changed().connect(sigc::mem_fun(*this,&ProfileWindow::on_movingaveragerangeselect));
   }
   Gtk::ToolButton *returnbuttonprof = NULL;
   refXml->get_widget("returnbuttonprof",returnbuttonprof);
   if(returnbuttonprof)returnbuttonprof->signal_clicked().connect(sigc::mem_fun(*this,&ProfileWindow::on_returnbuttonprof_clicked));
   Gtk::ToolButton *showprofilebutton = NULL;
   refXml->get_widget("showprofilebutton",showprofilebutton);
   if(showprofilebutton)showprofilebutton->signal_clicked().connect(sigc::mem_fun(*this,&ProfileWindow::on_showprofilebutton_clicked));
   Gtk::ToolButton *classbutton = NULL;
   refXml->get_widget("classbutton",classbutton);
   if(classbutton)classbutton->signal_clicked().connect(sigc::mem_fun(*this,&ProfileWindow::on_classbutton_clicked));
   refXml->get_widget("orthogonalprof",orthogonalprof);
   if(orthogonalprof)orthogonalprof->signal_toggled().connect(sigc::mem_fun(*this,&ProfileWindow::on_orthogonalprof));
   refXml->get_widget("slantedprof",slantedprof);
   if(slantedprof)slantedprof->signal_toggled().connect(sigc::mem_fun(*this,&ProfileWindow::on_slantedprof));
   refXml->get_widget("slantwidthselectprof",slantwidthselectprof);
   if(slantwidthselectprof){
      slantwidthselectprof->set_range(0,30000);//Essentially arbitrary. Would there be any situation where a width greater than 30 km would be wanted?
      slantwidthselectprof->set_value(5);
      slantwidthselectprof->signal_value_changed().connect(sigc::mem_fun(*this,&ProfileWindow::on_slantwidthselectedprof));
   }
   refXml->get_widget("classificationselect",classificationselect);
   profilewindow->show_all();
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
}
ProfileWindow::~ProfileWindow(){
   delete classificationselect;
   delete showheightscalecheck;
   delete colourbyintensitymenuprof;
   delete colourbyheightmenuprof;
   delete colourbyflightlinemenuprof;
   delete colourbyclassificationmenuprof;
   delete colourbyreturnmenuprof;
   delete brightnessbyintensitymenuprof;
   delete brightnessbyheightmenuprof;
   delete pointwidthselectprof;
   delete pointshowtoggle;
   delete lineshowtoggle;
   delete movingaveragerangeselect;
   delete fencetoggleprof;
   delete rulertoggle;
   delete slantwidthselectprof;
   delete orthogonalprof;
   delete slantedprof;
}

//When toggled, the height scale is shown on the profile.
void ProfileWindow::on_showheightscalecheck(){
   prof->setshowheightscale(showheightscalecheck->get_active());
   if(prof->is_realized())prof->drawviewable(2);
}
//Does the same as on_colouractivated, except for the profile.
void ProfileWindow::on_colouractivatedprof(){
   prof->setintensitycolour(colourbyintensitymenuprof->get_active());
   prof->setheightcolour(colourbyheightmenuprof->get_active());
   prof->setlinecolour(colourbyflightlinemenuprof->get_active());
   prof->setclasscolour(colourbyclassificationmenuprof->get_active());
   prof->setreturncolour(colourbyreturnmenuprof->get_active());
   if(prof->is_realized())prof->drawviewable(1);
}
//Does the same as on_brightnessactivated, except for the profile.
void ProfileWindow::on_brightnessactivatedprof(){
   prof->setintensitybrightness(brightnessbyintensitymenuprof->get_active());
   prof->setheightbrightness(brightnessbyheightmenuprof->get_active());
   if(prof->is_realized())prof->drawviewable(1);
}

//This grabs the profile from the overview.
void ProfileWindow::on_showprofilebutton_clicked(){
   if(tdo->is_realized())profilewindow->present();
   double *profxs = NULL,*profys = NULL;//These are NOT to be deleted here as the arrays they will point to will be managed by the TwoDeeOVerview object.
   int profps = 0;
   if(tdo->is_realized())tdo->getprofile(profxs,profys,profps);
   if(profxs!=NULL&&profys!=NULL){
      tdo->setpausethread(true);//Showprofile uses the getpoint() method, and that must never be used by more than one thread at once.
      tdo->waitforpause();
      prof->showprofile(profxs,profys,profps,true);
      tdo->setpausethread(false);
   }
}
//This returns the profile to its original position.
void ProfileWindow::on_returnbuttonprof_clicked(){
   if(prof->is_realized())prof->returntostart();
}
//Does the same as on_pointwidthselected, except for the profile.
void ProfileWindow::on_pointwidthselectedprof(){
   prof->setpointwidth(pointwidthselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(2);
}
//Determines whether to display the points on the profile.
void ProfileWindow::on_pointshowtoggle(){
   prof->setdrawpoints(pointshowtoggle->get_active());
   if(prof->is_realized())prof->drawviewable(1);
}
//Determines whether to display the (best fit) lines on the profile.
void ProfileWindow::on_lineshowtoggle(){
   prof->setdrawmovingaverage(lineshowtoggle->get_active());
   if(prof->is_realized())prof->drawviewable(1);
}
//The best fit is a moving average, and this changes the range, and therefore the shape of the line.
void ProfileWindow::on_movingaveragerangeselect(){
   prof->setmavrgrange(movingaveragerangeselect->get_value());
   prof->make_moving_average();
   if(prof->is_realized())prof->drawviewable(1);
}
//This classifies the points surrounded by the fence.
void ProfileWindow::on_classbutton_clicked(){
   tdo->setpausethread(true);//Nothing else must read the points (or indeed write to them!) while the classifier is writing to them. Also, it uses the getpoint() method.
   tdo->waitforpause();
   if(prof->is_realized())prof->classify(classificationselect->get_value_as_int());
   tdo->setpausethread(false);
   tdo->drawviewable(1);
}
//Toggles whether clicking and dragging will select the fence in the profile.
void ProfileWindow::on_fencetoggleprof(){
   if(fencetoggleprof->get_active()){
      if(rulertoggle->get_active())rulertoggle->set_active(false);
      prof->setupfence();
      if(prof->is_realized())prof->drawviewable(1);
   }
   else{
      prof->unsetupfence();
      if(prof->is_realized()&&!rulertoggle->get_active()&&!fencetoggleprof->get_active())prof->drawviewable(1);
   }
}
void ProfileWindow::on_slantwidthselectedprof(){
   prof->setslantwidth(slantwidthselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(1);
}
void ProfileWindow::on_orthogonalprof(){
   if(orthogonalprof->get_active())if(slantedprof->get_active())slantedprof->set_active(false);
   if(!orthogonalprof->get_active())if(!slantedprof->get_active())slantedprof->set_active(true);
   prof->setslanted(!orthogonalprof->get_active());
   if(prof->is_realized())prof->drawviewable(1);
}
void ProfileWindow::on_slantedprof(){
   if(slantedprof->get_active())if(orthogonalprof->get_active())orthogonalprof->set_active(false);
   if(!slantedprof->get_active())if(!orthogonalprof->get_active())orthogonalprof->set_active(true);
   prof->setslanted(slantedprof->get_active());
   if(prof->is_realized())prof->drawviewable(1);
}
//When toggled, the profile view goes into rulering mode. When untoggled, rulering mode ends.
void ProfileWindow::on_rulertoggle(){
   if(rulertoggle->get_active()){
      if(fencetoggleprof->get_active())fencetoggleprof->set_active(false);
      prof->setupruler();
      if(prof->is_realized())prof->drawviewable(1);
   }
   else{
      prof->unsetupruler();
      if(prof->is_realized()&&!rulertoggle->get_active()&&!fencetoggleprof->get_active())prof->drawviewable(1);
   }
}


bool ProfileWindow::on_prof_key_press(GdkEventKey* event){
   switch(event->keyval){
      case GDK_P:case GDK_p:case GDK_space:on_showprofilebutton_clicked();return true;break;
      case GDK_C:case GDK_c:case GDK_K:case GDK_k:case GDK_Return:on_classbutton_clicked();return true;break;
      case GDK_w:case GDK_s:case GDK_a:case GDK_d:return prof->on_pan_key(event,aow->getmovespeed());break;
      case GDK_W:case GDK_S:case GDK_A:case GDK_D:
         if(fencetoggleprof->get_active())return prof->on_fence_key(event,aow->getmovespeed());
         else return false;
         break;
      case GDK_r:case GDK_v:case GDK_q:case GDK_e:
         case GDK_R:case GDK_V:case GDK_Q:case GDK_E:return on_profile_shift(event);break;
      case GDK_i:case GDK_o:case GDK_I:case GDK_O:return prof->on_zoom_key(event);break;
      case GDK_z:case GDK_Z:return prof->drawviewable(1);break;
      case GDK_f:case GDK_F:fencetoggleprof->set_active(!fencetoggleprof->get_active());return true;break;
      case GDK_t:case GDK_T:slantedprof->set_active(!slantedprof->get_active());return true;break;
      default:return false;break;
   }
}

bool ProfileWindow::on_profile_shift(GdkEventKey* event){
   switch(event->keyval){
      case GDK_r:case GDK_R:event->keyval = GDK_W;break;
      case GDK_v:case GDK_V:event->keyval = GDK_S;break;
      case GDK_q:case GDK_Q:event->keyval = GDK_A;break;
      case GDK_e:case GDK_E:event->keyval = GDK_D;break;
      default:return false;break;
   }
   bool shifted = tdo->on_prof_key(event,aow->getmovespeed(),true);
   if(!shifted)return shifted;
   if(tdo->is_realized())profilewindow->present();
   double *profxs = NULL,*profys = NULL;//These are NOT to be deleted here as the arrays they will point to will be managed by the TwoDeeOVerview object.
   int profps = 0;
   if(tdo->is_realized())tdo->getprofile(profxs,profys,profps);
   if(profxs!=NULL&&profys!=NULL){
      shifted = prof->shift_viewing_parameters(event,aow->getmovespeed());
      if(!shifted)return shifted;
      tdo->setpausethread(true);//Showprofile uses the getpoint() method, and that must never be used by more than one thread at once.
      tdo->waitforpause();
      prof->showprofile(profxs,profys,profps,false);
      tdo->setpausethread(false);
      return true;
   }
   return false;
}

