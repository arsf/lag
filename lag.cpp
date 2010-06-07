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
#include "LASloader.h"
#include "LASsaver.h"
#include "ASCIIloader.h"
#include "TwoDeeOverview.h"
#include "Profile.h"
using namespace std;

TwoDeeOverview *tdo = NULL;//The 2d overview.
Profile *prof = NULL;//The profile.
string exename = "";//The path of the executable.
bool drawwhentoggled = true;//This variable prevents the image(s) from being drawn twice as a result of toggling a radio button (or similar), which deactivates (and therefore toggles again) another one in the same group. This variable must start as true, as the methods make it opposite before using it, so that things will de drawn after the second "toggling".

//Data handling:
int bucketlimit = 100000;//How many points in each bucket, maximum.
int cachelimit = 25000000;//How many points to hold in cache. 1 GB ~= 25000000 points.
bool loadedanyfiles = false;//Whether or not any files have already been loaded in this session.
//Quadtree:
quadtree* lidardata = NULL;//The flightlines are stored here.
   ostringstream *loaderrorstream = NULL;//Stringstream getting error messages from the quadtree.
   ofstream loaderroroutput;//Stream outputting error messages from the quadtree to a file.
   string loaderroroutputfile;//Path of file for error message output.

//Gtk objects:
Gtk::EventBox *eventboxtdo = NULL;//Contains the overview.
Gtk::EventBox *eventboxprof = NULL;//Contains the profile.
Gtk::AboutDialog *about = NULL;//Information about LAG.
//Advanced viewing options:
Gtk::Dialog *advancedoptionsdialog = NULL;//Dialog window for advanced options.
   Gtk::SpinButton *classificationselect = NULL;//This determines what to classify points as when selected through the profile.
   //False elevation:
      Gtk::CheckButton *classcheckbutton0 = NULL;//Elevate classifications with the respective codes:
      Gtk::CheckButton *classcheckbutton2 = NULL;//...
      Gtk::CheckButton *classcheckbutton3 = NULL;//...
      Gtk::CheckButton *classcheckbutton4 = NULL;//...
      Gtk::CheckButton *classcheckbutton5 = NULL;//...
      Gtk::CheckButton *classcheckbutton6 = NULL;//...
      Gtk::CheckButton *classcheckbutton7 = NULL;//...
      Gtk::CheckButton *classcheckbutton8 = NULL;//...
      Gtk::CheckButton *classcheckbutton9 = NULL;//...
      Gtk::CheckButton *classcheckbutton12 = NULL;//...
      Gtk::CheckButton *classcheckbuttonA = NULL;//"Anything else" classification elevator.
   //Colouring and shading:
      Gtk::SpinButton *heightmaxselect = NULL;//Determines the maximum height for colouring and shading.
      Gtk::SpinButton *heightminselect = NULL;//Determines the minimum height for colouring and shading.
      Gtk::HScrollbar *heightscrollbar = NULL;//Used for "stepping through" the height range (absolute minimum to absolute maximum) with a page size defined by the difference between heightmaxselect and heightminselect.
      Gtk::SpinButton *heightoffsetselect = NULL;//This increases all brightness levels by a fixed amount, if based on height. In analogy to heightfloorselect, this raises both the floor and the ceiling.
      Gtk::SpinButton *heightfloorselect = NULL;//This increases brightness levels by a varying amount such that the lowest level will be increased by the value and higher levels by by a declining value until the level of 1.0, which will not increase at all. In the analogy, the floor is raised but not the ceiling, and the "space" between them is "squashed".
      Gtk::SpinButton *intensitymaxselect = NULL;//Equivalent as for height:
      Gtk::SpinButton *intensityminselect = NULL;//...
      Gtk::HScrollbar *intensityscrollbar = NULL;//...
      Gtk::SpinButton *intensityoffsetselect = NULL;//...
      Gtk::SpinButton *intensityfloorselect = NULL;//...
      Gtk::Button *drawingresetbutton = NULL;//Resets all other colouring and shading widgets to initial values.
   //Detail level (i.e. how many points to skip for each point shown, worked out INDIRECTLY; see the profile and overview classes for details):
      Gtk::SpinButton *maindetailselect = NULL;//Determines how many points are skipped displaying the main overview image.
      Gtk::SpinButton *maindetailselectprof = NULL;//Determines how many points are skipped displaying the main profile image.
      Gtk::SpinButton *previewdetailselectprof = NULL;//Determines how many points are skipped displaying the profile preview.
//File chooser:
Gtk::FileChooserDialog *filechooserdialog = NULL;//For opening files.
   Gtk::SpinButton *pointskipselect = NULL;//How many points to skip while loading one.
   Gtk::CheckButton *fenceusecheck = NULL;//Check button determining whether the (overview) fence is used for loading flightlines.
   Gtk::Entry *asciicodeentry = NULL;//The type code for opening ASCII files.
   Gtk::SpinButton *cachesizeselect = NULL;//The maximumum number of points to hold in cache.
   Gtk::Label *cachesizeGBlabel = NULL;//This displays the cache size in terms of gigabytes, approximately.
//File saver:
Gtk::FileChooserDialog *filesaverdialog = NULL;//For opening files.
   Gtk::Label *flightlinelistlabel = NULL;//This displays the cache size in terms of gigabytes, approximately.
   Gtk::SpinButton *flightlinesaveselect = NULL;
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

//Signals:
sigc::connection heightminconn;//These connections are for controlling the interaction of the height and intensity colouring and shading scrollbars and spinbuttons, particularly so that they do not interfere with each other at the wrong moment.
sigc::connection heightmaxconn;//...
sigc::connection intensityminconn;//...
sigc::connection intensitymaxconn;//...

//Show the about dialog when respective menu item activated.
void on_aboutmenuactivated(){ about->show_all(); }
//Hide the about dialog when close button activated.
void on_aboutresponse(int response_id){ about->hide_all(); }

//Opens the advanced options dialog.
void on_advancedbutton_clicked(){ advancedoptionsdialog->show_all(); }
//Closes the advanced options dialog.
void on_advancedoptionsdialog_response(int response_id){ advancedoptionsdialog->hide_all(); }
//The checkbuttons and their activations:
void on_classcheckbutton0_toggled(){ tdo->setheightenNonC(classcheckbutton0->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void on_classcheckbutton2_toggled(){ tdo->setheightenGround(classcheckbutton2->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void on_classcheckbutton3_toggled(){ tdo->setheightenLowVeg(classcheckbutton3->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void on_classcheckbutton4_toggled(){ tdo->setheightenMedVeg(classcheckbutton4->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void on_classcheckbutton5_toggled(){ tdo->setheightenHighVeg(classcheckbutton5->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void on_classcheckbutton6_toggled(){ tdo->setheightenBuildings(classcheckbutton6->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void on_classcheckbutton7_toggled(){ tdo->setheightenNoise(classcheckbutton7->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void on_classcheckbutton8_toggled(){ tdo->setheightenMass(classcheckbutton8->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void on_classcheckbutton9_toggled(){ tdo->setheightenWater(classcheckbutton9->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void on_classcheckbutton12_toggled(){ tdo->setheightenOverlap(classcheckbutton12->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
void on_classcheckbuttonA_toggled(){ tdo->setheightenUndefined(classcheckbuttonA->get_active()); if(tdo->is_realized())tdo->drawviewable(1); }
//The drawing settings:
void changecoloursandshades(){
   //Please note that there is a reason why the profile is updated before the overview: if it is the other way around then the overview's drawing thread would be running so it will be unpredictable which part will execute opengl code first, which can sometimes mean that the overview will be drawn the same size as the profile, which might confuse users:
   if(prof->is_realized())prof->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(prof->is_realized())prof->drawviewable(1);
   if(tdo->is_realized())tdo->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(tdo->is_realized())tdo->drawviewable(1);
}
void on_heightmaxselect_changed(){
   heightminselect->set_range(tdo->getrminz(),heightmaxselect->get_value()-0.01);
   heightscrollbar->get_adjustment()->set_page_size(heightmaxselect->get_value() - heightminselect->get_value());//Want the scrollbar's slider length to correlate with the range we define.
   heightscrollbar->set_increments(1,heightmaxselect->get_value() - heightminselect->get_value());
   changecoloursandshades();
}
void on_heightminselect_changed(){
   heightmaxselect->set_range(heightminselect->get_value()+0.01,tdo->getrmaxz());
   heightscrollbar->set_value(heightminselect->get_value());//Value for the scrollbar only changed here as upper value is defined by this value and the page size.
   heightscrollbar->get_adjustment()->set_page_size(heightmaxselect->get_value() - heightminselect->get_value());//Want the scrollbar's slider length to correlate with the range we define.
   heightscrollbar->set_increments(1,heightmaxselect->get_value() - heightminselect->get_value());
   changecoloursandshades();
}
bool on_heightscrollbar_scrolled(Gtk::ScrollType scroll,double new_value){
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
void on_heightoffsetselect_changed(){
   tdo->setzoffset(heightoffsetselect->get_value());
   prof->setzoffset(heightoffsetselect->get_value());
   changecoloursandshades();
}
void on_heightfloorselect_changed(){
   tdo->setzfloor(heightfloorselect->get_value());
   prof->setzfloor(heightfloorselect->get_value());
   changecoloursandshades();
}
void on_intensitymaxselect_changed(){
   intensityminselect->set_range(tdo->getrminintensity(),intensitymaxselect->get_value()-1);
   intensityscrollbar->get_adjustment()->set_page_size(intensitymaxselect->get_value() - intensityminselect->get_value());//Want the scrollbar's slider length to correlate with the range we define.
   intensityscrollbar->set_increments(1,intensitymaxselect->get_value() - intensityminselect->get_value());
   changecoloursandshades();
}
void on_intensityminselect_changed(){
   intensitymaxselect->set_range(intensityminselect->get_value()+1,tdo->getrmaxintensity());
   intensityscrollbar->set_value(intensityminselect->get_value());//Value for the scrollbar only changed here as upper value is defined by this value and the page size.
   intensityscrollbar->get_adjustment()->set_page_size(intensitymaxselect->get_value() - intensityminselect->get_value());//Want the scrollbar's slider length to correlate with the range we define.
   intensityscrollbar->set_increments(1,intensitymaxselect->get_value() - intensityminselect->get_value());
   changecoloursandshades();
}
bool on_intensityscrollbar_scrolled(Gtk::ScrollType scroll,double new_value){
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
void on_intensityoffsetselect_changed(){
   tdo->setintensityoffset(intensityoffsetselect->get_value());
   prof->setintensityoffset(intensityoffsetselect->get_value());
   changecoloursandshades();
}
void on_intensityfloorselect_changed(){
   tdo->setintensityfloor(intensityfloorselect->get_value());
   prof->setintensityfloor(intensityfloorselect->get_value());
   changecoloursandshades();
}
//This resets the advanced colouring and shading options to the values indicated by the drawing objects.
void on_drawingresetbutton_clicked(){
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
void on_maindetailselected(){
   tdo->setmaindetail(maindetailselect->get_value());
   if(tdo->is_realized())tdo->drawviewable(1);
}
//Does the same as on_maindetailselected, except for the profile.
void on_maindetailselectedprof(){
   prof->setmaindetail(maindetailselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(1);
}
//Does the same as on_maindetailselectedprof, except for the preview of the profile.
void on_previewdetailselectedprof(){
   prof->setpreviewdetail(previewdetailselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(2);
}

/*Determines whether the input filename(s) are correct and, if so, creates or modifies the quadtree to accomodate the data. First it makes sure that a sufficient number of arguments have been passed to include the executable, point offset and at least one filename. It then extracts the point offset, which is used to skip a certain number of points between each read point, for faster loading. It then starts dealing withthe filenames:
 * Try:
 *    For all the filenames:
 *       If the filename is not empty:
 *          If the filename ends with .las or .LAS:
 *             Create LASLoader;
 *             If this is the first filename AND either the user has pressed the refresh button or no files have yet loaded OR the quadtree is empty:
 *                If using a fence:
 *                   Delete old quadtree (including the original dummy one if the program has just started);
 *                   Get fence coordinates;
 *                   Create new quadtree with data with fence;
 *                Else:
 *                   Delete old quadtree (including the original dummy one if the program has just started);
 *                   Create new quadtree with data without fence;
 *             Else:
 *                If using a fence:
 *                   Get fence coordinates;
 *                   Add data with fence to quadtree;
 *                Else:
 *                   Add data without fence to quadtree;
 *             Write any errors to the error file;
 *          Else if the filename ends with .txt or .TXT:
 *             Get typecode from text box;
 *             Create ASCIIloader useing typecode;
 *             If this is the first filename AND either the user has pressed the refresh button or no files have yet loaded OR the quadtree is empty:
 *                If using a fence:
 *                   Delete old quadtree (including the original dummy one if the program has just started);
 *                   Get fence coordinates;
 *                   Create new quadtree with data with fence;
 *                Else:
 *                   Delete old quadtree (including the original dummy one if the program has just started);
 *                   Create new quadtree with data without fence;
 *             Else:
 *                If using a fence:
 *                   Get fence coordinates;
 *                   Add data with fence to quadtree;
 *                Else:
 *                   Add data without fence to quadtree;
 *             Write any errors to the error file;
 *          Else do nothing;
 * Catch:
 *    Empty the quadtree;
 *
 * Then a pointer to the data is sent to all of the display areas, which are then prepared for displaying and then display.
 *
 *
 * */
int testfilename(int argc,char *argv[],bool start,bool usearea){
   cachelimit = cachesizeselect->get_value();
   try{//Attempt to get real files.
      string pointoffset,filename;
      if(argc < 3){
         cout << "Form is \"lag <point skip number> <first file> [other files]...\"" << endl;
         return 11;
      }
      pointoffset.append(argv[1]);
      int poffs = atoi(pointoffset.c_str());//This returns the integer translation of the string, or zero if there is none, so...
      if(poffs == 0 && pointoffset != "0"){//... in the situation where there is a value of zero, check the string to see whether this is because of there being a zero value or because of there not being an integer. If the value in the string is not zero:
         cout << "The point offset must be an integer greater than or equal to zero. In addition, zero can only be accepted in the form \"0\", not \"00\" etc.." << endl;
         return 1;
      }
      for(int count = 2;count<argc;count++){//We start after the executable path and the point offset.
         filename.assign(argv[count]);
         if(filename != ""){
            if(filename.find(".las",filename.length()-4)!=string::npos||filename.find(".LAS",filename.length()-4)!=string::npos){//For las files:
               LASloader* loader = new LASloader(argv[count]);
               if((count==2 && (start || !loadedanyfiles))||lidardata==NULL){//If refreshing (or from command-line) use first filename to make quadtree...
                  if(usearea){//If using the fence:
                     if(lidardata != NULL)delete lidardata;
                     lidardata = NULL;//This prevents a double free if the creation of the new quadtree fails and throws an exception.
                     loaderrorstream->str("");
                     double *fencexs = NULL,*fenceys = NULL;//These are NOT to be deleted here as the arrays they will point to are managed by the TwoDeeOVerview object.
                     int fenceps = 0;
                     if(tdo->is_realized())tdo->getfence(fencexs,fenceys,fenceps);
                     if(fencexs!=NULL&&fenceys!=NULL)lidardata = new quadtree(loader,bucketlimit,poffs,fencexs,fenceys,fenceps,cachelimit,loaderrorstream);
                     else{
                        cout << "No fence!" << endl;
                        return 222;
                     }
                  }
                  else{//If not:
                     if(lidardata != NULL)delete lidardata;
                     lidardata = NULL;//This prevents a double free if the creation of the new quadtree fails and throws an exception.
                     loaderrorstream->str("");
                     lidardata = new quadtree(loader,bucketlimit,poffs,cachelimit,loaderrorstream);
                  }
               }
               else{//... but for all other situations add to it.
                  if(usearea){//If using the fence:
                     double *fencexs = NULL,*fenceys = NULL;//These are NOT to be deleted here as the arrays they will point to are managed by the TwoDeeOVerview object.
                     int fenceps = 0;
                     if(tdo->is_realized())tdo->getfence(fencexs,fenceys,fenceps);
                     if(fencexs!=NULL&&fenceys!=NULL)lidardata->load(loader,poffs,fencexs,fenceys,fenceps);
                     else{
                        cout << "No fence!" << endl;
                        return 222;
                     }
                  }
                  else lidardata->load(loader,poffs);//If not.
               }
               cout << filename << endl;
               if(loaderrorstream->str()!=""){
                  cout << "There have been errors in loading. Please see the file " + loaderroroutputfile << endl;
                  loaderroroutput << filename << endl;
                  loaderroroutput << loaderrorstream->str();
                  loaderroroutput.flush();
                  loaderrorstream->str("");
               }
               delete loader;
            }
            else if(filename.find(".txt",filename.length()-4)!=string::npos||filename.find(".TXT",filename.length()-4)!=string::npos){//For ASCII files (only works through GUI... Must get it to work for command-line at some point:
               string code1 = asciicodeentry->get_text();//The type code is needed to properly interpret the ASCII file.
               const char* code = code1.c_str();
               ASCIIloader* aloader = new ASCIIloader(argv[count],code);
               if((count==2 && (start || !loadedanyfiles))||lidardata==NULL){//If refreshing (or from command-line) use first filename to make quadtree...
                  if(usearea){//If using the fence:
                     if(lidardata != NULL)delete lidardata;
                     lidardata = NULL;//This prevents a double free if the creation of the new quadtree fails and throws an exception.
                     loaderrorstream->str("");
                     double *fencexs = NULL,*fenceys = NULL;//These are NOT to be deleted here as the arrays they will point to are managed by the TwoDeeOVerview object.
                     int fenceps = 0;
                     if(tdo->is_realized())tdo->getfence(fencexs,fenceys,fenceps);
                     if(fencexs!=NULL&&fenceys!=NULL)lidardata = new quadtree(aloader,bucketlimit,poffs,fencexs,fenceys,fenceps,cachelimit,loaderrorstream);
                     else{
                        cout << "No fence!" << endl;
                        return 222;
                     }
                  }
                  else{//If not:
                     if(lidardata != NULL)delete lidardata;
                     lidardata = NULL;//This prevents a double free if the creation of the new quadtree fails and throws an exception.
                     loaderrorstream->str("");
                     lidardata = new quadtree(aloader,bucketlimit,poffs,cachelimit,loaderrorstream);
                  }
               }
               else{//... but for all other situations add to it.
                  if(usearea){//If using the fence:
                     double *fencexs = NULL,*fenceys = NULL;//These are NOT to be deleted here as the arrays they will point to are managed by the TwoDeeOVerview object.
                     int fenceps = 0;
                     if(tdo->is_realized())tdo->getfence(fencexs,fenceys,fenceps);
                     if(fencexs!=NULL&&fenceys!=NULL)lidardata->load(aloader,poffs,fencexs,fenceys,fenceps);
                     else{
                        cout << "No fence!" << endl;
                        return 222;
                     }
                  }
                  else lidardata->load(aloader,poffs);//If not.
               }
               cout << filename << endl;
               if(loaderrorstream->str()!=""){
                  cout << "There have been errors in loading. Please see the file " + loaderroroutputfile << endl;
                  loaderroroutput << filename << endl;
                  loaderroroutput << loaderrorstream->str();
                  loaderroroutput.flush();
                  loaderrorstream->str("");
               }
               delete aloader;
            }
            else{//For incorrect file extensions:
               cout << "Files must have the extensions .las, .LAS, .txt or .TXT." << endl;
               return 17;
            }
         }
      }
   }
   catch(descriptiveexception e){
      cout << "There has been an exception:" << endl;
      cout << "What: " << e.what() << endl;
      cout << "Why: " << e.why() << endl;
      loaderrorstream->str("");
      if(lidardata != NULL)delete lidardata;
      lidardata = NULL;
      tdo->hide_all();
      eventboxtdo->remove();
      prof->hide_all();
      eventboxprof->remove();
      loadedanyfiles = false;
      return 22;
   }
   tdo->setlidardata(lidardata,bucketlimit);//Provide the drawing objects access to the quadtree:
   prof->setlidardata(lidardata,bucketlimit);//...
   //Possibly: Move two copies of this to the relevant LAS and ASCII parts, above, so that files are drawn as soon as they are loaded and as the other files are loading. This might not work because of the bug that causes the flightline(s) not to be drawn immediately after loading. UPDATE: now it seems to draw just one bucket(!!!) immediately after loading.
   if(loadedanyfiles){//If drawing areas are already visible, prepare the new images and draw them.
      tdo->prepare_image();
      tdo->drawviewable(1);
      prof->prepare_image();
      prof->drawviewable(1);
   }
   else{//Otherwise, pack them into the vboxes and then show them, which will do as the above block does.
      eventboxtdo->add(*tdo);
      tdo->show_all();
      eventboxprof->add(*prof);
      prof->show_all();
   }
   on_drawingresetbutton_clicked();//(Re)Set the advanced colouring and shading options to the values indicated by the recently loaded flightlines.
   loadedanyfiles = true;
   return 0;
}
//If either the add or refresh button is pressed, then this function takes the selected filenames and creates an imitation of a command-line command, which is then sent to testfilename() where the file will be opened.
void on_filechooserdialogresponse(int response_id){
   if(response_id == Gtk::RESPONSE_CLOSE)filechooserdialog->hide_all();
   else if(response_id == 1 || response_id == 2){
      Glib::SListHandle<Glib::ustring> names = filechooserdialog->get_filenames();
      int argc = names.size() + 2;//testfilename expects a command-line command in the form: <this program> <point offset <file 1> [file 2]...
      char** argv = new char*[argc];
      argv[0] = new char[exename.length()+1];//This program.
      strcpy(argv[0],exename.c_str());
      ostringstream pointoffset;
      pointoffset << pointskipselect->get_value_as_int();
      string poffs = pointoffset.str();
      argv[1] = new char[poffs.length()+1];//The point offset.
      strcpy(argv[1],poffs.c_str());
      argc=2;
      for(Glib::SListHandle<Glib::ustring>::iterator itera = names.begin();itera!=names.end();itera++){//Until the last iterator is reached, insert the contents of the iterators into argv.
         argv[argc] = new char[(*itera).length()+1];
         strcpy(argv[argc],(*itera).c_str());
         argc++;
      }
      if(response_id == 1)testfilename(argc,argv,false,fenceusecheck->get_active());//For adding, do not create a new quadtree (false).
      if(response_id == 2)testfilename(argc,argv,true,fenceusecheck->get_active());//For refreshing, do create a new quadtree (true).
      for(int i = 0;i < argc;i++)delete[] argv[i];
      delete[] argv;
   }
}
//When the cachesize (in points) is changed, this outputs the value in Gigabytes (NOT Gibibytes) to a label next to it.
void on_cachesize_changed(){
   ostringstream GB;
   GB << ((double)cachesizeselect->get_value()*sizeof(point))/1000000000;
   string labelstring = "Approximately: " + GB.str() + " GB.";
   cachesizeGBlabel->set_text(labelstring);
}
//When selected from the menu, the file chooser opens.
void on_openfilemenuactivated(){ filechooserdialog->show_all(); }
void on_filesaverdialogresponse(int response_id){
   if(response_id == Gtk::RESPONSE_CLOSE)filechooserdialog->hide_all();
   else if(response_id == 1){
      LASsaver *saver = new LASsaver(filesaverdialog->get_filename().c_str(),lidardata->getfilename(flightlinesaveselect->get_value_as_int()).c_str());
      lidardata->saveflightline(flightlinesaveselect->get_value_as_int(),saver);
      delete saver;
   }
}
void on_flightlinesaveselected(){
   filesaverdialog->set_filename(lidardata->getfilename(flightlinesaveselect->get_value_as_int()));
}
//When selected from the menu, the file saver opens.
void on_savefilemenuactivated(){
   if(tdo->is_realized())filesaverdialog->show_all();
   else return;
   string flightline,list="";
   int count = 0;
   try{
      while(true){
         flightline = lidardata->getfilename(count);
         ostringstream number;
         number << count;
         list += number.str() + ":  " + flightline + "\n";
         count++;
      }
   }
   catch(descriptiveexception e){
      flightlinelistlabel->set_text(list);
      if(count<1)count = 1;
      flightlinesaveselect->set_range(0,count-1);
      on_flightlinesaveselected();
   }
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
   if(prof->is_realized())prof->classify(classificationselect->get_value_as_int());
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
         Gtk::MenuItem *savefilemenuitem = NULL;//For selecting to get file-saving menu.
         refXml->get_widget("savefilemenuitem",savefilemenuitem);
         if(savefilemenuitem)savefilemenuitem->signal_activate().connect(sigc::ptr_fun(&on_savefilemenuactivated));
         refXml->get_widget("filesaverdialog",filesaverdialog);
         if(filesaverdialog)filesaverdialog->signal_response().connect(sigc::ptr_fun(&on_filesaverdialogresponse));
         refXml->get_widget("flightlinelistlabel",flightlinelistlabel);
         refXml->get_widget("flightlinesaveselect",flightlinesaveselect);
         if(flightlinesaveselect)flightlinesaveselect->signal_value_changed().connect(sigc::ptr_fun(&on_flightlinesaveselected));
         refXml->get_widget("filechooserdialog",filechooserdialog);
         if(filechooserdialog)filechooserdialog->signal_response().connect(sigc::ptr_fun(&on_filechooserdialogresponse));
         refXml->get_widget("pointskipselect",pointskipselect);
         refXml->get_widget("fenceusecheck",fenceusecheck);
         refXml->get_widget("asciicodeentry",asciicodeentry);
         refXml->get_widget("cachesizeselect",cachesizeselect);
         if(cachesizeselect){
            cachesizeselect->set_range(1000000,1000000000000);//That is 0 to 40 TB! This code is written on a 4 GB RAM machine in 2009-10, so, if the rate of increase is that of quadrupling every five years, then 40 TB will be reached in less than 35 years.
            cachesizeselect->set_value(25000000);//25000000 points is about 1 GB. On this 4 GB RAM machine, I only want LAG to use a quarter of my resources.
            cachesizeselect->set_increments(1000000,1000000);
            refXml->get_widget("cachesizeGBlabel",cachesizeGBlabel);
            if(cachesizeGBlabel){
               on_cachesize_changed();
               cachesizeselect->signal_value_changed().connect(sigc::ptr_fun(&on_cachesize_changed));
            }
         }
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
         refXml->get_widget("advancedoptionsdialog",advancedoptionsdialog);
         if(advancedoptionsdialog)advancedoptionsdialog->signal_response().connect(sigc::ptr_fun(&on_advancedoptionsdialog_response));
         refXml->get_widget("classificationselect",classificationselect);
         //False elevation:
            refXml->get_widget("classcheckbutton0",classcheckbutton0);
            if(classcheckbutton0)classcheckbutton0->signal_toggled().connect(sigc::ptr_fun(&on_classcheckbutton0_toggled));
            refXml->get_widget("classcheckbutton2",classcheckbutton2);
            if(classcheckbutton2)classcheckbutton2->signal_toggled().connect(sigc::ptr_fun(&on_classcheckbutton2_toggled));
            refXml->get_widget("classcheckbutton3",classcheckbutton3);
            if(classcheckbutton3)classcheckbutton3->signal_toggled().connect(sigc::ptr_fun(&on_classcheckbutton3_toggled));
            refXml->get_widget("classcheckbutton4",classcheckbutton4);
            if(classcheckbutton4)classcheckbutton4->signal_toggled().connect(sigc::ptr_fun(&on_classcheckbutton4_toggled));
            refXml->get_widget("classcheckbutton5",classcheckbutton5);
            if(classcheckbutton5)classcheckbutton5->signal_toggled().connect(sigc::ptr_fun(&on_classcheckbutton5_toggled));
            refXml->get_widget("classcheckbutton6",classcheckbutton6);
            if(classcheckbutton6)classcheckbutton6->signal_toggled().connect(sigc::ptr_fun(&on_classcheckbutton6_toggled));
            refXml->get_widget("classcheckbutton7",classcheckbutton7);
            if(classcheckbutton7)classcheckbutton7->signal_toggled().connect(sigc::ptr_fun(&on_classcheckbutton7_toggled));
            refXml->get_widget("classcheckbutton8",classcheckbutton8);
            if(classcheckbutton8)classcheckbutton8->signal_toggled().connect(sigc::ptr_fun(&on_classcheckbutton8_toggled));
            refXml->get_widget("classcheckbutton9",classcheckbutton9);
            if(classcheckbutton9)classcheckbutton9->signal_toggled().connect(sigc::ptr_fun(&on_classcheckbutton9_toggled));
            refXml->get_widget("classcheckbutton12",classcheckbutton12);
            if(classcheckbutton12)classcheckbutton12->signal_toggled().connect(sigc::ptr_fun(&on_classcheckbutton12_toggled));
            refXml->get_widget("classcheckbuttonA",classcheckbuttonA);
            if(classcheckbuttonA)classcheckbuttonA->signal_toggled().connect(sigc::ptr_fun(&on_classcheckbuttonA_toggled));
         //Height and intensity threasholding and brightness modifiers:
            refXml->get_widget("heightmaxselect",heightmaxselect);
            if(heightmaxselect)heightmaxconn = heightmaxselect->signal_value_changed().connect(sigc::ptr_fun(&on_heightmaxselect_changed));
            refXml->get_widget("heightminselect",heightminselect);
            if(heightminselect)heightminconn = heightminselect->signal_value_changed().connect(sigc::ptr_fun(&on_heightminselect_changed));
            refXml->get_widget("heightscrollbar",heightscrollbar);
            if(heightscrollbar)heightscrollbar->signal_change_value().connect(sigc::ptr_fun(&on_heightscrollbar_scrolled));
            refXml->get_widget("heightoffsetselect",heightoffsetselect);
            if(heightoffsetselect)heightoffsetselect->signal_value_changed().connect(sigc::ptr_fun(&on_heightoffsetselect_changed));
            refXml->get_widget("heightfloorselect",heightfloorselect);
            if(heightfloorselect)heightfloorselect->signal_value_changed().connect(sigc::ptr_fun(&on_heightfloorselect_changed));
            refXml->get_widget("intensitymaxselect",intensitymaxselect);
            if(intensitymaxselect)intensitymaxconn = intensitymaxselect->signal_value_changed().connect(sigc::ptr_fun(&on_intensitymaxselect_changed));
            refXml->get_widget("intensityminselect",intensityminselect);
            if(intensityminselect)intensityminconn = intensityminselect->signal_value_changed().connect(sigc::ptr_fun(&on_intensityminselect_changed));
            refXml->get_widget("intensityscrollbar",intensityscrollbar);
            if(intensityscrollbar)intensityscrollbar->signal_change_value().connect(sigc::ptr_fun(&on_intensityscrollbar_scrolled));
            refXml->get_widget("intensityoffsetselect",intensityoffsetselect);
            if(intensityoffsetselect)intensityoffsetselect->signal_value_changed().connect(sigc::ptr_fun(&on_intensityoffsetselect_changed));
            refXml->get_widget("intensityfloorselect",intensityfloorselect);
            if(intensityfloorselect)intensityfloorselect->signal_value_changed().connect(sigc::ptr_fun(&on_intensityfloorselect_changed));
            refXml->get_widget("drawingresetbutton",drawingresetbutton);
            if(drawingresetbutton)drawingresetbutton->signal_clicked().connect(sigc::ptr_fun(&on_drawingresetbutton_clicked));
         //Detail (points to skip) level:
            refXml->get_widget("maindetailselect",maindetailselect);
            if(maindetailselect){
               maindetailselect->set_range(0,300);//Essentially arbitrary. Would there be any situation where such a coarse detail level as 300 pixels would be wanted?
               maindetailselect->set_value(0.00);
               maindetailselect->signal_value_changed().connect(sigc::ptr_fun(&on_maindetailselected));
            }
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

         refXml->get_widget("eventboxtdo",eventboxtdo);
         eventboxtdo->signal_key_press_event().connect(sigc::ptr_fun(&on_tdo_key_press));
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
      refXml->get_widget("maindetailselectprof",maindetailselectprof);
      if(maindetailselectprof){
         maindetailselectprof->set_range(0,300);//Essentially arbitrary. Would there be any situation where such a coarse detail level as 300 pixels would be wanted?
         maindetailselectprof->set_value(0);
         maindetailselectprof->signal_value_changed().connect(sigc::ptr_fun(&on_maindetailselectedprof));
      }
      refXml->get_widget("previewdetailselectprof",previewdetailselectprof);
      if(previewdetailselectprof){
         previewdetailselectprof->set_range(0,300);//Essentially arbitrary. Would there be any situation where such a coarse detail level as 300 pixels would be wanted?
         previewdetailselectprof->set_value(0);
         previewdetailselectprof->signal_value_changed().connect(sigc::ptr_fun(&on_previewdetailselectedprof));
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

      refXml->get_widget("eventboxprof",eventboxprof);
      eventboxprof->signal_key_press_event().connect(sigc::ptr_fun(&on_prof_key_press));
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
   tdo->setmaindetail(maindetailselect->get_value());
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
   prof->setmaindetail(maindetailselectprof->get_value());
   prof->setpreviewdetail(previewdetailselectprof->get_value());
   prof->setdrawpoints(pointshowtoggle->get_active());
   prof->setdrawmovingaverage(lineshowtoggle->get_active());
   prof->setmavrgrange(movingaveragerangeselect->get_value());
   prof->setslantwidth(slantwidthselectprof->get_value());
   prof->setslantwidth(slantwidthselectprof->get_value());
   prof->setslanted(slantedprof->get_active());
   testfilename(argc,argv,true,false);//In case of command-line commands.
   gtkmain.run(*overviewwindow);
   return 0;
}

int main(int argc, char** argv) {
   cout << "Build number: 2010.06.07.1" << endl;
   time_t starttime = time(NULL);
   char meh[80];
   strftime(meh, 80, "%Y.%m.%d(%j).%H-%M-%S.%Z", localtime(&starttime));
   ostringstream bleh;
   bleh << meh;
   loaderroroutputfile = "/tmp/LAGloadingerrors" + bleh.str() + ".txt";
   loaderroroutput.open(loaderroroutputfile.c_str());
   exename.append(argv[0]);//Record the program name.
   loadedanyfiles = false;
   loaderrorstream = new ostringstream();
   int returnvalue = GUIset(argc, argv);//Make the GUI.
   if(tdo!=NULL)delete tdo;
   if(prof != NULL)delete prof;
   if(lidardata != NULL)delete lidardata;
   delete loaderrorstream;
   loaderroroutput.close();
   return returnvalue;
}
