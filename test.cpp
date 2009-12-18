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
Gtk::ToggleToolButton *profiletoggle = NULL;//Toggle button determining whether mouse dragging selects the profile.
Gtk::ToggleToolButton *showprofiletoggle = NULL;//Toggle button determining whether the profile box is viewable on the 2d overview.
Gtk::ToggleToolButton *rulertoggle = NULL;//Toggle button determining whether the ruler is viewable.
Gtk::Label *rulerlabel = NULL;//Label displaying the distance along the ruler, in all dimensions etc..
TwoDeeOverview *tdo = NULL;//The 2d overview.
Profile *prof = NULL;//The profile.
Gtk::SpinButton *profwidthselect = NULL;//Determines the width of the profile in metres(?).
int bucketlimit = 100000;//How many points in each bucket, maximum.
string exename = "";//The path of the executable.

//When toggled, the 2d overview goes into profile selection mode. When untoggled, 2d overview goes out of profile selection mode and the profile parameters are sent to the profile area.
void on_profiletoggle(){
   if(profiletoggle->get_active())tdo->setupprofile();
   else{
   	tdo->unsetupprofile();
      double startx,starty,endx,endy,width;
      tdo->getprofile(startx,starty,endx,endy,width);
      prof->showprofile(startx,starty,endx,endy,width);
   }
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
         Gtk::VBox *vboxtdo = NULL;
         refXml->get_widget("vboxtdo",vboxtdo);
         if(vboxtdo){
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
            Glib::RefPtr<Gdk::GL::Config> glconfig;//Creating separate configs for each window. Is this really necessary? It does not do anything yet, but hopefully will form a nucleus to the solution to the shared viewport problem.
            glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |      Gdk::GL::MODE_DEPTH  |     Gdk::GL::MODE_DOUBLE);
            if (glconfig==NULL){
                glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |    Gdk::GL::MODE_DEPTH);
                if(glconfig==NULL)std::exit(1);
            }
            TwoDeeOverview* tdo1 = new TwoDeeOverview(glconfig,lidardata,bucketlimit);
            tdo = tdo1;//For some reason, I have not been able to use "new" with the global object directly. :-(
            tdo->set_size_request(200,200);
            tdo->setprofwidth(profwidthselect->get_value());
            vboxtdo->pack_end(*tdo,true,true);
         }
         window2->show_all();
      }
      Gtk::Window *window3 = NULL;
      refXml->get_widget("window3", window3);
      if(window3){
         window3->set_title("Window3");
         Gtk::VBox *vboxprof = NULL;
         refXml->get_widget("vboxprof",vboxprof);
         if(vboxprof){
            refXml->get_widget("rulertoggle",rulertoggle);
            if(rulertoggle)rulertoggle->signal_toggled().connect(sigc::ptr_fun(&on_rulertoggle));
            refXml->get_widget("rulerlabel",rulerlabel);
            Glib::RefPtr<Gdk::GL::Config> glconfig2;//Creating separate configs for each window. Is this really necessary? It does not do anything yet, but hopefully will form a nucleus to the solution to the shared viewport problem.
            glconfig2 = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |      Gdk::GL::MODE_DEPTH  |     Gdk::GL::MODE_DOUBLE);
            if (glconfig2==NULL){
               glconfig2 = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |    Gdk::GL::MODE_DEPTH);
               if(glconfig2==NULL)std::exit(1);
            }
            Profile* prof1 = new Profile(glconfig2,lidardata,bucketlimit,rulerlabel);
            prof = prof1;
            prof->set_size_request(200,200);
            vboxprof->pack_end(*prof,true,true);
         }
         window3->show_all();
      }
//      Gtk::Window *window4 = NULL;
//      refXml->get_widget("window4", window4);
//      window4->set_title("Window4");
//      window4->show();
//      gtkmain.run(*windowp);   
      gtkmain.run(*window2);
   } else {
      std::cerr << "eep, no main window?" << std::endl;
      return 1;
   }
   return 0;
}

int main(int argc, char** argv) {
   try{//Attempt to get real files.
      string pointoffset,filename;
      if(argc < 3){
         cout << "Form is \"lag <point skip number> <first file> [other files]...\"" << endl;
         return 11;
      }
      exename.append(argv[0]);
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
               if(count==2)lidardata = new quadtree(loader,bucketlimit,poffs);//First time make quadtree...
               else lidardata->load(loader,poffs);//... otherwise add to it.
               cout << filename << endl;
               delete loader;
            }
            else if(filename.find(".txt",filename.length()-4)!=string::npos||filename.find(".TXT",filename.length()-4)!=string::npos){//For ASCII files:
               ostringstream filenum;
               filenum << count+1;
               cout << "Type code for file " + filenum.str() << endl;//This code is needed in order to properly interpret the ASCII file.
               string code1;
               getline(cin,code1);
               const char* code = code1.c_str();
               cout << code << endl;
               ASCIIloader* aloader = new ASCIIloader(argv[count],code);
               if(count==2)lidardata = new quadtree(aloader,bucketlimit,poffs);//First time make quadtree...
               else lidardata->load(aloader,poffs);//... otherwise add to it.
               cout << filename << endl;
               delete aloader;
            }
            else{//For incorrect file extensions:
               cout << "Files must have the extensions .las, .LAS, .txt or .TXT." << endl;
               return 17;
            }
         }
      }
//       The following block of commented out code is for testing purposes:
//       LASloader* loader = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_111020_1.LAS");
//       LASloader* loader3 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_110312_1.LAS");
//       LASloader* loader6 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_111753_1.LAS");
//       LASloader* loader7 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_112149_1.LAS");
//       LASloader* loader8 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_112524_1.LAS");
//       LASloader* loader9 = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_112915_1.LAS");
//       LASloader* loadera = new LASloader("/users/rsg/harg/hargsvn/lag/lag/las_files/LDR090601_113327_1.LAS");
//       lidardata = new quadtree(loader,bucketlimit,3);
//       lidardata->load(loader3,3);
//       lidardata->load(loader6,3);
//       lidardata->load(loader7,3);
//       lidardata->load(loader8,3);
//       lidardata->load(loader9,3);
//       lidardata->load(loadera,3);
//       delete loader;
//       delete loader3;
//       delete loader6;
//       delete loader7;
//       delete loader8;
//       delete loader9;
//       delete loadera;
   }
   catch(char const* e){
      cout << e << endl;
      cout << "Please check to make sure your files exist and the paths are properly spelled." << endl;
      return 22;
   }
   return GUIset(argc, argv);//Make the GUI.
   delete lidardata;
}
