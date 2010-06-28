/*
 * File: AdvancedOptionsWindow.cpp
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
#include "AdvancedOptionsWindow.h"

AdvancedOptionsWindow::AdvancedOptionsWindow(TwoDeeOverview *tdo,Profile *prof,Glib::RefPtr<Gnome::Glade::Xml> refXml){
   this->tdo = tdo;
   this->prof = prof;
   refXml->get_widget("advancedoptionsdialog",advancedoptionsdialog);
   if(advancedoptionsdialog)advancedoptionsdialog->signal_response().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_advancedoptionsdialog_response));
   //False elevation:
      refXml->get_widget("classcheckbutton0",classcheckbutton0);
      if(classcheckbutton0)classcheckbutton0->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton0_toggled));
      refXml->get_widget("classcheckbutton2",classcheckbutton2);
      if(classcheckbutton2)classcheckbutton2->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton2_toggled));
      refXml->get_widget("classcheckbutton3",classcheckbutton3);
      if(classcheckbutton3)classcheckbutton3->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton3_toggled));
      refXml->get_widget("classcheckbutton4",classcheckbutton4);
      if(classcheckbutton4)classcheckbutton4->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton4_toggled));
      refXml->get_widget("classcheckbutton5",classcheckbutton5);
      if(classcheckbutton5)classcheckbutton5->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton5_toggled));
      refXml->get_widget("classcheckbutton6",classcheckbutton6);
      if(classcheckbutton6)classcheckbutton6->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton6_toggled));
      refXml->get_widget("classcheckbutton7",classcheckbutton7);
      if(classcheckbutton7)classcheckbutton7->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton7_toggled));
      refXml->get_widget("classcheckbutton8",classcheckbutton8);
      if(classcheckbutton8)classcheckbutton8->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton8_toggled));
      refXml->get_widget("classcheckbutton9",classcheckbutton9);
      if(classcheckbutton9)classcheckbutton9->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton9_toggled));
      refXml->get_widget("classcheckbutton12",classcheckbutton12);
      if(classcheckbutton12)classcheckbutton12->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbutton12_toggled));
      refXml->get_widget("classcheckbuttonA",classcheckbuttonA);
      if(classcheckbuttonA)classcheckbuttonA->signal_toggled().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_classcheckbuttonA_toggled));
   //Height and intensity threasholding and brightness modifiers:
      refXml->get_widget("heightmaxselect",heightmaxselect);
      if(heightmaxselect)heightmaxconn = heightmaxselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_heightmaxselect_changed));
      refXml->get_widget("heightminselect",heightminselect);
      if(heightminselect)heightminconn = heightminselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_heightminselect_changed));
      refXml->get_widget("heightscrollbar",heightscrollbar);
      if(heightscrollbar)heightscrollbar->signal_change_value().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_heightscrollbar_scrolled));
      refXml->get_widget("heightoffsetselect",heightoffsetselect);
      if(heightoffsetselect)heightoffsetselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_heightoffsetselect_changed));
      refXml->get_widget("heightfloorselect",heightfloorselect);
      if(heightfloorselect)heightfloorselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_heightfloorselect_changed));
      refXml->get_widget("intensitymaxselect",intensitymaxselect);
      if(intensitymaxselect)intensitymaxconn = intensitymaxselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_intensitymaxselect_changed));
      refXml->get_widget("intensityminselect",intensityminselect);
      if(intensityminselect)intensityminconn = intensityminselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_intensityminselect_changed));
      refXml->get_widget("intensityscrollbar",intensityscrollbar);
      if(intensityscrollbar)intensityscrollbar->signal_change_value().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_intensityscrollbar_scrolled));
      refXml->get_widget("intensityoffsetselect",intensityoffsetselect);
      if(intensityoffsetselect)intensityoffsetselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_intensityoffsetselect_changed));
      refXml->get_widget("intensityfloorselect",intensityfloorselect);
      if(intensityfloorselect)intensityfloorselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_intensityfloorselect_changed));
      refXml->get_widget("drawingresetbutton",drawingresetbutton);
      if(drawingresetbutton)drawingresetbutton->signal_clicked().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_drawingresetbutton_clicked));
   //Detail (points to skip) level:
      refXml->get_widget("maindetailselect",maindetailselect);
      if(maindetailselect){
         maindetailselect->set_range(0,300);//Essentially arbitrary. Would there be any situation where such a coarse detail level as 300 pixels would be wanted?
         maindetailselect->set_value(0.00);
         maindetailselect->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_maindetailselected));
      }
      refXml->get_widget("maindetailselectprof",maindetailselectprof);
      if(maindetailselectprof){
         maindetailselectprof->set_range(0,300);//Essentially arbitrary. Would there be any situation where such a coarse detail level as 300 pixels would be wanted?
         maindetailselectprof->set_value(0);
         maindetailselectprof->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_maindetailselectedprof));
      }
      refXml->get_widget("previewdetailselectprof",previewdetailselectprof);
      if(previewdetailselectprof){
         previewdetailselectprof->set_range(0,300);//Essentially arbitrary. Would there be any situation where such a coarse detail level as 300 pixels would be wanted?
         previewdetailselectprof->set_value(0);
         previewdetailselectprof->signal_value_changed().connect(sigc::mem_fun(*this,&AdvancedOptionsWindow::on_previewdetailselectedprof));
      }
      tdo->setmaindetail(maindetailselect->get_value());
      prof->setmaindetail(maindetailselectprof->get_value());
      prof->setpreviewdetail(previewdetailselectprof->get_value());
}

AdvancedOptionsWindow::~AdvancedOptionsWindow(){
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
   delete advancedoptionsdialog;//Have to delete parent after children?
}

//Closes the advanced options dialog.
void AdvancedOptionsWindow::on_advancedoptionsdialog_response(int response_id){ advancedoptionsdialog->hide(); }
//The checkbuttons and their activations:
void AdvancedOptionsWindow::on_classcheckbutton0_toggled(){ tdo->setheightenNonC(classcheckbutton0->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void AdvancedOptionsWindow::on_classcheckbutton2_toggled(){ tdo->setheightenGround(classcheckbutton2->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void AdvancedOptionsWindow::on_classcheckbutton3_toggled(){ tdo->setheightenLowVeg(classcheckbutton3->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void AdvancedOptionsWindow::on_classcheckbutton4_toggled(){ tdo->setheightenMedVeg(classcheckbutton4->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void AdvancedOptionsWindow::on_classcheckbutton5_toggled(){ tdo->setheightenHighVeg(classcheckbutton5->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void AdvancedOptionsWindow::on_classcheckbutton6_toggled(){ tdo->setheightenBuildings(classcheckbutton6->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void AdvancedOptionsWindow::on_classcheckbutton7_toggled(){ tdo->setheightenNoise(classcheckbutton7->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void AdvancedOptionsWindow::on_classcheckbutton8_toggled(){ tdo->setheightenMass(classcheckbutton8->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void AdvancedOptionsWindow::on_classcheckbutton9_toggled(){ tdo->setheightenWater(classcheckbutton9->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void AdvancedOptionsWindow::on_classcheckbutton12_toggled(){ tdo->setheightenOverlap(classcheckbutton12->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void AdvancedOptionsWindow::on_classcheckbuttonA_toggled(){ tdo->setheightenUndefined(classcheckbuttonA->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
//The drawing settings:
void AdvancedOptionsWindow::changecoloursandshades(){
   //Please note that there is a reason why the profile is updated before the overview: if it is the other way around then the overview's drawing thread would be running so it will be unpredictable which part will execute opengl code first, which can sometimes mean that the overview will be drawn the same size as the profile, which might confuse users:
   if(prof->is_realized())prof->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(prof->is_realized())prof->drawviewable(1);
   if(tdo->is_realized())tdo->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(tdo->is_realized())tdo->drawviewable(1);
}
void AdvancedOptionsWindow::on_heightmaxselect_changed(){
   heightminselect->set_range(tdo->getrminz(),heightmaxselect->get_value()-0.01);
   heightscrollbar->get_adjustment()->set_page_size(heightmaxselect->get_value() - heightminselect->get_value());//Want the scrollbar's slider length to correlate with the range we define.
   heightscrollbar->set_increments(1,heightmaxselect->get_value() - heightminselect->get_value());
   changecoloursandshades();
}
void AdvancedOptionsWindow::on_heightminselect_changed(){
   heightmaxselect->set_range(heightminselect->get_value()+0.01,tdo->getrmaxz());
   heightscrollbar->set_value(heightminselect->get_value());//Value for the scrollbar only changed here as upper value is defined by this value and the page size.
   heightscrollbar->get_adjustment()->set_page_size(heightmaxselect->get_value() - heightminselect->get_value());//Want the scrollbar's slider length to correlate with the range we define.
   heightscrollbar->set_increments(1,heightmaxselect->get_value() - heightminselect->get_value());
   changecoloursandshades();
}
bool AdvancedOptionsWindow::on_heightscrollbar_scrolled(Gtk::ScrollType scroll,double new_value){
   if(new_value + heightscrollbar->get_adjustment()->get_page_size() > heightscrollbar->get_adjustment()->get_upper())new_value = heightscrollbar->get_adjustment()->get_upper() - heightscrollbar->get_adjustment()->get_page_size();//New upper value (new_value plus page size) must not exceed the maximum possible value, otherwise it might mess things up when used to set the ranges, below.
   if(new_value == heightminselect->get_value())return true;
   heightmaxconn.block();//Letting these signals continue would complicate things significantly, as they would then try to set this scrollbar's properties.
   heightminconn.block();//...
   heightmaxselect->set_range(new_value+0.01,tdo->getrmaxz());//Set the new ranges first so that the old ranges cannot clamp the new values.
   heightminselect->set_range(tdo->getrminz(),new_value + heightscrollbar->get_adjustment()->get_page_size()-0.01);//...
   heightmaxselect->set_value(new_value + heightscrollbar->get_adjustment()->get_page_size());
   heightminselect->set_value(new_value);
   heightmaxconn.unblock();
   heightminconn.unblock();
   changecoloursandshades();
   return true;
}
void AdvancedOptionsWindow::on_heightoffsetselect_changed(){
   tdo->setzoffset(heightoffsetselect->get_value());
   prof->setzoffset(heightoffsetselect->get_value());
   changecoloursandshades();
}
void AdvancedOptionsWindow::on_heightfloorselect_changed(){
   tdo->setzfloor(heightfloorselect->get_value());
   prof->setzfloor(heightfloorselect->get_value());
   changecoloursandshades();
}
void AdvancedOptionsWindow::on_intensitymaxselect_changed(){
   intensityminselect->set_range(tdo->getrminintensity(),intensitymaxselect->get_value()-1);
   intensityscrollbar->get_adjustment()->set_page_size(intensitymaxselect->get_value() - intensityminselect->get_value());//Want the scrollbar's slider length to correlate with the range we define.
   intensityscrollbar->set_increments(1,intensitymaxselect->get_value() - intensityminselect->get_value());
   changecoloursandshades();
}
void AdvancedOptionsWindow::on_intensityminselect_changed(){
   intensitymaxselect->set_range(intensityminselect->get_value()+1,tdo->getrmaxintensity());
   intensityscrollbar->set_value(intensityminselect->get_value());//Value for the scrollbar only changed here as upper value is defined by this value and the page size.
   intensityscrollbar->get_adjustment()->set_page_size(intensitymaxselect->get_value() - intensityminselect->get_value());//Want the scrollbar's slider length to correlate with the range we define.
   intensityscrollbar->set_increments(1,intensitymaxselect->get_value() - intensityminselect->get_value());
   changecoloursandshades();
}
bool AdvancedOptionsWindow::on_intensityscrollbar_scrolled(Gtk::ScrollType scroll,double new_value){
   if(new_value + intensityscrollbar->get_adjustment()->get_page_size() > intensityscrollbar->get_adjustment()->get_upper())new_value = intensityscrollbar->get_adjustment()->get_upper() - intensityscrollbar->get_adjustment()->get_page_size();//New upper value (new_value plus page size) must not exceed the maximum possible value, otherwise it might mess things up when used to set the ranges, below.
   if(new_value == intensityminselect->get_value())return true;
   intensitymaxconn.block();//Letting these signals continue would complicate things significantly, as they would then try to set this scrollbar's properties.
   intensityminconn.block();//...
   intensitymaxselect->set_range(intensityminselect->get_value()+1,tdo->getrmaxintensity());//Set the new ranges first so that the old ranges cannot clamp the new values.
   intensityminselect->set_range(tdo->getrminintensity(),intensitymaxselect->get_value()-1);//...
   intensitymaxselect->set_value(new_value + intensityscrollbar->get_adjustment()->get_page_size());
   intensityminselect->set_value(new_value);
   intensitymaxconn.unblock();
   intensityminconn.unblock();
   changecoloursandshades();
   return true;
}
void AdvancedOptionsWindow::on_intensityoffsetselect_changed(){
   tdo->setintensityoffset(intensityoffsetselect->get_value());
   prof->setintensityoffset(intensityoffsetselect->get_value());
   changecoloursandshades();
}
void AdvancedOptionsWindow::on_intensityfloorselect_changed(){
   tdo->setintensityfloor(intensityfloorselect->get_value());
   prof->setintensityfloor(intensityfloorselect->get_value());
   changecoloursandshades();
}
//This resets the advanced colouring and shading options to the values indicated by the drawing objects.
void AdvancedOptionsWindow::on_drawingresetbutton_clicked(){
   heightmaxselect->set_range(tdo->getrminz()+0.01,tdo->getrmaxz());
   heightmaxselect->set_value(tdo->getrmaxz());
   heightminselect->set_range(tdo->getrminz(),tdo->getrmaxz()-0.01);
   heightminselect->set_value(tdo->getrminz());
   heightscrollbar->set_range(tdo->getrminz(),tdo->getrmaxz());
   heightscrollbar->set_value(tdo->getrminz());
   heightscrollbar->get_adjustment()->set_page_size(tdo->getrmaxz()-tdo->getrminz());
   heightscrollbar->set_increments(0.01,tdo->getrmaxz()-tdo->getrminz());
   heightoffsetselect->set_value(0);
   heightfloorselect->set_value(0);
   intensitymaxselect->set_range(tdo->getrminintensity()+1,tdo->getrmaxintensity());
   intensitymaxselect->set_value(tdo->getrmaxintensity());
   intensityminselect->set_range(tdo->getrminintensity(),tdo->getrmaxintensity()-1);
   intensityminselect->set_value(tdo->getrminintensity());
   intensityscrollbar->set_range(tdo->getrminintensity(),tdo->getrmaxintensity());
   intensityscrollbar->set_value(tdo->getrminintensity());
   intensityscrollbar->get_adjustment()->set_page_size(tdo->getrmaxintensity()-tdo->getrminintensity());
   intensityscrollbar->set_increments(1,tdo->getrmaxintensity()-tdo->getrminintensity());
   intensityoffsetselect->set_value(0);
   intensityfloorselect->set_value(0);
   //Draws as a result of the other callbacks, and only does so once because of threading (!!!!), so it may be prudent to change this in the future so that there is only ever one call.
}
//This indirectly determines how many points are skipped when viewing the main overview image. I.e. this affects it as well as the number of visible buckets.
void AdvancedOptionsWindow::on_maindetailselected(){
   int before = tdo->makeresolutionindex();
   tdo->setmaindetail(maindetailselect->get_value());
   tdo->makedetail();
   int after = tdo->makeresolutionindex();
   if(tdo->is_realized() && before != after)tdo->drawviewable(1);
}
//Does the same as on_maindetailselected, except for the profile.
void AdvancedOptionsWindow::on_maindetailselectedprof(){
   prof->setmaindetail(maindetailselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(1);
}
//Does the same as on_maindetailselectedprof, except for the preview of the profile.
void AdvancedOptionsWindow::on_previewdetailselectedprof(){
   prof->setpreviewdetail(previewdetailselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(2);
}
