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

//EASTER EGG! Clippy!
string picturename;//Path of clippy image.
const bool useclippy = false;//Whether or not to use clippy.

bool drawwhentoggled = true;

quadtree* lidardata = NULL;//The flightlines are stored here.
int bucketlimit = 100000;//How many points in each bucket, maximum.
int cachelimit = 25000000;//How many points to hold in cache. 1 GB ~= 50000000 points.
TwoDeeOverview *tdo = NULL;//The 2d overview.
Profile *prof = NULL;//The profile.
string exename = "";//The path of the executable.
bool loadedanyfiles = false;//Whether or not any files have already been loaded in this session.
//Quadtree error handling:
ostringstream *loaderrorstream;//Stringstream getting error messages from the quadtree.
ofstream loaderroroutput;//Stream outputting error messages from the quadtree to a file.
string loaderroroutputfile;//Path of file for error message output.

//Gtk objects:
Gtk::VBox *vboxtdo = NULL;//Contains the overview.
Gtk::VBox *vboxprof = NULL;//Contains the profile.
Gtk::AboutDialog *about = NULL;//Information about LAG.
//File chooser:
Gtk::FileChooserDialog *filechooserdialog = NULL;//For opening files.
Gtk::SpinButton *pointskipselect = NULL;//How many points to skip after loading one.
Gtk::CheckButton *fenceusecheck = NULL;//Check button determining whether the fence is used for loading flightlines.
Gtk::Entry *asciicodeentry = NULL;//The type code for opening ASCII files.
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
Gtk::ToolButton *returnbutton = NULL;
Gtk::ToggleToolButton *fencetoggle = NULL;//Toggle button determining whether mouse dragging selects the fence.
Gtk::ToggleToolButton *profiletoggle = NULL;//Toggle button determining whether mouse dragging selects the profile.
Gtk::SpinButton *profwidthselect = NULL;//Determines the width of the profile in metres.
Gtk::SpinButton *pointwidthselect = NULL;//Determines the width of the points in the overview in pixels.
Gtk::SpinButton *maindetailselect = NULL;//Determines how many points are skipped displaying the main overview image.
Gtk::Dialog *advancedoptionsdialog = NULL;//Dialog window for advanced options.
   //Advanced viewing options for the overview:
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
Gtk::SpinButton *maindetailselectprof = NULL;//Determines how many points are skipped displaying the main profile image.
Gtk::SpinButton *previewdetailselectprof = NULL;//Determines how many points are skipped displaying the profile preview.
Gtk::ToggleToolButton *pointshowtoggle = NULL;//Whether to show the points on the profile.
Gtk::ToggleToolButton *lineshowtoggle = NULL;//Whether to show the lines on the profile.
Gtk::SpinButton *movingaveragerangeselect = NULL;//The range of the moving average for the lines on the profile.
Gtk::ToggleToolButton *rulertoggle = NULL;//Toggle button determining whether the ruler is viewable on the profile.
Gtk::Label *rulerlabel = NULL;//Label displaying the distance along the ruler, in all dimensions etc. for the profile.

//Show the about dialog when respective menu item activated.
void on_aboutmenuactivated(){ about->show_all(); }
//Hide the about dialog when close button activated.
void on_aboutresponse(int response_id){ about->hide_all(); }

//Get the area to load the flightline(s) in by calling the overview's getfence() method.
void get_area(double &minX,double &minY,double &maxX,double &maxY){ tdo->getfence(minX,minY,maxX,maxY); }
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
                     lidardata = NULL;
                     loaderrorstream->str("");
                     double minX,minY,maxX,maxY;
                     get_area(minX,minY,maxX,maxY);
                     lidardata = new quadtree(loader,bucketlimit,poffs,minX,minY,maxX,maxY,cachelimit,loaderrorstream);
                  }
                  else{//If not:
                     if(lidardata != NULL)delete lidardata;
                     lidardata = NULL;
                     loaderrorstream->str("");
                     lidardata = new quadtree(loader,bucketlimit,poffs,cachelimit,loaderrorstream);
                  }
               }
               else{//... but for all other situations add to it.
                  if(usearea){//If using the fence:
                     double minX,minY,maxX,maxY;
                     get_area(minX,minY,maxX,maxY);
                     lidardata->load(loader,poffs,minX,minY,maxX,maxY);
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
                     lidardata = NULL;
                     loaderrorstream->str("");
                     double minX,minY,maxX,maxY;
                     get_area(minX,minY,maxX,maxY);
                     lidardata = new quadtree(aloader,bucketlimit,poffs,minX,minY,maxX,maxY,cachelimit,loaderrorstream);
                  }
                  else{//If not:
                     if(lidardata != NULL)delete lidardata;
                     lidardata = NULL;
                     loaderrorstream->str("");
                     lidardata = new quadtree(aloader,bucketlimit,poffs,cachelimit,loaderrorstream);
                  }
               }
               else{//... but for all other situations add to it.
                  if(usearea){//If using the fence:
                     double minX,minY,maxX,maxY;
                     get_area(minX,minY,maxX,maxY);
                     lidardata->load(aloader,poffs,minX,minY,maxX,maxY);
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
//      cout << "Please check to make sure your files exist and the paths are properly spelled." << endl;
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
   tdo->setlidardata(lidardata,bucketlimit);
   prof->setlidardata(lidardata,bucketlimit);
   //Possibly: Move two copies of this to the relevant LAS and ASCII parts, above, so that files are drawn as soon as they are loaded and as the other files are loading. This will need the viewport bug to be fixed in order to work properly.
   if(loadedanyfiles){//If drawing areas are already visible, prepare the new images and draw them.
      tdo->prepare_image();
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
   loadedanyfiles = true;
   return 1;
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
//When selected from the menu, the file choosers opens.
void on_openfilemenuactivated(){ filechooserdialog->show_all(); }

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
      double startx,starty,endx,endy,width;
      tdo->getprofile(startx,starty,endx,endy,width);
      prof->showprofile(startx,starty,endx,endy,width);
      if(tdo->is_realized()&&!profiletoggle->get_active()&&!rulertoggleover->get_active()&&!fencetoggle->get_active())tdo->drawviewable(2);
   }
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}

////When toggled, the profile view goes into rulering mode. When untoggled, rulering mode ends.
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

//When toggled, the profile box is shown on the 2d overview regardless of whether profiling mode is active.
void on_showprofilecheck(){
   tdo->setshowprofile(showprofilecheck->get_active());
   if(tdo->is_realized())tdo->drawviewable(2);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}

//When the value in the spinbutton for profile width is changed, tell the 2d overview, then make the new profile box and then draw it. This does NOT update the profile itself (or, at least, not yet). To update the profile after the width has been satisfactorily adjusted, the profiletoggle must be toggled and then untoggled.
void on_profwidthselected(){
   tdo->setprofwidth(profwidthselect->get_value());
   tdo->makeprofbox();
   if(tdo->is_realized())tdo->drawviewable(2);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
//   double startx,starty,endx,endy,width;//Experiment...
//   tdo->getprofile(startx,starty,endx,endy,width);//...
//   prof->showprofile(startx,starty,endx,endy,width);//...
}

//This changes the width of the points in pixels.
void on_pointwidthselected(){
   tdo->setpointwidth(pointwidthselect->get_value());
   if(tdo->is_realized())tdo->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}

//This indirectly determines how many points are skipped when viewing the main overview image. I.e. this affects it as well as the number of visible buckets.
void on_maindetailselected(){
   tdo->setmaindetail(maindetailselect->get_value());
   if(tdo->is_realized())tdo->drawviewable(1);
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

//Does the same as on_pointwidthselected, except for the profile.
void on_pointwidthselectedprof(){
   prof->setpointwidth(pointwidthselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(2);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}

//Does the same as on_maindetailselected, except for the profile.
void on_maindetailselectedprof(){
   prof->setmaindetail(maindetailselectprof->get_value());
   if(prof->is_realized())prof->drawviewable(1);
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}

//Does the same as on_previewdetailselected, except for the profile.
void on_previewdetailselectedprof(){
   prof->setpreviewdetail(previewdetailselectprof->get_value());
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

//This returns the overview to its original position.
void on_returnbutton_clicked(){
   if(tdo->is_realized())tdo->returntostart();
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}

//This returns the profile to its original position.
void on_returnbuttonprof_clicked(){
   if(prof->is_realized())prof->returntostart();
   if(useclippy==true)if(tdo->is_realized())tdo->clippy(picturename);
}

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
   Gtk::Window *windowp = NULL;
   refXml->get_widget("window1", windowp);
   if (windowp) {
      windowp->set_title("LAG");
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
            //Fence and profile toggles and attribute selectors:
            refXml->get_widget("fencetoggle",fencetoggle);
            if(fencetoggle)fencetoggle->signal_toggled().connect(sigc::ptr_fun(&on_fencetoggle));
            refXml->get_widget("profiletoggle",profiletoggle);
            if(profiletoggle)profiletoggle->signal_toggled().connect(sigc::ptr_fun(&on_profiletoggle));
            refXml->get_widget("profwidthselect",profwidthselect);
            if(profwidthselect){
               profwidthselect->set_range(0,30000);//Essentially arbitrary. Would there be any situation where a width greater than 30 km would be wanted?
               profwidthselect->set_value(5);
               profwidthselect->signal_value_changed().connect(sigc::ptr_fun(&on_profwidthselected));
            }
            //For overview image viewing attributes:
            refXml->get_widget("pointwidthselect",pointwidthselect);
            if(pointwidthselect){
               pointwidthselect->set_range(0,300);//Essentially arbitrary. Would there be any situation where a width greater than 300 pixels would be wanted? Very far future?
               pointwidthselect->set_value(2);
               pointwidthselect->signal_value_changed().connect(sigc::ptr_fun(&on_pointwidthselected));
            }
            refXml->get_widget("maindetailselect",maindetailselect);
            if(maindetailselect){
               maindetailselect->set_range(0,300);//Essentially arbitrary. Would there be any situation where such a coarse detail level as 300 pixels would be wanted?
               maindetailselect->set_value(0.01);
               maindetailselect->signal_value_changed().connect(sigc::ptr_fun(&on_maindetailselected));
            }
            refXml->get_widget("returnbutton",returnbutton);
            if(returnbutton)returnbutton->signal_clicked().connect(sigc::ptr_fun(&on_returnbutton_clicked));
            //Advanced viewing options:
               Gtk::ToolButton *advancedbutton = NULL;
               refXml->get_widget("advancedbutton",advancedbutton);
               if(advancedbutton)advancedbutton->signal_clicked().connect(sigc::ptr_fun(&on_advancedbutton_clicked));
               refXml->get_widget("advancedoptionsdialog",advancedoptionsdialog);
               if(advancedoptionsdialog)advancedoptionsdialog->signal_response().connect(sigc::ptr_fun(&on_advancedoptionsdialog_response));
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
            //The ruler:
            refXml->get_widget("rulertoggleover",rulertoggleover);
            if(rulertoggleover)rulertoggleover->signal_toggled().connect(sigc::ptr_fun(&on_rulertoggleover));
            refXml->get_widget("rulerlabelover",rulerlabelover);
         }
         window2->show_all();
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
               pointwidthselectprof->set_range(0,300);//Essentially arbitrary. Would there be any situation where a width greater than 300 pixels would be wanted? Very far future?
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
//      Gtk::Window *window4 = NULL;
//      refXml->get_widget("window4", window4);
//      window4->set_title("Window4");
//      window4->show();
//      gtkmain.run(*windowp);   
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
//      tdo = tdo1;//For some reason, I have not been able to use "new" with the global object directly. :-(
      tdo->set_size_request(200,200);
      //Initialisations:
      tdo->setintensitycolour(colourbyintensitymenu->get_active());
      tdo->setheightcolour(colourbyheightmenu->get_active());
      tdo->setlinecolour(colourbyflightlinemenu->get_active());
      tdo->setclasscolour(colourbyclassificationmenu->get_active());
      tdo->setreturncolour(colourbyreturnmenu->get_active());
      tdo->setintensitybrightness(brightnessbyintensitymenu->get_active());
      tdo->setheightbrightness(brightnessbyheightmenu->get_active());
      tdo->setprofwidth(profwidthselect->get_value());
      tdo->setpointwidth(pointwidthselect->get_value());
      tdo->setmaindetail(maindetailselect->get_value());
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
//      prof = prof1;
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
   } else {
      std::cerr << "eep, no main window?" << std::endl;
      return 1;
   }
   return 0;
}

int main(int argc, char** argv) {
   cout << "Build number: 2010.02.19.1" << endl;
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
   lidardata = NULL;
   return GUIset(argc, argv);//Make the GUI.
   delete tdo;
   delete prof;
   if(lidardata != NULL)delete lidardata;
   delete loaderrorstream;
   loaderroroutput.close();
}
