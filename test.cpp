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

quadtree* lidardata; 
Gtk::ToggleToolButton *profiletoggle = NULL;
Gtk::ToggleToolButton *showprofiletoggle = NULL;
Gtk::ToggleToolButton *rulertoggle = NULL;
Gtk::Label *rulerlabel = NULL;
TwoDeeOverview *tdo = NULL;
Profile *prof = NULL;
Gtk::SpinButton *profwidthselect = NULL;
int bucketlimit = 100000;
string exename = "";

void on_profiletoggle(){
   if(profiletoggle->get_active())tdo->setupprofile();
   else{
   	tdo->unsetupprofile();
      double startx,starty,endx,endy,width;
      tdo->getprofile(startx,starty,endx,endy,width);
      prof->showprofile(startx,starty,endx,endy,width);
   }
}

void on_showprofiletoggle(){
   tdo->setshowprofile(showprofiletoggle->get_active());
   tdo->drawviewable(1);
}

void on_rulertoggle(){
   if(rulertoggle->get_active())prof->setupruler();
   else prof->unsetupruler();
}

void on_profwidthselected(){
   tdo->setprofwidth(profwidthselect->get_value());
   tdo->makeprofbox();
   tdo->drawviewable(2);
}

int GUIset(int argc,char *argv[]){
   Gtk::Main gtkmain(argc, argv);
   Glib::RefPtr<Gnome::Glade::Xml> refXml;
   try{
      unsigned int index = exename.rfind("/");
      if(index==string::npos)index=0;
      else index++;
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
         window2->set_reallocate_redraws(true);
         Gtk::VBox *vboxtdo = NULL;
         refXml->get_widget("vboxtdo",vboxtdo);
         if(vboxtdo){
            refXml->get_widget("profiletoggle",profiletoggle);
            if(profiletoggle)profiletoggle->signal_toggled().connect(sigc::ptr_fun(&on_profiletoggle));
            refXml->get_widget("showprofiletoggle",showprofiletoggle);
            if(showprofiletoggle)showprofiletoggle->signal_toggled().connect(sigc::ptr_fun(&on_showprofiletoggle));
            refXml->get_widget("profwidthselect",profwidthselect);
            if(profwidthselect){
               profwidthselect->set_range(0,300);
               profwidthselect->set_value(5);
               profwidthselect->signal_value_changed().connect(sigc::ptr_fun(&on_profwidthselected));
            }
            Glib::RefPtr<Gdk::GL::Config> glconfig;
            glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |      Gdk::GL::MODE_DEPTH  |     Gdk::GL::MODE_DOUBLE);
            if (glconfig==NULL){
                glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |    Gdk::GL::MODE_DEPTH);
                if(glconfig==NULL)std::exit(1);
            }
            TwoDeeOverview* tdo1 = new TwoDeeOverview(glconfig,lidardata,bucketlimit);
            tdo = tdo1;
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
            Glib::RefPtr<Gdk::GL::Config> glconfig2;
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
   try{
      string fnames,pointoffset,filename;
      exename.append(argv[0]);
      pointoffset.append(argv[0]);
      int poffs = 0;
      poffs = StringToUINT(pointoffset);
      if(poffs = 0 && pointoffset != "0")return 1;
      for(int i=2;i<argc;i++)fnames.append(argv[i]);
      if(fnames==""){
         cout << "Must give a filename." << endl;
         return 2;
      }
      int spacepos=0,count=0;
      do{
         spacepos = fnames.find_first_of(" ");
         filename.assign(fnames,0,spacepos);
         fnames.erase(0,spacepos+1);
         if(filename != ""){
            if(filename.find(".las",filename.length()-4)!=string::npos||filename.find(".LAS",filename.length()-4)!=string::npos){
               const char* filnam = filename.c_str();
               LASloader* loader = new LASloader(filnam);
               if(count==0)lidardata = new quadtree(loader,bucketlimit,poffs);
               else lidardata->load(loader,poffs);
               delete loader;
            }
            else if(filename.find(".txt",filename.length()-4)!=string::npos||filename.find(".TXT",filename.length()-4)!=string::npos){
               const char* filnam = filename.c_str();
               ostringstream filenum;
               filenum << count+1;
               cout << "Type code for file " + filenum.str() << endl;
               string code1;
//               cin >> code1;
               getline(cin,code1);
               const char* code = code1.c_str();
               cout << code << endl;
               ASCIIloader* aloader = new ASCIIloader(filnam,code);
               if(count==0)lidardata = new quadtree(aloader,bucketlimit,poffs);
               else lidardata->load(aloader,poffs);
               delete aloader;
            }
         }
         count++;
      }while(spacepos!=string::npos && fnames != "");
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
      cout << "bugger" << endl;
   }
   
   return GUIset(argc, argv);
   delete lidardata;
}

