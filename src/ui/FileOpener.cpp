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
#include <gtkglmm.h>
#include <vector>
#include "../TwoDeeOverview.h"
#include "../Profile.h"
#include "../SelectionBox.h"
#include "FileOpener.h"

FileOpener::FileOpener(TwoDeeOverview *tdo, Profile *prof, const Glib::RefPtr<Gtk::Builder>& builder, AdvancedOptionsWindow *aow, FileSaver *fs,
           	   	   	   int bucketlimit, Gtk::EventBox *eventboxtdo, Gtk::EventBox *eventboxprof, TwoDeeOverviewWindow *tdow)
:
        tdo				(tdo),
        prof			(prof),
        tdow			(tdow),
        aow				(aow),
        fs				(fs),
        lidardata		(NULL),
        eventboxtdo		(eventboxtdo),
        eventboxprof	(eventboxprof),
        bucketlimit		(bucketlimit)
{
	load_xml(builder);

	time_t starttime = time(NULL);

	char meh[80];
	strftime(meh, 80, "%Y.%m.%d(%j).%H-%M-%S.%Z", localtime(&starttime));
	ostringstream bleh;
	bleh << meh;

	loaderrorstream = new ostringstream();
	loadedanyfiles = false;
	cachelimit = 35000000;

	cachesizeselect->set_range(10e6,10e11);

	cachesizeselect->set_value(350e5);
	cachesizeselect->set_increments(10e5,10e5);

	resbaseselect->set_range(2,1000);
	resbaseselect->set_value(5);
	resbaseselect->set_increments(1,1);

	resdepthselect->set_range(1,10);
	resdepthselect->set_value(4);
	resdepthselect->set_increments(1,1);

	numlines = 0;

	connect_signals();

	on_cachesize_changed();
}


FileOpener::~FileOpener()
{
   delete loadoutputlabel;
   delete pointskipselect;
   delete fenceusecheck;
   delete asciicodeentry;
   delete cachesizeselect;
   delete cachesizeGBlabel;
   delete loaderrorstream;
   delete scaleFactorEntryX;
   delete scaleFactorEntryY;
   delete scaleFactorEntryZ;
   delete btnUseDefault;
   delete openbutton;
   //Have to delete parent after children?
   delete filechooserdialog;
}

void FileOpener::load_xml(const Glib::RefPtr<Gtk::Builder>& builder)
{
	builder->get_widget("openfilemenuitem",openfilemenuitem);
	builder->get_widget("filechooserdialog",filechooserdialog);
	builder->get_widget("pointskipselect",pointskipselect);
	builder->get_widget("fenceusecheck",fenceusecheck);
	builder->get_widget("asciicodeentry",asciicodeentry);
	builder->get_widget("cachesizeselect",cachesizeselect);
	builder->get_widget("scaleFactorEntryX", scaleFactorEntryX);
	builder->get_widget("scaleFactorEntryY", scaleFactorEntryY);
	builder->get_widget("scaleFactorEntryZ", scaleFactorEntryZ);
	builder->get_widget("btnUseDefault", btnUseDefault);
	builder->get_widget("cachesizeGBlabel",cachesizeGBlabel);
	builder->get_widget("loadoutputlabel",loadoutputlabel);
	builder->get_widget("resbaseselect",resbaseselect);
	builder->get_widget("resdepthselect",resdepthselect);
	builder->get_widget("openbutton",openbutton);
}

void FileOpener::connect_signals()
{
	openfilemenuitem->signal_activate().connect(sigc::mem_fun(*this,&FileOpener::on_openfilemenuactivated));
	openbutton->signal_clicked().connect(sigc::mem_fun(*this,&FileOpener::on_openfilemenuactivated));
	filechooserdialog->signal_response().connect(sigc::mem_fun(*this,&FileOpener::on_filechooserdialogresponse));
	btnUseDefault->signal_toggled().connect(sigc::mem_fun(*this,&FileOpener::on_usedefault_changed));
	cachesizeselect->signal_value_changed().connect(sigc::mem_fun(*this,&FileOpener::on_cachesize_changed));
	resbaseselect->signal_value_changed().connect(sigc::mem_fun(*this,&FileOpener::on_resolutionbase_changed));
    Gtk::Main::signal_quit().connect(sigc::mem_fun(*this, &FileOpener::on_quit));
}

int FileOpener::on_quit()
{
	Gtk::Window* profw = tdow->get_profilewindow();
	if (profw->get_realized())
		profw->hide_all();

	if (lidardata != NULL)
	{
		std::cout << "Cleaning up..." << std::endl;
		delete lidardata;
	}

	return 0;
}


void FileOpener::on_usedefault_changed()
{
	bool temp = !(btnUseDefault->get_active());

	scaleFactorEntryX->set_sensitive(temp);
	scaleFactorEntryY->set_sensitive(temp);
	scaleFactorEntryZ->set_sensitive(temp);
}


void FileOpener::on_resolutionbase_changed()
{
   //This is absolutely vital to prevent an overflow.
   resdepthselect->set_range(0,log(pow(2,sizeof(int)*8)/100)/log(resbaseselect->get_value_as_int()));
}


/*Determines whether the input filename(s) are correct and, if so, creates or 
 * modifies the quadtree to accomodate the data. First it makes sure that a 
 * sufficient number of arguments have been passed to include the executable, 
 * point offset and at least one filename. It then extracts the point offset, 
 * which is used to skip a certain number of points between each read point, 
 * for faster loading. It then starts dealing withthe filenames:
 * Try:
 *    For all the filenames:
 *       If the filename is not empty:
 *          If the filename ends with .las or .LAS:
 *             Create LASLoader;
 *             If this is the first filename AND either the user has pressed 
 *             the refresh button or no files have yet loaded OR the quadtree 
 *             is empty:
 *                If using a fence:
 *                   Delete old quadtree (including the original dummy one if 
 *                   the program has just started);
 *                   Get fence coordinates;
 *                   Create new quadtree with data with fence;
 *                Else:
 *                   Delete old quadtree (including the original dummy one if 
 *                   the program has just started);
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
 *             If this is the first filename AND either the user has pressed 
 *             the refresh button or no files have yet loaded OR the quadtree 
 *             is empty:
 *                If using a fence:
 *                   Delete old quadtree (including the original dummy one if 
 *                   the program has just started);
 *                   Get fence coordinates;
 *                   Create new quadtree with data with fence;
 *                Else:
 *                   Delete old quadtree (including the original dummy one if 
 *                   the program has just started);
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
 * Try:
 *    For all the filenames:
 *       If using a fence:
 *          If the filename is not empty:
 *                Create LASLoader or ASCIILoader depending on file type;
 *                If this is the first filename AND either the user has 
 *                pressed the refresh button or no files have yet loaded OR 
 *                the quadtree is empty:
 *                      Delete old quadtree (including the original dummy one 
 *                      if the program has just started);
 *                      Get fence coordinates;
 *                      Create new quadtree with data with fence;
 *                Else:
 *                      Get fence coordinates;
 *                      Add data with fence to quadtree;
 *                Write any errors to the error file;
 *          Else do nothing;
 *       Else:
 *          Determine boundary of quadtree from all lines to be loaded (not 
 *          necessary for fence as fence is a boundary);
 *          If the filename is not empty:
 *                Create LASLoader or ASCIILoader depending on file type;
 *                If this is the first filename AND either the user has 
 *                pressed the refresh button or no files have yet loaded OR 
 *                the quadtree is empty:
 *                      Delete old quadtree (including the original dummy one 
 *                      if the program has just started);
 *                      Create new quadtree with data;
 *                Else:
 *                      Add data to quadtree;
 *                Write any errors to the error file;
 *          Else do nothing;
 * Catch:
 *    Empty the quadtree;
 *
 * Then a pointer to the data is sent to all of the display areas, which are 
 * then prepared for displaying and then display.
 *
 *
 * */

int FileOpener::testfilename(int argc,char *argv[],bool start,bool usearea)
{
   int resolutiondepth = resdepthselect->get_value_as_int();
   int resolutionbase = resbaseselect->get_value_as_int();
   int bucketlevels = 0;
   bool newQuadtree = false;

   // These variables store min and max Z values used for slicing
   double minZ=0, maxZ=0;
   bool zSet = false;

   loadoutputlabel->set_text("");
   Gdk::Window::process_all_updates();
   cachelimit = cachesizeselect->get_value();

   if(start || !loadedanyfiles || lidardata==NULL) numlines = 0;

   try{
	  //Attempt to get real files.
      string pointoffset,filename;

      if(argc < 3)
      {
         return 1;
      }

      pointoffset.append(argv[1]);

      // This returns the integer translation of the string, or zero 
      // if there is none, so...
      int poffs = atoi(pointoffset.c_str());

      // in the situation where there is a value of zero, check the string to 
      // see whether this is because of there being a zero value or because of 
      // there not being an integer. If the value in the string is not zero:
      if(poffs == 0 && pointoffset != "0")
      {
         cout << "The point offset must be an integer greater than or "
              << "equal to zero. In addition, zero can only be accepted "
              << "in the form \"0\", not \"00\" etc.." << endl;
         return 1;
      }

      if(usearea)
      {
         //We start after the executable path and the point offset.
         for(int count = 2;count<argc;count++)
         {
            numlines++;
            filename.assign(argv[count]);

            if(filename != "")
            {
               //These are NOT to be deleted here as the arrays they will point 
               //to are managed by the TwoDeeOVerview object.
               SelectionBox fence;
               //int numberOfFenceCorners = 0;

               if(tdo->is_realized())
               {
                  fence = tdo->getFence();
               }

               // Please fix this!
               if (true)
               {
                  LidarPointLoader *loader = NULL;
                  bool validfile = true;

                  //For LAS files.
                  if(filename.find(".las",filename.length()-4)!=string::npos ||
                     filename.find(".LAS",filename.length()-4)!=string::npos)
                  {
                	  loader = new LasLoader(argv[count]);
                  }

                  // For ASCII files
                  else if(filename.find(".txt",filename.length()-4) !=string::npos ||
                          filename.find(".TXT",filename.length()-4) !=string::npos)
                  {

                	 if (btnUseDefault->get_active())
                	 {
                		 // The type code is needed to properly interpret the
                		 // ASCII file.
                		 string code1 = asciicodeentry->get_text();
                		 loader = new LasLoader(argv[count],code1.c_str());
                	 }
                	 else
                	 {
                		 double scale_factor[3];
                		 const char* temp;
                		 temp = scaleFactorEntryX->get_text().c_str();
                		 scale_factor[0] = atof(temp);
                		 temp = scaleFactorEntryY->get_text().c_str();
                		 scale_factor[1] = atof(temp);
                		 temp = scaleFactorEntryZ->get_text().c_str();
                		 scale_factor[2] = atof(temp);

                		 temp = NULL;
                		 string code1 = asciicodeentry->get_text();
                		 loader = new LasLoader(argv[count], code1.c_str(), scale_factor);
                	 }
                  }

                  //For incorrect file extensions:
                  else
                  {
                     string message = "Files must have the extensions .las, .LAS, .txt or .TXT.";
                     cout << message << endl;
                     Gdk::Window::process_all_updates();
                     validfile = false;
                  }

                  if(validfile)
                  {
                     // If refreshing (or from command-line) use first filename 
                     // to make quadtree...
                     if((count==2 && (start || !loadedanyfiles)) || lidardata==NULL)
                     {
                        if(lidardata != NULL)
                        {
                           delete lidardata;
                        }

                        // This prevents a double free if the creation of the 
                        // new quadtree fails and throws an exception.
                        lidardata = NULL;
                        loaderrorstream->str("");
                        lidardata = new Quadtree(loader,bucketlimit,poffs, fence.getXs(), fence.getYs(), 4, cachelimit,
                                           bucketlevels,resolutionbase, resolutiondepth,loaderrorstream);

                        int numberofpointsloaded =lidardata->getNumberOfPoints();

                        if(numberofpointsloaded == 0)
                        {
                           string message =" No points loaded from file, either because of a lack of points or because points \
                                                  lie outside quadtree boundary (possibly because of fence). Please check file.";
                           cout << message << endl;

                           Gdk::Window::process_all_updates();
                           numlines--;
                        }
                        newQuadtree = true;
                     }
                     else
                     {
                        int numberofpointsloaded = lidardata->load(loader, poffs,bucketlevels, fence.getXs(), fence.getYs(), 4);

                        if(numberofpointsloaded == 0){
                           string message =" No points loaded from file, either because of a lack of points or because points \
                                             lie outside quadtree boundary (possibly because of fence). Please check file.";

                           cout << message << endl;

                           Gdk::Window::process_all_updates();
                           numlines--;
                        }
                     }
                  }
                  if (!zSet)
                  {
                	  minZ = maxZ = loader->getMinZ();
                	  zSet = true;
                  }

                  if (loader->getMinZ() < minZ)
                	  minZ = loader->getMinZ();
                  if (loader->getMaxZ() > maxZ)
                	  maxZ = loader->getMaxZ();

                  tdow->set_utm_zone(loader->get_utm_zone());

                  if(loader != NULL) delete loader;
               }
               else
               {
                  cout << "No fence!" << endl;
                  loadoutputlabel->set_text(loadoutputlabel->get_text() + "No fence!\n");
                  Gdk::Window::process_all_updates();
                  return 222;
               }
            }
            cout << filename << endl;
            loadoutputlabel->set_text(loadoutputlabel->get_text() + filename + "\n");

            Gdk::Window::process_all_updates();

            if(loaderrorstream->str()!="")
            {
               string message = "There have been errors in loading.";
               cerr << message << endl;

               Gdk::Window::process_all_updates();

               loaderrorstream->str("");
            }
         }
      }
      else
      {
         double minx=0,maxx=0,miny=0,maxy=0;

         if(start || !loadedanyfiles || lidardata==NULL)
         {
        	//We start after the executable path and the point offset.
            for(int count = 2;count<argc;count++)
            {
               filename.assign(argv[count]);

               if(filename != "")
               {
                  LidarPointLoader *loader = NULL;

                  // For las files:
                  if(filename.find(".las",filename.length()-4)!=string::npos ||
                     filename.find(".LAS",filename.length()-4)!=string::npos)
                  {
                     loader = new LasLoader(argv[count]);
                  }

                  // For ASCII files (only works through GUI... Must get it to 
                  // work for command-line at some point:
                  else if(filename.find(".txt",filename.length()-4) != string::npos ||
                          filename.find(".TXT",filename.length()-4) != string::npos)
                  {
                	  if (btnUseDefault->get_active())
                   	  {
                		  // The type code is needed to properly interpret the
                	      // ASCII file.
                	      string code1 = asciicodeentry->get_text();
                	      loader = new LasLoader(argv[count],code1.c_str());
                   	  }
                	  else
                	  {
                		  double scale_factor[3];
                		  const char* temp;
                		  temp = scaleFactorEntryX->get_text().c_str();
                		  scale_factor[0] = atof(temp);
                		  temp = scaleFactorEntryY->get_text().c_str();
                		  scale_factor[1] = atof(temp);
                		  temp = scaleFactorEntryZ->get_text().c_str();
                		  scale_factor[2] = atof(temp);

                		  temp = NULL;
                		  string code1 = asciicodeentry->get_text();
                		  loader = new LasLoader(argv[count], code1.c_str(), scale_factor);
                	  }
                  }

                  Boundary lidarboundary = loader->getBoundary();
                  if(lidarboundary.minX < minx || count == 2)
                     minx = lidarboundary.minX;
                  if(lidarboundary.maxX > maxx || count == 2)
                     maxx = lidarboundary.maxX;
                  if(lidarboundary.minY < miny || count == 2)
                     miny = lidarboundary.minY;
                  if(lidarboundary.maxY > maxy || count == 2)
                     maxy = lidarboundary.maxY;

                  if (!zSet)
                  {
                	  minZ = maxZ = loader->getMinZ();
                	  zSet = true;
                  }
                  if (loader->getMinZ() < minZ)
                	  minZ = loader->getMinZ();
                  if (loader->getMaxZ() > maxZ)
                	  maxZ = loader->getMaxZ();

                  tdow->set_utm_zone(loader->get_utm_zone());

                  if(loader != NULL)
                     delete loader;
               }
            }
         }

         //We start after the executable path and the point offset.
         for(int count = 2;count<argc;count++)
         {
            numlines++;
            filename.assign(argv[count]);

            if(filename != "")
            {
               LidarPointLoader *loader = NULL;
               // bool validfile = true;

               //For LAS files.
               if(filename.find(".las",filename.length()-4)!=string::npos ||
                  filename.find(".LAS",filename.length()-4)!=string::npos)
               {
            	   loader = new LasLoader(argv[count]);
               }

               // For ASCII files
               else if(filename.find(".txt",filename.length()-4) != string::npos ||
                       filename.find(".TXT",filename.length()-4) != string::npos)
               {
             	  if (btnUseDefault->get_active())
                	  {
             		  // The type code is needed to properly interpret the
             	      // ASCII file.
             	      string code1 = asciicodeentry->get_text();
             	      loader = new LasLoader(argv[count],code1.c_str());
                	  }
             	  else
             	  {
            		  double scale_factor[3];
            		  const char* temp;
            		  temp = scaleFactorEntryX->get_text().c_str();
            		  scale_factor[0] = atof(temp);
            		  temp = scaleFactorEntryY->get_text().c_str();
            		  scale_factor[1] = atof(temp);
            		  temp = scaleFactorEntryZ->get_text().c_str();
            		  scale_factor[2] = atof(temp);

            		  temp = NULL;
            		  string code1 = asciicodeentry->get_text();
            		  loader = new LasLoader(argv[count], code1.c_str(), scale_factor);
             	  }
               }

                  // If refreshing (or from command-line) use first filename 
                  // to make quadtree...
                  if((count==2 && (start || !loadedanyfiles)) || lidardata==NULL)
                  {
                     if(lidardata != NULL)
                     {
                        delete lidardata;
                     }

                     // This prevents a double free if the creation of the new 
                     // quadtree fails and throws an exception.
                     lidardata = NULL;
                     loaderrorstream->str("");
                     
                     lidardata = new Quadtree(minx,miny,maxx,maxy,bucketlimit,
                                              cachelimit,bucketlevels,
                                              resolutionbase,resolutiondepth,
                                              loaderrorstream);
                     int numberofpointsloaded = lidardata->load(loader,poffs,
                                                                bucketlevels);
                     if(numberofpointsloaded == 0)
                     {
                        string message = "No points loaded from file, either because of a lack of points or because points "
                        		"lie outside quadtree boundary (possibly because of fence). Please check file.";
                        cout << message << endl;
                        // loadoutputlabel->set_text(loadoutputlabel->get_text() + message + "\n");
                        Gdk::Window::process_all_updates();
                        numlines--;
                     }
                     newQuadtree = true;
                  }
                  else
                  {
                     int numberofpointsloaded = lidardata->load(loader,poffs,bucketlevels);
                     if(numberofpointsloaded == 0)
                     {
                        string message = "No points loaded from file, either because of a lack of points or \
                                          because points lie outside quadtree boundary (possibly because \
                                          of fence). Please check file.";
                        cout << message << endl;
                        loadoutputlabel->set_text(loadoutputlabel->get_text() + message + "\n");
                        Gdk::Window::process_all_updates();
                        numlines--;
                     }
                  }

                  if (!zSet)
                  {
                	  minZ = maxZ = loader->getMinZ();
                	  zSet = true;
                  }
                  if (loader->getMinZ() < minZ)
                	  minZ = loader->getMinZ();
                  if (loader->getMaxZ() > maxZ)
                	  maxZ = loader->getMaxZ();

                  tdow->set_utm_zone(loader->get_utm_zone());

                  if(loader != NULL)delete loader;
            }
            cout << filename << endl;
            loadoutputlabel->set_text(loadoutputlabel->get_text() + filename + "\n");
            Gdk::Window::process_all_updates();

            if(loaderrorstream->str()!="")
            {
               string message = "There have been errors in loading.";
               cout << message << endl;
               //  loadoutputlabel->set_text(loadoutputlabel->get_text() + message + "\n");
               Gdk::Window::process_all_updates();
               loaderrorstream->str("");
            }
         }
      }
   }
   catch(DescriptiveException e)
   {
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
   //Provide the drawing objects access to the quadtree:
   tdo->setlidardata(lidardata,bucketlimit);
   prof->setlidardata(lidardata,bucketlimit);
   if(newQuadtree)
   {
      tdo->setresolutionbase(resolutionbase);
      tdo->setresolutiondepth(resolutiondepth);
      // This is absolutely vital to prevent an overflow in the drawing thread 
      // in the overview that then causes an infinite loop.
      aow->setmaindetailrange(0,log(pow(2,sizeof(int)*8)/100) / log(resbaseselect->get_value_as_int()));
   }

   // Possibly: Move two copies of this to the relevant LAS and ASCII parts, 
   // above, so that files are drawn as soon as they are loaded and as the 
   // other files are loading. This might not work because of the bug that 
   // causes the flightline(s) not to be drawn immediately after loading. 
   // UPDATE: now it seems to draw just one bucket(!!!) immediately after 
   // loading.
   
   // If drawing areas are already visible, prepare the new images and draw 
   // them.
   
   if(loadedanyfiles)
   {
      tdo->prepare_image();
      tdo->drawviewable(1);
      prof->prepare_image();
      prof->drawviewable(1);
   }

   // Otherwise, pack them into the vboxes and then show them, which will do 
   // as the above block does.
   else
   {
      eventboxtdo->add(*tdo);
      tdo->show_all();
      eventboxprof->add(*prof);
      prof->show_all();
   }

   loadedanyfiles = true;
   string flightline,list="";
   for(int i = 0;i<numlines;i++)
   {
      flightline = lidardata->getFileName(i);
      ostringstream number;
      number << i;
      list += number.str() + ":  " + flightline + "\n";
   }

   fs->setlidardata(lidardata);
   fs->setlabeltext(list);
   fs->setlinerange(0,numlines-1);
   tdow->setraiselinerange(0,numlines-1);

   tdow->set_slice_range(minZ, maxZ);

   // (Re)Set the advanced colouring and shading options to the values 
   // indicated by the recently loaded flightlines.
   aow->resetcolouringandshading();
   return 0;
}

// If either the add or refresh button is pressed, then this function takes 
// the selected filenames and creates an imitation of a command-line command, 
// which is then sent to testfilename() where the file will be opened.
void FileOpener::on_filechooserdialogresponse(int response_id)
{
   if(response_id == Gtk::RESPONSE_CLOSE)
   {
      filechooserdialog->hide_all();
   }
   else if(response_id == 1 || response_id == 2)
   {
      Glib::SListHandle<Glib::ustring> names=filechooserdialog->get_filenames();

      // testfilename expects a command-line command in the form: <this program>
      // <point offset <file 1> [file 2]...
      int argc = names.size() + 2;

      char** argv = new char*[argc];

      string exename = "blah";

      //This program.
      argv[0] = new char[exename.length()+1];
      strcpy(argv[0],exename.c_str());
      ostringstream pointoffset;
      pointoffset << pointskipselect->get_value_as_int();
      string poffs = pointoffset.str();

      //The point offset.
      argv[1] = new char[poffs.length()+1];
      strcpy(argv[1],poffs.c_str());
      argc=2;

      // Until the last iterator is reached, insert the contents of the 
      // iterators into argv.
      for(Glib::SListHandle<Glib::ustring>::iterator itera = names.begin();
          itera!=names.end(); itera++)
      {
         argv[argc] = new char[(*itera).length()+1];
         strcpy(argv[argc],(*itera).c_str());
         argc++;
      }

      if(response_id == 1)
      {
         //For adding, do not create a new quadtree (false).
         testfilename(argc,argv,false,fenceusecheck->get_active());
      }
      if(response_id == 2)
      {
         //For refreshing, do create a new quadtree (true).
         testfilename(argc,argv,true,fenceusecheck->get_active());
      }

      for(int i = 0;i < argc;i++)
         delete[] argv[i];

      delete[] argv;
   }
}

// When the cachesize (in points) is changed, this outputs the value in 
// Gigabytes (NOT Gibibytes) to a label next to it.
void FileOpener::on_cachesize_changed()
{
   ostringstream GB;
   GB << ((double)cachesizeselect->get_value()*sizeof(LidarPoint))/1000000000;
   string labelstring = "Approximately: " + GB.str() + " GB.";
   cachesizeGBlabel->set_text(labelstring);
}

//When selected from the menu, the file chooser opens.
void FileOpener::on_openfilemenuactivated()
{
   show(); 
}
