#include <iostream>
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "quadtree.h"
#include "LASloader.h"
#include "ASCIIloader.h"
#include "TwoDeeOverview.h"
#include "Profile.h"
#include "commonfunctions.h"
using namespace std;

quadtree* lidardata;//The flightlines are stored here.
TwoDeeOverview *tdo = NULL;//The 2d overview.
Profile *prof = NULL;//The profile.
int bucketlimit = 100000;//How many points in each bucket, maximum.
string exename = "";//The path of the executable.
bool loadedanyfiles = false;

//Gtk objects:
Gtk::ToggleToolButton *fencetoggle = NULL;//Toggle button determining whether mouse dragging selects the fence.
Gtk::CheckButton *fenceusecheck = NULL;
Gtk::ToggleToolButton *profiletoggle = NULL;//Toggle button determining whether mouse dragging selects the profile.
Gtk::ToggleToolButton *showprofiletoggle = NULL;//Toggle button determining whether the profile box is viewable on the 2d overview.
Gtk::ToggleToolButton *rulertoggle = NULL;//Toggle button determining whether the ruler is viewable.
Gtk::Label *rulerlabel = NULL;//Label displaying the distance along the ruler, in all dimensions etc..
Gtk::RadioMenuItem *colourbyintensitymenu = NULL;
Gtk::RadioMenuItem *colourbyheightmenu = NULL;
Gtk::RadioMenuItem *colourbyflightlinemenu = NULL;
Gtk::RadioMenuItem *colourbyclassificationmenu = NULL;
Gtk::RadioMenuItem *colourbyreturnmenu = NULL;
Gtk::RadioMenuItem *brightnessbyintensitymenu = NULL;
Gtk::RadioMenuItem *brightnessbyheightmenu = NULL;
Gtk::SpinButton *profwidthselect = NULL;//Determines the width of the profile in metres(?).
Gtk::SpinButton *pointwidthselect = NULL;
Gtk::SpinButton *maindetailselect = NULL;
Gtk::SpinButton *previewdetailselect = NULL;
Gtk::RadioMenuItem *colourbynonemenuprof = NULL;
Gtk::RadioMenuItem *colourbyintensitymenuprof = NULL;
Gtk::RadioMenuItem *colourbyheightmenuprof = NULL;
Gtk::RadioMenuItem *colourbyflightlinemenuprof = NULL;
Gtk::RadioMenuItem *colourbyclassificationmenuprof = NULL;
Gtk::RadioMenuItem *colourbyreturnmenuprof = NULL;
Gtk::RadioMenuItem *brightnessbynonemenuprof = NULL;
Gtk::RadioMenuItem *brightnessbyintensitymenuprof = NULL;
Gtk::RadioMenuItem *brightnessbyheightmenuprof = NULL;
Gtk::SpinButton *pointwidthselectprof = NULL;
Gtk::SpinButton *maindetailselectprof = NULL;
Gtk::SpinButton *previewdetailselectprof = NULL;
Gtk::ToggleToolButton *pointshowtoggle = NULL;
Gtk::ToggleToolButton *lineshowtoggle = NULL;
Gtk::SpinButton *movingaveragerangeselect = NULL;
Gtk::MenuItem *openfilemenuitem = NULL;
Gtk::FileChooserDialog *filechooserdialog = NULL;
Gtk::SpinButton *pixelskipselect = NULL;
Gtk::VBox *vboxtdo = NULL;
Gtk::VBox *vboxprof = NULL;
Gtk::Entry *asciicodeentry = NULL;

void on_returnbuttonprof_clicked(){
   prof->returntostart();
}

void on_returnbutton_clicked(){
   tdo->returntostart();
}

void get_area(double &minX,double &minY,double &maxX,double &maxY){
   tdo->getfence(minX,minY,maxX,maxY);
}

int testfilename(int argc,char *argv[],bool start,bool usearea){
   bool show_areas = loadedanyfiles;
   try{//Attempt to get real files.
      string pointoffset,filename;
      if(argc < 3){
         cout << "Form is \"lag <point skip number> <first file> [other files]...\"" << endl;
         return 11;
      }
      pointoffset.append(argv[1]);
      int poffs = 0;
      poffs = StringToUINT(pointoffset);//This returns the integer translation of the string, or zero if there is none, so...
      if(poffs == 0 && pointoffset != "0"){//... in the situation where there is a value of zero, check the string to see whether this is because of there being a zero value or because of there not being and integer. If the value in the string is not zero:
         cout << "The point offset must be an integer greater than or equal to zero. In addition, zero can only be accepted in the form \"0\", not \"00\" etc.." << endl;
         return 1;
      }
      for(int count = 2;count<argc;count++){//We start after the executable path and the point offset.
         filename.assign(argv[count]);
         if(filename != ""){
            if(filename.find(".las",filename.length()-4)!=string::npos||filename.find(".LAS",filename.length()-4)!=string::npos){//For las files:
               LASloader* loader = new LASloader(argv[count]);
               if(count==2 && (start || !loadedanyfiles)){
                  if(usearea){
                     delete lidardata;
                     double minX,minY,maxX,maxY;
                     get_area(minX,minY,maxX,maxY);
                     lidardata = new quadtree(loader,bucketlimit,poffs,minX,minY,maxX,maxY);
                  }
                  else{
                     delete lidardata;
                     lidardata = new quadtree(loader,bucketlimit,poffs);//First time make quadtree...
                  }
               }
               else{
                  if(usearea){
                     double minX,minY,maxX,maxY;
                     get_area(minX,minY,maxX,maxY);
                     lidardata->load(loader,poffs,minX,minY,maxX,maxY);
                  }
                  else lidardata->load(loader,poffs);//... otherwise add to it.
               }
               cout << filename << endl;
               delete loader;
               loadedanyfiles = true;
            }
            else if(filename.find(".txt",filename.length()-4)!=string::npos||filename.find(".TXT",filename.length()-4)!=string::npos){//For ASCII files:
               string code1 = asciicodeentry->get_text();
               const char* code = code1.c_str();
               cout << code << endl;
               ASCIIloader* aloader = new ASCIIloader(argv[count],code);
               if(count==2 && (start || !loadedanyfiles)){
                  if(usearea){
                     delete lidardata;
                     double minX,minY,maxX,maxY;
                     get_area(minX,minY,maxX,maxY);
                     lidardata = new quadtree(aloader,bucketlimit,poffs,minX,minY,maxX,maxY);
                  }
                  else{
                     delete lidardata;
                     lidardata = new quadtree(aloader,bucketlimit,poffs);//First time make quadtree...
                  }
               }
               else{
                  if(usearea){
                     double minX,minY,maxX,maxY;
                     get_area(minX,minY,maxX,maxY);
                     lidardata->load(aloader,poffs,minX,minY,maxX,maxY);
                  }
                  else lidardata->load(aloader,poffs);//... otherwise add to it.
               }
               cout << filename << endl;
               delete aloader;
               loadedanyfiles = true;
            }
            else{//For incorrect file extensions:
               cout << "Files must have the extensions .las, .LAS, .txt or .TXT." << endl;
               return 17;
            }
         }
      }
   }
   catch(char const* e){
      cout << e << endl;
      cout << "Please check to make sure your files exist and the paths are properly spelled." << endl;
      return 22;
   }
   if(show_areas){
      tdo->prepare_image();
      tdo->drawviewable(1);
      prof->prepare_image();
      prof->drawviewable(1);
   }
     else{
         vboxtdo->pack_end(*tdo,true,true);
         tdo->show_all();
         vboxprof->pack_end(*prof,true,true);
         prof->show_all();
     }
   return 1;
}

void on_openfilemenuactivated(){
   filechooserdialog->show_all();
}

void on_filechooserdialogresponse(int response_id){
   if(response_id == Gtk::RESPONSE_CLOSE)filechooserdialog->hide_all();
   else if(response_id == 1 || response_id == 2){
      Glib::SListHandle<Glib::ustring> names = filechooserdialog->get_filenames();
      int argc = names.size() + 2;
      char** argv = new char*[argc];
      argv[0] = new char[exename.length()+1];
      strcpy(argv[0],exename.c_str());
      ostringstream pixeloffset;
      pixeloffset << pixelskipselect->get_value_as_int();
      string poffs = pixeloffset.str();
      argv[1] = new char[poffs.length()+1];
      strcpy(argv[1],poffs.c_str());
      argc=2;
      for(Glib::SListHandle<Glib::ustring>::iterator itera = names.begin();itera!=names.end();itera++){
         argv[argc] = new char[(*itera).length()+1];
         strcpy(argv[argc],(*itera).c_str());
         argc++;
      }
      if(response_id == 1)testfilename(argc,argv,false,fenceusecheck->get_active());
      if(response_id == 2)testfilename(argc,argv,true,fenceusecheck->get_active());
      for(int i = 0;i < argc;i++)delete[] argv[i];
      delete[] argv;
   }
//   cout << lidardata->flightlinenum << endl;
}

void on_pointshowtoggle(){
   prof->setdrawpoints(pointshowtoggle->get_active());
   prof->drawviewable(1);
}

void on_lineshowtoggle(){
   prof->setdrawmovingaverage(lineshowtoggle->get_active());
   prof->drawviewable(1);
}

void on_movingaveragerangeselect(){
   prof->setmavrgrange(movingaveragerangeselect->get_value());
   prof->drawviewable(1);
}

void on_pointwidthselected(){
   tdo->setpointwidth(pointwidthselect->get_value());
   tdo->drawviewable(2);
}

void on_maindetailselected(){
   tdo->setmaindetail(maindetailselect->get_value());
   tdo->drawviewable(1);
}

void on_previewdetailselected(){
   tdo->setpreviewdetail(previewdetailselect->get_value());
   tdo->drawviewable(2);
}

void on_colouractivated(){
   tdo->setintensitycolour(colourbyintensitymenu->get_active());
   tdo->setheightcolour(colourbyheightmenu->get_active());
   tdo->setlinecolour(colourbyflightlinemenu->get_active());
   tdo->setclasscolour(colourbyclassificationmenu->get_active());
   tdo->setreturncolour(colourbyreturnmenu->get_active());
   tdo->drawviewable(1);
}

void on_brightnessactivated(){
   tdo->setintensitybrightness(brightnessbyintensitymenu->get_active());
   tdo->setheightbrightness(brightnessbyheightmenu->get_active());
   tdo->drawviewable(1);
}

void on_pointwidthselectedprof(){
   prof->setpointwidth(pointwidthselectprof->get_value());
   prof->drawviewable(2);
}

void on_maindetailselectedprof(){
   prof->setmaindetail(maindetailselectprof->get_value());
   prof->drawviewable(1);
}

void on_previewdetailselectedprof(){
   prof->setpreviewdetail(previewdetailselectprof->get_value());
   prof->drawviewable(2);
}

void on_colouractivatedprof(){
   prof->setintensitycolour(colourbyintensitymenuprof->get_active());
   prof->setheightcolour(colourbyheightmenuprof->get_active());
   prof->setlinecolour(colourbyflightlinemenuprof->get_active());
   prof->setclasscolour(colourbyclassificationmenuprof->get_active());
   prof->setreturncolour(colourbyreturnmenuprof->get_active());
   prof->drawviewable(1);
}

void on_brightnessactivatedprof(){
   prof->setintensitybrightness(brightnessbyintensitymenuprof->get_active());
   prof->setheightbrightness(brightnessbyheightmenuprof->get_active());
   prof->drawviewable(1);
}

void on_fencetoggle(){
   if(fencetoggle->get_active()){
      if(profiletoggle->get_active())profiletoggle->set_active(false);
      tdo->setupfence();
   }
   else{
   	tdo->unsetupfence();
   }
   tdo->drawviewable(1);
}

//void on_showfencetoggle(){
//   tdo->setshowfence(showfencetoggle->get_active());
//   tdo->drawviewable(1);
//}

//When toggled, the 2d overview goes into profile selection mode. When untoggled, 2d overview goes out of profile selection mode and the profile parameters are sent to the profile area.
void on_profiletoggle(){
   if(profiletoggle->get_active()){
      if(fencetoggle->get_active())fencetoggle->set_active(false);
      tdo->setupprofile();
   }
   else{
   	tdo->unsetupprofile();
      double startx,starty,endx,endy,width;
      tdo->getprofile(startx,starty,endx,endy,width);
      prof->showprofile(startx,starty,endx,endy,width);
   }
   tdo->drawviewable(1);
}

//When toggled, the profile box is shown on the 2d overview regardless of whether profiling mode is active.
void on_showprofiletoggle(){
   tdo->setshowprofile(showprofiletoggle->get_active());
   tdo->drawviewable(1);
}

//When the value in the spinbutton for profile width is changed, tell the 2d overview, then make the new profile box and then draw it. This does NOT update the profile itself (or, at least, not yet). To update the profile after the width has been satisfactorily adjusted, the profiletoggle must be toggled and then untoggled.
void on_profwidthselected(){
   tdo->setprofwidth(profwidthselect->get_value());
   tdo->makeprofbox();
   tdo->drawviewable(2);
   double startx,starty,endx,endy,width;//Experiment...
   tdo->getprofile(startx,starty,endx,endy,width);//...
   prof->showprofile(startx,starty,endx,endy,width);//...
}

//When toggled, the profile view goes into rulering mode. When untoggled, rulering mode ends.
void on_rulertoggle(){
   if(rulertoggle->get_active())prof->setupruler();
   else prof->unsetupruler();
}

//Sets up the GUI.
int GUIset(int argc,char *argv[]){
   Gtk::Main gtkmain(argc, argv);
   Glib::RefPtr<Gnome::Glade::Xml> refXml;
   try{//Takes the path the executable was called with and uses it to find the .glade file needed to make the GUI.
      unsigned int index = exename.rfind("/");//Find the last forward slash and make its index our index.
      if(index==string::npos)index=0;//I.e. in the event that there is no forward slash (so must be calling from the same directory), just go from 0, where the forward slash would have been.
      else index++;//We do not actually want to include the forward slash.
      string gladename = exename;
      gladename.replace(index,10,"test.glade");
      cout << exename << endl;
      cout << gladename << endl;
      refXml = Gnome::Glade::Xml::create(gladename);
   }
   catch(const Gnome::Glade::XmlError& ex){ 
      cerr << ex.what() << std::endl;
      cerr << "The file fastqcSVN.glade must be located in the same directory as the fastQC executable." << endl;
      return 1;
   }
   Gtk::GL::init(argc, argv);
   Gtk::Window *windowp = NULL;
   refXml->get_widget("window1", windowp);
   if (windowp) {
      windowp->set_title("Test");
      Gtk::Window *window2 = NULL;
      refXml->get_widget("window2", window2);
      if(window2){
         window2->set_title("Window2");
         refXml->get_widget("vboxtdo",vboxtdo);
         if(vboxtdo){
            refXml->get_widget("openfilemenuitem",openfilemenuitem);
            if(openfilemenuitem)openfilemenuitem->signal_activate().connect(sigc::ptr_fun(&on_openfilemenuactivated));
            refXml->get_widget("filechooserdialog",filechooserdialog);
            if(filechooserdialog)filechooserdialog->signal_response().connect(sigc::ptr_fun(&on_filechooserdialogresponse));
            refXml->get_widget("pixelskipselect",pixelskipselect);
            refXml->get_widget("asciicodeentry",asciicodeentry);

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

            Gtk::RadioMenuItem *brightnessbynonemenu = NULL;
            refXml->get_widget("brightnessbynonemenu",brightnessbynonemenu);
            if(brightnessbynonemenu)brightnessbynonemenu->signal_activate().connect(sigc::ptr_fun(&on_brightnessactivated));

            refXml->get_widget("brightnessbyintensitymenu",brightnessbyintensitymenu);
            if(brightnessbyintensitymenu)brightnessbyintensitymenu->signal_activate().connect(sigc::ptr_fun(&on_brightnessactivated));

            refXml->get_widget("brightnessbyheightmenu",brightnessbyheightmenu);
            if(brightnessbyheightmenu)brightnessbyheightmenu->signal_activate().connect(sigc::ptr_fun(&on_brightnessactivated));

            refXml->get_widget("fencetoggle",fencetoggle);
            if(fencetoggle)fencetoggle->signal_toggled().connect(sigc::ptr_fun(&on_fencetoggle));
            refXml->get_widget("fenceusecheck",fenceusecheck);

            refXml->get_widget("profiletoggle",profiletoggle);
            if(profiletoggle)profiletoggle->signal_toggled().connect(sigc::ptr_fun(&on_profiletoggle));
            refXml->get_widget("showprofiletoggle",showprofiletoggle);
            if(showprofiletoggle)showprofiletoggle->signal_toggled().connect(sigc::ptr_fun(&on_showprofiletoggle));
            refXml->get_widget("profwidthselect",profwidthselect);
            if(profwidthselect){
               profwidthselect->set_range(0,30000);//Essentially arbitrary. Would there be any situation where a width greater than 30 km would be wanted?
               profwidthselect->set_value(5);
               profwidthselect->signal_value_changed().connect(sigc::ptr_fun(&on_profwidthselected));
            }
            refXml->get_widget("pointwidthselect",pointwidthselect);
            if(pointwidthselect){
               pointwidthselect->set_range(0,300);//Essentially arbitrary. Would there be any situation where a width greater than 300 pixels would be wanted? Very far future?
               pointwidthselect->set_value(1);
               pointwidthselect->signal_value_changed().connect(sigc::ptr_fun(&on_pointwidthselected));
            }
            refXml->get_widget("maindetailselect",maindetailselect);
            if(maindetailselect){
               maindetailselect->set_range(0,300);//Essentially arbitrary. Would there be any situation where such a coarse detail level as 300 pixels would be wanted?
               maindetailselect->set_value(0.01);
               maindetailselect->signal_value_changed().connect(sigc::ptr_fun(&on_maindetailselected));
            }
            refXml->get_widget("previewdetailselect",previewdetailselect);
            if(previewdetailselect){
               previewdetailselect->set_range(0,300);//Essentially arbitrary. Would there be any situation where such a coarse detail level as 300 pixels would be wanted?
               previewdetailselect->set_value(1);
               previewdetailselect->signal_value_changed().connect(sigc::ptr_fun(&on_previewdetailselected));
            }
            Gtk::ToolButton *returnbutton = NULL;
            refXml->get_widget("returnbutton",returnbutton);
            if(returnbutton)returnbutton->signal_clicked().connect(sigc::ptr_fun(&on_returnbutton_clicked));
         }
         window2->show_all();
      }
      Gtk::Window *window3 = NULL;
      refXml->get_widget("window3", window3);
      if(window3){
         window3->set_title("Window3");
         refXml->get_widget("vboxprof",vboxprof);
         if(vboxprof){
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

            refXml->get_widget("brightnessbynonemenuprof",brightnessbynonemenuprof);
            if(brightnessbynonemenuprof)brightnessbynonemenuprof->signal_activate().connect(sigc::ptr_fun(&on_brightnessactivatedprof));

            refXml->get_widget("brightnessbyintensitymenuprof",brightnessbyintensitymenuprof);
            if(brightnessbyintensitymenuprof)brightnessbyintensitymenuprof->signal_activate().connect(sigc::ptr_fun(&on_brightnessactivatedprof));

            refXml->get_widget("brightnessbyheightmenuprof",brightnessbyheightmenuprof);
            if(brightnessbyheightmenuprof)brightnessbyheightmenuprof->signal_activate().connect(sigc::ptr_fun(&on_brightnessactivatedprof));

            refXml->get_widget("rulertoggle",rulertoggle);
            if(rulertoggle)rulertoggle->signal_toggled().connect(sigc::ptr_fun(&on_rulertoggle));
            refXml->get_widget("rulerlabel",rulerlabel);
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
       if(glconfig==NULL)std::exit(1);
   }
     TwoDeeOverview* tdo1 = new TwoDeeOverview(glconfig,lidardata,bucketlimit);
     tdo = tdo1;//For some reason, I have not been able to use "new" with the global object directly. :-(
     tdo->set_size_request(200,200);
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
     tdo->setpreviewdetail(previewdetailselect->get_value());
     Glib::RefPtr<Gdk::GL::Config> glconfig2;//Creating separate configs for each window. Is this really necessary? It does not do anything yet, but hopefully will form a nucleus to the solution to the shared viewport problem.
     glconfig2 = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |      Gdk::GL::MODE_DEPTH  |     Gdk::GL::MODE_DOUBLE);
     if (glconfig2==NULL){
        glconfig2 = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |    Gdk::GL::MODE_DEPTH);
        if(glconfig2==NULL)std::exit(1);
     }
     Profile* prof1 = new Profile(glconfig2,lidardata,bucketlimit,rulerlabel);
     prof = prof1;
     prof->set_size_request(200,200);
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
      testfilename(argc,argv,true,false);
      gtkmain.run(*window2);
   } else {
      std::cerr << "eep, no main window?" << std::endl;
      return 1;
   }
   return 0;
}

int main(int argc, char** argv) {
   exename.append(argv[0]);
   lidardata = new quadtree(0,0,1,1,bucketlimit);
   loadedanyfiles = false;
   return GUIset(argc, argv);//Make the GUI.
   delete tdo;
   delete prof;
   delete lidardata;
}
