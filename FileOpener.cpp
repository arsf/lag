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
 * File: FileOpener.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: June-July 2010
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <vector>
#include "TwoDeeOverview.h"
#include "Profile.h"
#include "FileOpener.h"

FileOpener::FileOpener(TwoDeeOverview *tdo,Profile *prof,Glib::RefPtr<Gnome::Glade::Xml> refXml,AdvancedOptionsWindow *aow,FileSaver *fs,Quadtree *lidardata,int bucketlimit,Gtk::EventBox *eventboxtdo,Gtk::EventBox *eventboxprof,TwoDeeOverviewWindow *tdow){
   this->tdo = tdo;
   this->prof = prof;
   this->aow = aow;
   this->fs = fs;
   this->lidardata = lidardata;
   this->bucketlimit = bucketlimit;
   this->eventboxtdo = eventboxtdo;
   this->eventboxprof = eventboxprof;
   this->tdow = tdow;
   time_t starttime = time(NULL);
   char meh[80];
   strftime(meh, 80, "%Y.%m.%d(%j).%H-%M-%S.%Z", localtime(&starttime));
   ostringstream bleh;
   bleh << meh;
   loaderroroutputfile = "/tmp/LAGloadingerrors" + bleh.str() + ".txt";
   loaderroroutput.open(loaderroroutputfile.c_str());
   loaderrorstream = new ostringstream();
   loadedanyfiles = false;
   cachelimit = 25000000;
   Gtk::MenuItem *openfilemenuitem = NULL;//For selecting to get file-opening menu.
   refXml->get_widget("openfilemenuitem",openfilemenuitem);
   if(openfilemenuitem)openfilemenuitem->signal_activate().connect(sigc::mem_fun(*this,&FileOpener::on_openfilemenuactivated));
   refXml->get_widget("filechooserdialog",filechooserdialog);
   if(filechooserdialog)filechooserdialog->signal_response().connect(sigc::mem_fun(*this,&FileOpener::on_filechooserdialogresponse));
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
         cachesizeselect->signal_value_changed().connect(sigc::mem_fun(*this,&FileOpener::on_cachesize_changed));
      }
   }
   refXml->get_widget("loadoutputlabel",loadoutputlabel);
   refXml->get_widget("resbaseselect",resbaseselect);
   if(resbaseselect){
      resbaseselect->set_range(2,1000);//What is the probability someone will seriously want resolutions of 1000^(-x)? Note that a value of less than 2 would cause serious problems.
      resbaseselect->set_value(4);
      resbaseselect->set_increments(1,1);
   }
   refXml->get_widget("resdepthselect",resdepthselect);
   if(resdepthselect){
      on_resolutionbase_changed();
      resbaseselect->signal_value_changed().connect(sigc::mem_fun(*this,&FileOpener::on_resolutionbase_changed));
      resdepthselect->set_value(4);
      resdepthselect->set_increments(1,1);
   }
   numlines = 0;
}
FileOpener::~FileOpener(){
   loaderroroutput.close();
   delete loadoutputlabel;
   delete pointskipselect;
   delete fenceusecheck;
   delete asciicodeentry;
   delete cachesizeselect;
   delete cachesizeGBlabel;
   delete loaderrorstream;
   delete filechooserdialog;//Have to delete parent after children?
}

void FileOpener::on_resolutionbase_changed(){
   resdepthselect->set_range(0,log(pow(2,sizeof(int)*8)/100)/log(resbaseselect->get_value_as_int()));//This is absolutely vital to prevent an overflow.
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
int FileOpener::testfilename(int argc,char *argv[],bool start,bool usearea){
   int resolutiondepth = resdepthselect->get_value_as_int();
   int resolutionbase = resbaseselect->get_value_as_int();
   int bucketlevels = 0;
   bool newQuadtree = false;
   loadoutputlabel->set_text("");
   Gdk::Window::process_all_updates();
   cachelimit = cachesizeselect->get_value();
   if(start || !loadedanyfiles || lidardata==NULL)numlines = 0;
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
      if(usearea){
         for(int count = 2;count<argc;count++){//We start after the executable path and the point offset.
            numlines++;
            filename.assign(argv[count]);
            if(filename != ""){
               double *fencexs = NULL,*fenceys = NULL;//These are NOT to be deleted here as the arrays they will point to are managed by the TwoDeeOVerview object.
               int fenceps = 0;
               if(tdo->is_realized())tdo->getfence(fencexs,fenceys,fenceps);
               if(fencexs!=NULL&&fenceys!=NULL){
                  LidarPointLoader *loader = NULL;
                  bool validfile = true;
                  if(filename.find(".las",filename.length()-4)!=string::npos ||
                     filename.find(".LAS",filename.length()-4)!=string::npos)loader = new LasLoader(argv[count]);//For LAS files.
                  else if(filename.find(".txt",filename.length()-4)!=string::npos ||
                          filename.find(".TXT",filename.length()-4)!=string::npos){//For ASCII files (only works through GUI... Must get it to work for command-line at some point:
                     string code1 = asciicodeentry->get_text();//The type code is needed to properly interpret the ASCII file.
                     loader = new AsciiLoader(argv[count],code1.c_str());
                  }
                  else{//For incorrect file extensions:
                     string message = "Files must have the extensions .las, .LAS, .txt or .TXT.";
                     cout << message << endl;
                     loadoutputlabel->set_text(loadoutputlabel->get_text() + message + "\n");
                     Gdk::Window::process_all_updates();
                     validfile = false;
                  }
                  if(validfile){
                     if((count==2 && (start || !loadedanyfiles))||lidardata==NULL){//If refreshing (or from command-line) use first filename to make quadtree...
                        if(lidardata != NULL)delete lidardata;
                        lidardata = NULL;//This prevents a double free if the creation of the new quadtree fails and throws an exception.
                        loaderrorstream->str("");
                        lidardata = new Quadtree(loader,bucketlimit,poffs,fencexs,fenceys,fenceps,cachelimit,bucketlevels,resolutionbase,resolutiondepth,loaderrorstream);
                        int numberofpointsloaded = lidardata->getNumberOfPoints();
                        if(numberofpointsloaded == 0){
                           string message = "No points loaded from file, either because of a lack of points or because points lie outside quadtree boundary (possibly because of fence). Please check file.";
                           cout << message << endl;
                           loadoutputlabel->set_text(loadoutputlabel->get_text() + message + "\n");
                           Gdk::Window::process_all_updates();
                        }
                        newQuadtree = true;
                     }
                     else{
                        int numberofpointsloaded = lidardata->load(loader,poffs,bucketlevels,fencexs,fenceys,fenceps);
                        if(numberofpointsloaded == 0){
                           string message = "No points loaded from file, either because of a lack of points or because points lie outside quadtree boundary (possibly because of fence). Please check file.";
                           cout << message << endl;
                           loadoutputlabel->set_text(loadoutputlabel->get_text() + message + "\n");
                           Gdk::Window::process_all_updates();
                        }
                     }
                  }
                  if(loader != NULL)delete loader;
               }
               else{
                  cout << "No fence!" << endl;
                  loadoutputlabel->set_text(loadoutputlabel->get_text() + "No fence!\n");
                  Gdk::Window::process_all_updates();
                  return 222;
               }
            }
            cout << filename << endl;
            loadoutputlabel->set_text(loadoutputlabel->get_text() + filename + "\n");
            Gdk::Window::process_all_updates();
            if(loaderrorstream->str()!=""){
               string message = "There have been errors in loading. Please see the file " + loaderroroutputfile;
               cout << message << endl;
               loadoutputlabel->set_text(loadoutputlabel->get_text() + message + "\n");
               Gdk::Window::process_all_updates();
               loaderroroutput << filename << endl;
               loaderroroutput << loaderrorstream->str();
               loaderroroutput.flush();
               loaderrorstream->str("");
            }
         }
      }
      else{
         double minx=0,maxx=0,miny=0,maxy=0;
         if(start || !loadedanyfiles || lidardata==NULL){
            for(int count = 2;count<argc;count++){//We start after the executable path and the point offset.
               filename.assign(argv[count]);
               if(filename != ""){
                  LidarPointLoader *loader = NULL;
                  if(filename.find(".las",filename.length()-4)!=string::npos||filename.find(".LAS",filename.length()-4)!=string::npos){//For las files:
                     loader = new LasLoader(argv[count]);
                  }
                  else if(filename.find(".txt",filename.length()-4)!=string::npos||filename.find(".TXT",filename.length()-4)!=string::npos){//For ASCII files (only works through GUI... Must get it to work for command-line at some point:
                     string code1 = asciicodeentry->get_text();//The type code is needed to properly interpret the ASCII file.
                     loader = new AsciiLoader(argv[count],code1.c_str());
                  }
                  Boundary lidarboundary = loader->getBoundary();
                  if(lidarboundary.minX < minx || count == 2)minx = lidarboundary.minX;
                  if(lidarboundary.maxX > maxx || count == 2)maxx = lidarboundary.maxX;
                  if(lidarboundary.minY < miny || count == 2)miny = lidarboundary.minY;
                  if(lidarboundary.maxY > maxy || count == 2)maxy = lidarboundary.maxY;
                  if(loader != NULL)delete loader;
               }
            }
         }
         for(int count = 2;count<argc;count++){//We start after the executable path and the point offset.
            numlines++;
            filename.assign(argv[count]);
            if(filename != ""){
               LidarPointLoader *loader = NULL;
               bool validfile = true;
               if(filename.find(".las",filename.length()-4)!=string::npos ||
                  filename.find(".LAS",filename.length()-4)!=string::npos)loader = new LasLoader(argv[count]);//For LAS files.
               else if(filename.find(".txt",filename.length()-4)!=string::npos ||
                       filename.find(".TXT",filename.length()-4)!=string::npos){//For ASCII files (only works through GUI... Must get it to work for command-line at some point:
                  string code1 = asciicodeentry->get_text();//The type code is needed to properly interpret the ASCII file.
                  const char* code = code1.c_str();
                  loader = new AsciiLoader(argv[count],code);
               }
               else{//For incorrect file extensions:
                  string message = "Files must have the extensions .las, .LAS, .txt or .TXT.";
                  cout << message << endl;
                  loadoutputlabel->set_text(loadoutputlabel->get_text() + message + "\n");
                  Gdk::Window::process_all_updates();
                  validfile = false;
               }
               if(validfile){
                  if((count==2 && (start || !loadedanyfiles))||lidardata==NULL){//If refreshing (or from command-line) use first filename to make quadtree...
                     if(lidardata != NULL)delete lidardata;
                     lidardata = NULL;//This prevents a double free if the creation of the new quadtree fails and throws an exception.
                     loaderrorstream->str("");
                     lidardata = new Quadtree(minx,miny,maxx,maxy,bucketlimit,cachelimit,bucketlevels,resolutionbase,resolutiondepth,loaderrorstream);
                     int numberofpointsloaded = lidardata->load(loader,poffs,bucketlevels);
                     if(numberofpointsloaded == 0){
                        string message = "No points loaded from file, either because of a lack of points or because points lie outside quadtree boundary (possibly because of fence). Please check file.";
                        cout << message << endl;
                        loadoutputlabel->set_text(loadoutputlabel->get_text() + message + "\n");
                        Gdk::Window::process_all_updates();
                     }
                     newQuadtree = true;
                  }
                  else{
                     int numberofpointsloaded = lidardata->load(loader,poffs,bucketlevels);
                     if(numberofpointsloaded == 0){
                        string message = "No points loaded from file, either because of a lack of points or because points lie outside quadtree boundary (possibly because of fence). Please check file.";
                        cout << message << endl;
                        loadoutputlabel->set_text(loadoutputlabel->get_text() + message + "\n");
                        Gdk::Window::process_all_updates();
                     }
                  }
               }
               if(loader != NULL)delete loader;
            }
            cout << filename << endl;
            loadoutputlabel->set_text(loadoutputlabel->get_text() + filename + "\n");
            Gdk::Window::process_all_updates();
            if(loaderrorstream->str()!=""){
               string message = "There have been errors in loading. Please see the file " + loaderroroutputfile;
               cout << message << endl;
               loadoutputlabel->set_text(loadoutputlabel->get_text() + message + "\n");
               Gdk::Window::process_all_updates();
               loaderroroutput << filename << endl;
               loaderroroutput << loaderrorstream->str();
               loaderroroutput.flush();
               loaderrorstream->str("");
            }
         }
      }
   }
   catch(DescriptiveException e){
      string message = "There has been an exception:\n";
      message += "What: " + static_cast<string>(e.what());
      message += "\nWhy: " + static_cast<string>(e.why());
      cout << message << endl;
      loadoutputlabel->set_text(loadoutputlabel->get_text() + message + "\n");
      Gdk::Window::process_all_updates();
      loaderrorstream->str("");
      if(lidardata != NULL)delete lidardata;
      lidardata = NULL;
      tdo->hide_all();
      eventboxtdo->remove();
      prof->hide_all();
      eventboxprof->remove();
      loadedanyfiles = false;
      numlines = 0;
      newQuadtree = false;
      return 22;
   }
   tdo->setlidardata(lidardata,bucketlimit);//Provide the drawing objects access to the quadtree:
   prof->setlidardata(lidardata,bucketlimit);//...
   if(newQuadtree){
      tdo->setresolutionbase(resolutionbase);
      tdo->setresolutiondepth(resolutiondepth);
      aow->setmaindetailrange(0,log(pow(2,sizeof(int)*8)/100)/log(resbaseselect->get_value_as_int()));//This is absolutely vital to prevent an overflow in the drawing thread in the overview that then causes an infinite loop.
   }
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
   loadedanyfiles = true;
   string flightline,list="";
   for(int i = 0;i<numlines;i++){
      flightline = lidardata->getFileName(i);
      ostringstream number;
      number << numlines;
      list += number.str() + ":  " + flightline + "\n";
   }
   fs->setlidardata(lidardata);
   fs->setlabeltext(list);
   fs->setlinerange(0,numlines-1);
   tdow->setraiselinerange(0,numlines-1);
   aow->resetcolouringandshading();//(Re)Set the advanced colouring and shading options to the values indicated by the recently loaded flightlines.
   return 0;
}

//If either the add or refresh button is pressed, then this function takes the selected filenames and creates an imitation of a command-line command, which is then sent to testfilename() where the file will be opened.
void FileOpener::on_filechooserdialogresponse(int response_id){
   if(response_id == Gtk::RESPONSE_CLOSE)filechooserdialog->hide_all();
   else if(response_id == 1 || response_id == 2){
      Glib::SListHandle<Glib::ustring> names = filechooserdialog->get_filenames();
      int argc = names.size() + 2;//testfilename expects a command-line command in the form: <this program> <point offset <file 1> [file 2]...
      char** argv = new char*[argc];
      string exename = "blah";
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
void FileOpener::on_cachesize_changed(){
   ostringstream GB;
   GB << ((double)cachesizeselect->get_value()*sizeof(Point))/1000000000;
   string labelstring = "Approximately: " + GB.str() + " GB.";
   cachesizeGBlabel->set_text(labelstring);
}

//When selected from the menu, the file chooser opens.
void FileOpener::on_openfilemenuactivated(){ show(); }
