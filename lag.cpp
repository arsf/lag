/*
 * File: lag.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: November 2009 - February 2010
 *
 * */
#include <iostream>
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "quadtree.h"
#include "LASloader.h"
#include "ASCIIloader.h"
#include "TwoDeeOverview.h"
#include "Profile.h"
using namespace std;

TwoDeeOverview *tdo = NULL;//The 2d overview.
Profile *prof = NULL;//The profile.
string exename = "";//The path of the executable.
bool drawwhentoggled = true;//This variable prevents the image(s) from being drawn twice as a result of toggling a radio button (or similar), which deactivates (and therefore toggles again) another one in the same group. This variable must start as true, as the methods make it opposite before using it, so that things will de drawn after the second "toggling".

//EASTER EGG! Clippy!
string picturename;//Path of clippy image.
bool useclippy = false;//Whether or not to use clippy.
Gtk::CheckButton *clippycheck = NULL;//Check button determining whether the users will be questioned about whether they need help.
void on_clippycheck_toggled(){ useclippy = clippycheck->get_active(); }//The assistant will now wonder whether the user wants any help.

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
Gtk::VBox *vboxtdo = NULL;//Contains the overview.
Gtk::VBox *vboxprof = NULL;//Contains the profile.
Gtk::AboutDialog *about = NULL;//Information about LAG.
//Advanced viewing options:
Gtk::Dialog *advancedoptionsdialog = NULL;//Dialog window for advanced options.
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
      Gtk::SpinButton *heightmaxselect = NULL;//Determines the maximum height for C&S.
      Gtk::SpinButton *heightminselect = NULL;//Determines the minimum height for C&S.
      Gtk::SpinButton *heightoffsetselect = NULL;//This increases all brightness levels by a fixed amount, if based on height.
      Gtk::SpinButton *heightfloorselect = NULL;//This increases brightness levels by a varying amount such that the lowest level will be increased by the value and higher levels by by a declining value until the level of 1.0, which will not increase at all.
      Gtk::SpinButton *intensitymaxselect = NULL;//Equivalent as for height:
      Gtk::SpinButton *intensityminselect = NULL;//...
      Gtk::SpinButton *intensityoffsetselect = NULL;//...
      Gtk::SpinButton *intensityfloorselect = NULL;//...
      Gtk::Button *drawingresetbutton = NULL;//Resets all other C&S widgets to initial values.
   //Detail level (i.e. how many points to skip for each point shown, worked out INDIRECTLY; see the profile and overview classes for details:
      Gtk::SpinButton *maindetailselect = NULL;//Determines how many points are skipped displaying the main overview image.
      Gtk::SpinButton *maindetailselectprof = NULL;//Determines how many points are skipped displaying the main profile image.
      Gtk::SpinButton *previewdetailselectprof = NULL;//Determines how many points are skipped displaying the profile preview.
//File chooser:
Gtk::FileChooserDialog *filechooserdialog = NULL;//For opening files.
   Gtk::SpinButton *pointskipselect = NULL;//How many points to skip after loading one.
   Gtk::CheckButton *fenceusecheck = NULL;//Check button determining whether the fence is used for loading flightlines.
   Gtk::Entry *asciicodeentry = NULL;//The type code for opening ASCII files.
   Gtk::SpinButton *cachesizeselect = NULL;//How many points to hold in cache.
   Gtk::Label *cachesizeGBlabel = NULL;//This displays the cache size in terms of gigabytes, approximately.
//Overview:
   Gtk::MenuItem *openfilemenuitem = NULL;//For selecting to get file-opening menu.
   Gtk::CheckMenuItem *showprofilecheck = NULL;//Check button determining whether the profile box is viewable on the 2d overview.
   Gtk::RadioMenuItem *colourbyintensitymenu = NULL;//Determines whether the image is coloured by intensity.
   Gtk::RadioMenuItem *colourbyheightmenu = NULL;//Determines whether the image is coloured by height.
   Gtk::RadioMenuItem *colourbyflightlinemenu = NULL;//Determines whether the image is coloured by flightline.
   Gtk::RadioMenuItem *colourbyclassificationmenu = NULL;//Determines whether the image is coloured by classification.
   Gtk::RadioMenuItem *colourbyreturnmenu = NULL;//Determines whether the image is coloured by return.
   Gtk::RadioMenuItem *brightnessbyintensitymenu = NULL;//Determines whether the image is shaded by intensity.
   Gtk::RadioMenuItem *brightnessbyheightmenu = NULL;//Determines whether the image is shaded by height.
   Gtk::ToggleToolButton *fencetoggle = NULL;//Toggle button determining whether mouse dragging selects the fence.
   Gtk::ToggleToolButton *profiletoggle = NULL;//Toggle button determining whether mouse dragging selects the profile.
   Gtk::ToggleToolButton *orthogonalrectshapetoggle = NULL;//Toggle button determining whether or not to use an orthogonal selection.
   Gtk::ToggleToolButton *slantedrectshapetoggle = NULL;//Toggle button determining whether or not to use a slanted selection.
   Gtk::SpinButton *slantwidthselect = NULL;//Determines the width of the profile in metres.
   Gtk::SpinButton *pointwidthselect = NULL;//Determines the width of the points in the overview in pixels.
   Gtk::ToggleToolButton *rulertoggleover = NULL;//Toggle button determining whether the ruler is viewable on the overview.
   Gtk::Label *rulerlabelover = NULL;//Label displaying the distance along the ruler, in all dimensions etc. for the overview.
//Profile:
Gtk::Window *window3 = NULL;
Gtk::RadioMenuItem *colourbyintensitymenuprof = NULL;//Determines whether the profile is coloured by intensity.
Gtk::RadioMenuItem *colourbyheightmenuprof = NULL;//Determines whether the profile is coloured by height.
Gtk::RadioMenuItem *colourbyflightlinemenuprof = NULL;//Determines whether the profile is coloured by flightline.
Gtk::RadioMenuItem *colourbyclassificationmenuprof = NULL;//Determines whether the profile is coloured by classification.
Gtk::RadioMenuItem *colourbyreturnmenuprof = NULL;//Determines whether the profile is coloured by return.
Gtk::RadioMenuItem *brightnessbyintensitymenuprof = NULL;//Determines whether the profile is shaded by intensity.
Gtk::RadioMenuItem *brightnessbyheightmenuprof = NULL;//Determines whether the profile is shaded by height.
Gtk::SpinButton *pointwidthselectprof = NULL;//Determines the width of the points in the profile in pixels.
Gtk::ToggleToolButton *pointshowtoggle = NULL;//Whether to show the points on the profile.
Gtk::ToggleToolButton *lineshowtoggle = NULL;//Whether to show the lines on the profile.
Gtk::SpinButton *movingaveragerangeselect = NULL;//The range of the moving average for the lines on the profile.
Gtk::ToggleToolButton *rulertoggle = NULL;//Toggle button determining whether the ruler is viewable on the profile.
Gtk::Label *rulerlabel = NULL;//Label displaying the distance along the ruler, in all dimensions etc. for the profile.

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
//The drawing settings (please note that there is a reason why the profile is updated before the overview: if it is the other way around then the overview's drawing thread would be running so it will be unpredictable which part will execute OpenGL code first, which can sometimes mean that the overview will be drawn the same size as the profile, which might confuse users):
void on_heightmaxselect_changed(){
   heightminselect->set_range(tdo->getrminz(),heightmaxselect->get_value()-1);
   if(prof->is_realized())prof->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(prof->is_realized())prof->drawviewable(1);
   if(tdo->is_realized())tdo->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(tdo->is_realized())tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
void on_heightminselect_changed(){
   heightmaxselect->set_range(heightminselect->get_value()+1,tdo->getrmaxz());
   if(prof->is_realized())prof->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(prof->is_realized())prof->drawviewable(1);
   if(tdo->is_realized())tdo->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(tdo->is_realized())tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
void on_heightoffsetselect_changed(){
   prof->setzoffset(heightoffsetselect->get_value());
   if(prof->is_realized())prof->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(prof->is_realized())prof->drawviewable(1);
   tdo->setzoffset(heightoffsetselect->get_value());
   if(tdo->is_realized())tdo->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(tdo->is_realized())tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
void on_heightfloorselect_changed(){
   prof->setzfloor(heightfloorselect->get_value());
   if(prof->is_realized())prof->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(prof->is_realized())prof->drawviewable(1);
   tdo->setzfloor(heightfloorselect->get_value());
   if(tdo->is_realized())tdo->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(tdo->is_realized())tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
void on_intensitymaxselect_changed(){
   intensityminselect->set_range(tdo->getrminintensity(),intensitymaxselect->get_value()-1);
   if(prof->is_realized())prof->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(prof->is_realized())prof->drawviewable(1);
   if(tdo->is_realized())tdo->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(tdo->is_realized())tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
void on_intensityminselect_changed(){
   intensitymaxselect->set_range(intensityminselect->get_value()+1,tdo->getrmaxintensity());
   if(prof->is_realized())prof->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(prof->is_realized())prof->drawviewable(1);
   if(tdo->is_realized())tdo->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(tdo->is_realized())tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
void on_intensityoffsetselect_changed(){
   prof->setintensityoffset(intensityoffsetselect->get_value());
   if(prof->is_realized())prof->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(prof->is_realized())prof->drawviewable(1);
   tdo->setintensityoffset(intensityoffsetselect->get_value());
   if(tdo->is_realized())tdo->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(tdo->is_realized())tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
void on_intensityfloorselect_changed(){
   prof->setintensityfloor(intensityfloorselect->get_value());
   if(prof->is_realized())prof->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(prof->is_realized())prof->drawviewable(1);
   tdo->setintensityfloor(intensityfloorselect->get_value());
   if(tdo->is_realized())tdo->coloursandshades(heightmaxselect->get_value(),heightminselect->get_value(),intensitymaxselect->get_value_as_int(),intensityminselect->get_value_as_int());
   if(tdo->is_realized())tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//This resets the advanced colouring and shading options to the values indicated by the drawing objects.
void on_drawingresetbutton_clicked(){
   heightmaxselect->set_range(tdo->getrminz()+1,tdo->getrmaxz());
   heightmaxselect->set_value(tdo->getrmaxz());
   heightminselect->set_range(tdo->getrminz(),tdo->getrmaxz()-1);
   heightminselect->set_value(tdo->getrminz());
   heightoffsetselect->set_value(0);
   heightfloorselect->set_value(0);
   intensitymaxselect->set_range(tdo->getrminintensity()+1,tdo->getrmaxintensity());
   intensitymaxselect->set_value(tdo->getrmaxintensity());
   intensityminselect->set_range(tdo->getrminintensity(),tdo->getrmaxintensity()-1);
   intensityminselect->set_value(tdo->getrminintensity());
   intensityoffsetselect->set_value(0);
   intensityfloorselect->set_value(0);
   //Draws as a result of the other callbacks, and only does so once because of threading (!!!!), so it may be prudent to change this in the future so that there is only ever one call.
}
//This indirectly determines how many points are skipped when viewing the main overview image. I.e. this affects it as well as the number of visible buckets.
void on_maindetailselected(){
   tdo->setmaindetail(maindetailselect->get_value());
   if(tdo->is_realized())tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//Does the same as on_maindetailselected, except for the profile.
void on_maindetailselectedprof(){
   prof->setmaindetail(maindetailselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//Does the same as on_maindetailselectedprof, except for the preview of the profile.
void on_previewdetailselectedprof(){
   prof->setpreviewdetail(previewdetailselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(2);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
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
      vboxtdo->remove(*tdo);
      prof->hide_all();
      vboxprof->remove(*prof);
      loadedanyfiles = false;
      return 22;
   }
   cout << 1 << endl;
   tdo->setlidardata(lidardata,bucketlimit);//Provide the drawing objects access to the quadtree:
   cout << 2 << endl;
   prof->setlidardata(lidardata,bucketlimit);//...
   cout << 3 << endl;
   //Possibly: Move two copies of this to the relevant LAS and ASCII parts, above, so that files are drawn as soon as they are loaded and as the other files are loading. This might not work because of the bug that causes the flightline(s) not to be drawn immediately after loading. UPDATE: now it seems to draw just one bucket(!!!) immediately after loading.
   if(loadedanyfiles){//If drawing areas are already visible, prepare the new images and draw them.
      cout << "nleh" << endl;
      tdo->prepare_image();
      cout << "mleh" << endl;
      tdo->drawviewable(1);
      prof->prepare_image();
      prof->drawviewable(1);
   }
   else{//Otherwise, pack them into the vboxes and then show them, which will do as the above block does.
      vboxtdo->pack_end(*tdo,true,true);
      tdo->show_all();
      vboxprof->pack_end(*prof,true,true);
      prof->show_all();
   }
   cout << 4 << endl;
   on_drawingresetbutton_clicked();//(Re)Set the advanced colouring and shading options to the values indicated by the recently loaded flightlines.
   cout << 5 << endl;
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
//When selected from the menu, the file choosers opens.
void on_openfilemenuactivated(){ filechooserdialog->show_all(); }

//When toggled, the profile box is shown on the 2d overview regardless of whether profiling mode is active.
void on_showprofilecheck(){
   tdo->setshowprofile(showprofilecheck->get_active());
   if(tdo->is_realized())tdo->drawviewable(2);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//If one of the colour radio menu items is selected (and, therefore, the others deselected) then set the values of the colour control variables in the overview to the values of the corresponding radio menu items.
void on_colouractivated(){
   tdo->setintensitycolour(colourbyintensitymenu->get_active());
   tdo->setheightcolour(colourbyheightmenu->get_active());
   tdo->setlinecolour(colourbyflightlinemenu->get_active());
   tdo->setclasscolour(colourbyclassificationmenu->get_active());
   tdo->setreturncolour(colourbyreturnmenu->get_active());
   drawwhentoggled = !drawwhentoggled;//As the signal handler is called twice (each time a radio button is toggle or untoggled, and toggling one automatically untoggles another), and we only want to draw (which is slow when caching) once, draw half the time this handler is called.
   if(tdo->is_realized()&&drawwhentoggled)tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//If one of the brightness radio menu items is selected (and, therefore, the others deselected) then set the values of the brightness control variables in the overview to the values of the corresponding radio menu items.
void on_brightnessactivated(){
   tdo->setintensitybrightness(brightnessbyintensitymenu->get_active());
   tdo->setheightbrightness(brightnessbyheightmenu->get_active());
   drawwhentoggled = !drawwhentoggled;//As the signal handler is called twice (each time a radio button is toggle or untoggled, and toggling one automatically untoggles another), and we only want to draw (which is slow when caching) once, draw half the time this handler is called.
   if(tdo->is_realized()&&drawwhentoggled)tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}

//This returns the overview to its original position.
void on_returnbutton_clicked(){
   if(tdo->is_realized())tdo->returntostart();
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//This changes the width of the points in pixels.
void on_pointwidthselected(){
   tdo->setpointwidth(pointwidthselect->get_value());
   if(tdo->is_realized())tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
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
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
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
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
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
      if(!window3->get_visible())window3->show_all();
      double *profxs = NULL,*profys = NULL;//These are NOT to be deleted here as the arrays they will point to will be managed by the TwoDeeOVerview object.
      int profps = 0;
      if(tdo->is_realized())tdo->getprofile(profxs,profys,profps);
      if(profxs!=NULL&&profys!=NULL)prof->showprofile(profxs,profys,profps);
      if(tdo->is_realized()&&!profiletoggle->get_active()&&!rulertoggleover->get_active()&&!fencetoggle->get_active())tdo->drawviewable(2);
   }
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//When toggled, this makes sure that the slant toggle is in the opposite state and then sets the orthogonal shape on the overview and makes new profile and fence boundaries before redrawing the overview, possibly with the new boundaries for profile and/or fence displayed.
void on_orthogonalrectshapetoggle(){
   if(orthogonalrectshapetoggle->get_active())if(slantedrectshapetoggle->get_active())slantedrectshapetoggle->set_active(false);
   if(!orthogonalrectshapetoggle->get_active())if(!slantedrectshapetoggle->get_active())slantedrectshapetoggle->set_active(true);
   tdo->setorthogonalshape(orthogonalrectshapetoggle->get_active());
   tdo->makeprofboundaries();
   tdo->makefenceboundaries();
   if(tdo->is_realized())tdo->drawviewable(2);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//When toggled, this makes sure that the orthogonal toggle is in the opposite state and then sets the slanted shape on the overview and makes new profile and fence boundaries before redrawing the overview, possibly with the new boundaries for profile and/or fence displayed.
void on_slantedrectshapetoggle(){
   if(slantedrectshapetoggle->get_active())if(orthogonalrectshapetoggle->get_active())orthogonalrectshapetoggle->set_active(false);
   if(!slantedrectshapetoggle->get_active())if(!orthogonalrectshapetoggle->get_active())orthogonalrectshapetoggle->set_active(true);
   tdo->setslantedshape(slantedrectshapetoggle->get_active());
   tdo->makeprofboundaries();
   tdo->makefenceboundaries();
   if(tdo->is_realized())tdo->drawviewable(2);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//When the value in the spinbutton for slanted shape width is changed, tell the 2d overview, then make the new slanted box and then draw it. This does NOT update the profile itself (or, at least, not yet) if the slanted box is for a profile. To update the profile after the width has been satisfactorily adjusted, the profiletoggle must be toggled and then untoggled.
void on_slantwidthselected(){
   tdo->setslantwidth(slantwidthselect->get_value());
   tdo->makeprofboundaries();
   tdo->makefenceboundaries();
   if(tdo->is_realized())tdo->drawviewable(2);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}

//Does the same as on_colouractivated, except for the profile.
void on_colouractivatedprof(){
   prof->setintensitycolour(colourbyintensitymenuprof->get_active());
   prof->setheightcolour(colourbyheightmenuprof->get_active());
   prof->setlinecolour(colourbyflightlinemenuprof->get_active());
   prof->setclasscolour(colourbyclassificationmenuprof->get_active());
   prof->setreturncolour(colourbyreturnmenuprof->get_active());
   if(prof->is_realized())prof->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//Does the same as on_brightnessactivated, except for the profile.
void on_brightnessactivatedprof(){
   prof->setintensitybrightness(brightnessbyintensitymenuprof->get_active());
   prof->setheightbrightness(brightnessbyheightmenuprof->get_active());
   if(prof->is_realized())prof->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}

//This returns the profile to its original position.
void on_returnbuttonprof_clicked(){
   if(prof->is_realized())prof->returntostart();
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//Does the same as on_pointwidthselected, except for the profile.
void on_pointwidthselectedprof(){
   prof->setpointwidth(pointwidthselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(2);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//Determines whether to display the points on the profile.
void on_pointshowtoggle(){
   prof->setdrawpoints(pointshowtoggle->get_active());
   if(prof->is_realized())prof->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//Determines whether to display the (best fit) lines on the profile.
void on_lineshowtoggle(){
   prof->setdrawmovingaverage(lineshowtoggle->get_active());
   if(prof->is_realized())prof->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//The best fit is a moving average, and this changes the range, and therefore the shape of the line.
void on_movingaveragerangeselect(){
   prof->setmavrgrange(movingaveragerangeselect->get_value());
   prof->make_moving_average();
   if(prof->is_realized())prof->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}
//When toggled, the profile view goes into rulering mode. When untoggled, rulering mode ends.
void on_rulertoggle(){
   if(rulertoggle->get_active())prof->setupruler();
   else prof->unsetupruler();
   if(prof->is_realized())prof->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
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
      picturename = exename;
      picturename.replace(index,10,"clippy.png");
   }
   catch(const Gnome::Glade::XmlError& ex){ 
      cerr << ex.what() << std::endl;
      cerr << "The file lag.glade must be located in the same directory as the lag executable." << endl;
      return 1;
   }
   Gtk::GL::init(argc, argv);
   Gtk::Window *window2 = NULL;
   refXml->get_widget("window2", window2);
   if(window2){//The overview window:
      window2->set_title("LAG Overview");
      refXml->get_widget("vboxtdo",vboxtdo);
      if(vboxtdo){
         //Menues:
         //For opening files:
            refXml->get_widget("openfilemenuitem",openfilemenuitem);
            if(openfilemenuitem)openfilemenuitem->signal_activate().connect(sigc::ptr_fun(&on_openfilemenuactivated));
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
               if(heightmaxselect)heightmaxselect->signal_value_changed().connect(sigc::ptr_fun(&on_heightmaxselect_changed));
               refXml->get_widget("heightminselect",heightminselect);
               if(heightminselect)heightminselect->signal_value_changed().connect(sigc::ptr_fun(&on_heightminselect_changed));
               refXml->get_widget("heightoffsetselect",heightoffsetselect);
               if(heightoffsetselect)heightoffsetselect->signal_value_changed().connect(sigc::ptr_fun(&on_heightoffsetselect_changed));
               refXml->get_widget("heightfloorselect",heightfloorselect);
               if(heightfloorselect)heightfloorselect->signal_value_changed().connect(sigc::ptr_fun(&on_heightfloorselect_changed));
               refXml->get_widget("intensitymaxselect",intensitymaxselect);
               if(intensitymaxselect)intensitymaxselect->signal_value_changed().connect(sigc::ptr_fun(&on_intensitymaxselect_changed));
               refXml->get_widget("intensityminselect",intensityminselect);
               if(intensityminselect)intensityminselect->signal_value_changed().connect(sigc::ptr_fun(&on_intensityminselect_changed));
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
            //EASTER EGG! Clippy!:
               refXml->get_widget("clippycheck",clippycheck);
               if(clippycheck)clippycheck->signal_toggled().connect(sigc::ptr_fun(&on_clippycheck_toggled));
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
      }
      window2->show_all();
   }
   else {
      std::cerr << "eep, no main window?" << std::endl;
      return 1;
   }
   refXml->get_widget("window3", window3);
   if(window3){
      window3->set_title("LAG Profile");
      refXml->get_widget("vboxprof",vboxprof);
      if(vboxprof){
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
            previewdetailselectprof->set_value(0.3);
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
      }
      window3->show_all();
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
   tdo->setintensitycolour(colourbyintensitymenu->get_active());
   tdo->setheightcolour(colourbyheightmenu->get_active());
   tdo->setlinecolour(colourbyflightlinemenu->get_active());
   tdo->setclasscolour(colourbyclassificationmenu->get_active());
   tdo->setreturncolour(colourbyreturnmenu->get_active());
   tdo->setintensitybrightness(brightnessbyintensitymenu->get_active());
   tdo->setheightbrightness(brightnessbyheightmenu->get_active());
   tdo->setslantwidth(slantwidthselect->get_value());
   tdo->setpointwidth(pointwidthselect->get_value());
   tdo->setmaindetail(maindetailselect->get_value());
   tdo->setorthogonalshape(orthogonalrectshapetoggle->get_active());
   tdo->setslantedshape(slantedrectshapetoggle->get_active());
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
   testfilename(argc,argv,true,false);//In case of command-line commands.
   gtkmain.run(*window2);
   return 0;
}

int main(int argc, char** argv) {
   cout << "Build number: 2010.05.10.1" << endl;
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
