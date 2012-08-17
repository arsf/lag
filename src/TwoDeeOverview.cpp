/*
===============================================================================

 TwoDeeOverview.h

 Created on: Nov 2009
 Author: Haraldur Tristan Gunnarsson, Jan Holownia, Berin Smaldon

 LIDAR Analysis GUI (LAG), viewer for LIDAR files in .LAS or ASCII format
 Copyright (C) 2009-2012 Plymouth Marine Laboratory (PML)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

===============================================================================
*/

#include <iostream>
#include <iomanip>
#include <GL/gl.h>
#include <GL/glu.h>
#include "TwoDeeOverview.h"
#include "MathFuncs.h"
#include "geoprojectionconverter.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

// define for thread debugging
//#define THREAD_DEBUG

/*
==================================
 TwoDeeOverview::TwoDeeOverview
==================================
*/
TwoDeeOverview::TwoDeeOverview(
         const Glib::RefPtr<const Gdk::GL::Config>& config,
		   int bucketlimit, Gtk::Label *rulerlabel)
:	LagDisplay(config, bucketlimit),
		drawnsofarminx		(0),
		drawnsofarminy		(0),
		drawnsofarmaxx		(1),
		drawnsofarmaxy		(1),
		resolutionbase		(1),
		resolutiondepth	(1),
		numbuckets			(0),
		raiseline			(false),
		linetoraise			(0),
		drawnsinceload		(false),
		reversez		   	(false),
		showlegend			(false),
		showdistancescale	(false),
		drawneverything	(false),
		pointcount			(0),
		vertices			   (NULL),
		colours				(NULL),
		tdoDisplayNoise	(true),
		rulerlabel			(rulerlabel),
		profiling			(false),
		showprofile			(false),
		fencing				(false),
		showfence			(false),
		rulerwidth			(2),
		rulering			   (false),
		heightenNonC		(false),
		heightenGround		(false),
		heightenLowVeg		(false),
		heightenMedVeg		(false),
		heightenHighVeg	(false),
		heightenBuildings	(false),
		heightenNoise		(false),
		heightenMass		(false),
		heightenWater		(false),
		heightenOverlap	(false),
		heightenUndefined	(false),
		panningRefresh		(1),
		slicing				(false),
		latlong				(false),
		superzoom			(false)
{
   // Arrays for openGL input
   vertices = new float[3*bucketlimit];
   colours  = new float[3*bucketlimit];

   //Control:
   zoompower = 0.5;
   maindetailmod = 0.01;

   //Profiling:
   Colour red = Colour("red");
   Colour white = Colour("white");
   profbox = new BoxOverlay(rulerlabel,white,red);
   profbox->setcentre(centre);

   //Fencing:
   Colour blue = Colour("blue");
   fencebox = new BoxOverlay(rulerlabel,blue,blue);
   fencebox->setcentre(centre);

   //Events and signals:
   add_events(Gdk::SCROLL_MASK |
              Gdk::BUTTON1_MOTION_MASK |
              Gdk::BUTTON2_MOTION_MASK |
              Gdk::BUTTON3_MOTION_MASK | 
              Gdk::BUTTON_PRESS_MASK   |   
              Gdk::BUTTON_RELEASE_MASK);

   //Zooming:
   signal_scroll_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_zoom));

   //Panning:
   sigpanstart = signal_button_press_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_pan_start));
   sigpan = signal_motion_notify_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_pan));
   sigpanend = signal_button_release_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_pan_end));

   //Profiling:
   sigprofstart = signal_button_press_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_prof_start));
   sigprof = signal_motion_notify_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_prof));
   sigprofend = signal_button_release_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_prof_end));

   //Not in profiling mode initially.
   sigprofstart.block();
   sigprof.block();
   sigprofend.block();

   //Fencing:
   sigfencestart = signal_button_press_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_fence_start));
   sigfence = signal_motion_notify_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_fence));
   sigfenceend = signal_button_release_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_fence_end));

   //Not in fencing mode initially.
   sigfencestart.block();
   sigfence.block();
   sigfenceend.block();

   //Rulering:
   sigrulerstart = signal_button_press_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_ruler_start));
   sigruler = signal_motion_notify_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_ruler));
   sigrulerend = signal_button_release_event().connect(sigc::mem_fun(this,&TwoDeeOverview::on_ruler_end));

   //Not in rulering mode initially.
   sigrulerstart.block();
   sigruler.block();
   sigrulerend.block();

   // Dispatchers (threading):
   signal_InitGLDraw.connect(sigc::mem_fun(this,&TwoDeeOverview::InitGLDraw));
   signal_DrawGLToCard.connect(sigc::mem_fun(this,&TwoDeeOverview::DrawGLToCard));
   signal_FlushGLToScreen.connect(sigc::mem_fun(this,&TwoDeeOverview::FlushGLToScreen));
   signal_EndGLDraw.connect(sigc::mem_fun(this,&TwoDeeOverview::EndGLDraw));
}

/*
==================================
 TwoDeeOverview::~TwoDeeOverview
==================================
*/
TwoDeeOverview::~TwoDeeOverview()
{
   delete[] vertices;
   delete[] colours;

	delete fencebox;
	delete profbox;
}

/*
==================================
 TwoDeeOverview::convert_to_latlong

 Given point coordinates in UTM converts them to latlong.
==================================
*/
void TwoDeeOverview::convert_to_latlong(double* point)
{
	GeoProjectionConverter gpc;
	char tmp[255];
	gpc.set_longlat_projection(tmp, false);
	gpc.set_utm_projection(const_cast<char*>(utm_zone.c_str()), tmp, true);
	gpc.to_target(point);
}

/*
==================================
 TwoDeeOverview::drawoverlays

 Prepare the appropriate overlays for flushing to the screen.
==================================
*/

void TwoDeeOverview::drawoverlays()
{
   //Draw the profile box if profile mode is on.
   if(profiling||showprofile)
      profbox->makebox(rmaxz);

   //Draw the fence box if fence mode is on.
   if(fencing||showfence)
      fencebox->makebox(rmaxz);

   //Draw the ruler if ruler mode is on.
   if(rulering)
      makerulerbox();

   //Draw the distance scale when indicated.
   if(showdistancescale)
      makedistancescale();

   //Draw the appropriate legend when indicated.
   if(showlegend)
      makecolourlegend();
}

/*
==================================
 TwoDeeOverview::InitGlDraw

 This handler prepares OpenGL for drawing the buckets, by "beginning" OpenGL
 and then clearing the buffers. It also then draws any profile or fencing
 boxes or the ruler.
==================================
*/
void TwoDeeOverview::InitGLDraw()
{
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))
      return;

   //Need to clear screen because of gaps.
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glwindow->gl_end();

   // The GL extension is now ready for use
   {
      Glib::Mutex::Lock lock (GL_action);

      GL_data_impede = false;
      GL_control_impede = false;

      GL_data_condition.signal();
      GL_control_condition.signal();
   }
}

/*
==================================
 TwoDeeOverview::DrawGLToCard

 This handler draws the contents of the arrays vertices and colours to the
 framebuffer. Note that the reason the OpenGL vertex array stuff is in here
 is because, apparently, the glDisableClientState() calls try to access the
 arrays, and this caused valgrind to squeal. It would make the program
 marginally faster to add a boolean variable "ending_GL_draw" so that
 EndGLDraw could end the vertex array enablement (and InitGLDraw could start
 it), but is it worth the global variable and the extra effort? Also, it is
 not certain, but it seems that since the vertex array stuff has moved here
 the missing bucket problem has gone, and it is also now very very difficult
 to make the thread cause a crash.
==================================
*/
void TwoDeeOverview::DrawGLToCard()
{
#ifdef THREAD_DEBUG
   cout << "DrawGLToCard called" << endl;
#endif

   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

   if (!glwindow->gl_begin(get_gl_context()))
      return;

   // This is extra important as the profile will be operating in a different 
   // thread from the drawing thread and so can grab OpenGL in between calls 
   // to this method.
   guard_against_interaction_between_GL_areas();

   // These relate to the enabling of vertex arrays and assigning the 
   // arrays to GL.
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, vertices);
   glColorPointer(3, GL_FLOAT, 0, colours);
   //Send the arrays to OpenGL for processing (finally!).
   glDrawArrays(GL_POINTS,0,pointcount);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glwindow->gl_end();

   // The GL data arrays may now be used again
   {
      Glib::Mutex::Lock lock (GL_action);

      GL_data_impede = false;
      GL_data_condition.signal();
   }
}

/*
==================================
 TwoDeeOverview::FlushGLToScreen

 This handler exists so that flushing of the framebuffer can be done
 independently of drawing to it. This allows an arbitrary frequency of
 flushes to be easily set, to compromise between showing the user things
 are happening and the desire to reduce the flicker and the fact that more
 flushes will make drawing slower.
==================================
*/
void TwoDeeOverview::FlushGLToScreen()
{
#ifdef THREAD_DEBUG
   cout << "FlushGLToScreen called" << endl;
#endif
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))
      return;

   //Draw to screen every (few) bucket(s) to show user stuff is happening.
   if (glwindow->is_double_buffered())
      glwindow->swap_buffers();
   else glFlush();
   glwindow->gl_end();

   // The GL extension may now be used again
   {
      Glib::Mutex::Lock lock (GL_action);

      GL_control_impede = false;
      GL_control_condition.signal();
   }
}

/*
==================================
 TwoDeeOverview::EndGLDraw

 This cleans up when a drawing thread is ending/has ended. It flushes once
 more so that everything is visible on the screen, draws the overlays, ends
 the OpenGL session and then records that new threads may be made.
==================================
*/
void TwoDeeOverview::EndGLDraw()
{
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))
      return;

   if (glwindow->is_double_buffered())
      glwindow->swap_buffers();
   else 
      glFlush();

   glReadBuffer(GL_BACK);

   //Overlays go on top and should not be preserved otherwise you get shadowing.
   glDrawBuffer(GL_FRONT);
   drawoverlays();
   glFlush();
   glDrawBuffer(GL_BACK);
   glwindow->gl_end();

   // The GL extension may now be used again
   {
      Glib::Mutex::Lock lock (GL_action);

      GL_control_impede = false;

      GL_control_condition.signal();
   }
}

/*
==================================
 TwoDeeOverview::mainimage

 This method draws the main image. Note that redundancy (from this method
 and drawpointsfrombuckets()) in telling the main thread what to do is
 because the main thread's execution of the signals it receives is not
 deterministic because of the possibility of user interference.

   Wait until all other drawing threads have ended.
   Tell any new threads that are created to wait until this one is finished
   if this one still exists.
   Tell the main thread not to create any more threads until the OpenGL has
   been initialised.
   Signal main thread to initialise OpenGL for drawing.
   Prepare for determining the boundary of the rectangle that just
   encompasses all the buckets being drawn so far.
   Draw the initially cached buckets.
   If this completes without interruption, draw the initially uncached
   buckets.
   End the thread.
==================================
*/
void TwoDeeOverview::mainimage(PointBucket** buckets,int numbuckets)
{
#ifdef THREAD_DEBUG
   cout << "mainimage called" << endl;
#endif

   // These are "safe" versions of the centre coordinates, as they will not 
   // change while this thread is running, while the originals might.
   centreSafe.move(centre.getX(), centre.getY(), 0);

   drawneverything = false;

   //Prepare OpenGL.

   if (awaitClearGLData(true))
      return;

   // grabbed one but not the other, make sure both are released
   if (awaitClearGLControl(true))
   {
      Glib::Mutex::Lock lock (GL_action);

      GL_data_impede = false;
      GL_data_condition.signal();

      return;
   }

   signal_InitGLDraw();

   Boundary* lidarboundary = lidardata->getBoundary();

   //Preparing to extract boundaries of drawn area.
   drawnsofarminx=lidarboundary->maxX;
   drawnsofarminy=lidarboundary->maxY;
   drawnsofarmaxx=lidarboundary->minX;
   drawnsofarmaxy=lidarboundary->minY;
   delete lidarboundary;

   // This records what buckets are drawn as a result of being initially cached 
   // which is then used to draw only the initially uncached ones.
   bool *drawnbucketsarray = new bool[numbuckets];
   for(int i = 0;i < numbuckets;i++)
	   drawnbucketsarray[i] = false;

   //Draw the points from the initially cached buckets.
   bool completed = drawpointsfrombuckets(buckets, numbuckets, drawnbucketsarray, true);

#ifdef THREAD_DEBUG
   if(!completed)
    	cout << "Pass of cached interrupted. Stopping!" << endl;
   else
#endif

   if (completed)
   {
#ifdef THREAD_DEBUG
    	cout << "Finished pass of cached." << endl;
#endif

      //Draw the points from the initially uncached buckets.
      completed = drawpointsfrombuckets(buckets, numbuckets, drawnbucketsarray, false);

#ifdef THREAD_DEBUG
      if(!completed)
         cout << "Pass of uncached interrupted. Stopping!" << endl;
      else
#endif
      if (completed)
      {
         if(numbuckets > 0)
            drawneverything = true;
#ifdef THREAD_DEBUG
        	cout << "Finished pass of uncached, thread completed." << endl;
#endif
      }
   }
   delete[]drawnbucketsarray;
   threadend(buckets);

   return;
}

/*
==================================
 TwoDeeOverview::drawpointsfrombuckets

 This draws either the initially cached or the initially uncached buckets to
 the screen. Which to draw is determined by the value of cachedonly.
   FOR every bucket:
      Determine resolutionindex
      IF cachedonly==true && bucket is cached for resolutionindex &&
         bucket is not yet drawn|| cachedonly==false &&
         bucket is not yet drawn:
         Wait until the main thread has finished drawing to the framebuffer.
         IF the thread is being interrupted:
            Delete data array.
            Signal main thread to clear up the OpenGL settings for drawing.
            Tell the main thread that this thread is no longer
           (significantly) running.
            Delete vertices and colours arrays.
            Set it so that subsequent threads will not be interrupted
            immediately.
            Allow subsequent threads to act.
            End thread (return).
         Determine the boundary of the rectangle that just encompasses all
         the buckets being drawn so far.
         FOR every point:
            determine colour and brightness of point
            IF any classifications are selected for increased prominence,
            set the z coordinate of the point to be higher than any
            other points.
            place point
         IF the thread is not being interrupted:
            Tell the main thread not to create any more threads until the
            current bucket has been drawn to the framebuffer.
            Signal the main thread to draw the current bucket to the
            framebuffer.
            Every Nth bucket (currently 10th):
               Tell main thread not to create any more threads until the
               framebuffer has been flushed to the screen.
               Signal the main thread to flush the framebuffer to the screen.
   Wait until the main thread has finished drawing to the framebuffer.
==================================
*/
bool TwoDeeOverview::drawpointsfrombuckets(PointBucket** buckets,int numbuckets,
                      bool *drawnbucketsarray, bool cachedonly)
{
   int line=0,intensity=0,classification=0,rnumber=0;
   double z=0;

   //Colour
   Colour tempColour;

   //For every bucket:
   for(int i=0;i<numbuckets;++i)
   {
      Glib::Mutex::Lock pbkt_lock (*global_pointbucket_mutex);

      double bucketscreenwidth = imageUnitsToPixels(buckets[i]->getmaxX() - buckets[i]->getminX());
      double bucketscreenheight = imageUnitsToPixels(buckets[i]->getmaxY() - buckets[i]->getminY());
      double bucketpixelcount = bucketscreenwidth*bucketscreenheight;
      int bucketpointtopixelratio = (int)((double)buckets[i]->getNumberOfPoints(0)/bucketpixelcount);

      // This determines what resolution level of the bucket to grab 
      // from the quadtree.
      int resolutionindex = 0;

      // If maindetailmod is 2 then the resolutionindex is determined to be the 
      // one that has resolutionbase^2 points per pixel on average, with the 
      // resolutionindex rounded up to be an integer. The user can change 
      // maindetailmod at will to modify the frequency of "holes" appearing 
      // in the visible data.
      for (int j = bucketpointtopixelratio; j > (int)pow(resolutionbase,maindetailmod) - 1; j /= resolutionbase)
         resolutionindex++;

      // Obviously cannot call an index that is less detailed than any 
      // that exist.
      if(resolutionindex > resolutiondepth - 1)
         resolutionindex = resolutiondepth - 1;

      // If cachedonly is true, then will proceed if the bucket has not been 
      // loaded before (which at this point should be all of them) and if the 
      // bucket is cached. If cachedonly is false, then will proceed if the 
      // bucket has not been loaded before without any extra condition because 
      // user interference could cause previously uncached buckets to become 
      // cached (like using pointinfo() or loading a preview in the middle 
      // of drawing).
      if(drawnbucketsarray[i] == false && (!cachedonly || buckets[i]->getIncacheList()[resolutionindex] == cachedonly))
      {
         drawnbucketsarray[i] = true;

         // Yield pointbucket lock
         // And also wait for GL data to be safe to modify
         pbkt_lock.release();

         // Do not pass go, do not collect 200 dollars. The parent method will 
         // handle the fallout, just STOP!
         if(awaitClearGLData(true))
         {
#ifdef THREAD_DEBUG
            cout << "Draw interrupted" << endl;
#endif
            return false;
         }
         else
            pbkt_lock.acquire();

         // This is needed for putting values in the right indices for the 
         // vertices and colours arrays and for drawing them properly with 
         // OpenGL.
         pointcount=0;

         //Set the boundary of the buckets selected so far.
         if(buckets[i]->getminX()<drawnsofarminx)
            drawnsofarminx = buckets[i]->getminX();
         if(buckets[i]->getminY()<drawnsofarminy)
            drawnsofarminy = buckets[i]->getminY();
         if(buckets[i]->getmaxX()>drawnsofarmaxx)
            drawnsofarmaxx = buckets[i]->getmaxX();
         if(buckets[i]->getmaxY()>drawnsofarmaxy)
            drawnsofarmaxy = buckets[i]->getmaxY();

         //For every point, determine point colour and position:
         for(int j=0;j<buckets[i]->getNumberOfPoints(resolutionindex);++j)
         {
            if (tdoDisplayNoise || (!tdoDisplayNoise && buckets[i]->getPoint(j,resolutionindex).getClassification() != 7))
            {
               //This is here because it is used in calculations.
               z = buckets[i]->getPoint(j,resolutionindex).getZ();

               if (slicing && (z < slice_minz || z > slice_maxz))
               {
            	   continue;
               }

               //This is here because it is used in calculations.
               intensity = buckets[i]->getPoint(j,resolutionindex).getIntensity();

               // Select colour depending on colourBy value
               switch (colourBy)
               {
                  case colourByHeight:
                     tempColour = getColourByHeight(z); 
                     break;
                  case colourByIntensity:
                     tempColour = getColourByIntensity(intensity); 
                     break;
                  case colourByFlightline:
                     line = buckets[i]->getPoint(j,resolutionindex).getFlightline();
                     tempColour = getColourByFlightline(line);
                     break;
                  case colourByClassification:
                     classification = buckets[i]->
                                      getPoint(j,resolutionindex).getClassification();
                     tempColour = getColourByClassification(classification);
                     break;
                  case colourByReturn:
                     rnumber = buckets[i]->
                               getPoint(j,resolutionindex).getReturn();
                     tempColour = getColourByReturn(rnumber);
                     break;
                  // ColourByNone
                  default:
                     tempColour.setRGB(0, 1, 0);
                     break;
               }

               // Select brightness depending on brightness setting
               switch (brightnessBy)
               {
                  case brightnessByIntensity:
                     tempColour.multiply(brightnessintensityarray[(int)(intensity-rminintensity)]);
                     break;
                  case brightnessByHeight:
                     tempColour.multiply(brightnessheightarray[int(10*(z-rminz))]);
                     break;
                  // brightnessByNone
                  default:
                     break;
               }
                 
               vertices[3*pointcount]=buckets[i]->getPoint(j,resolutionindex).getX()-centreSafe.getX();
               vertices[3*pointcount+1]=buckets[i]->getPoint(j,resolutionindex).getY()-centreSafe.getY();
               if(heightenNonC ||
                  heightenGround ||
                  heightenLowVeg ||
                  heightenMedVeg ||
                  heightenHighVeg ||
                  heightenBuildings ||
                  heightenNoise ||
                  heightenMass ||
                  heightenWater ||
                  heightenOverlap ||
                  heightenUndefined)
               {
                  classification = buckets[i]->getPoint(j,resolutionindex).getClassification();
                  double incrementor = 100+abs(rmaxz-rminz);
                  switch(classification){
                     //Heighten non-classified.
                     case 0:case 1:if(heightenNonC)z+=incrementor;break;
                     //Heighten the ground.
                     case 2:if(heightenGround)z+=incrementor;break;
                     //Heighten low vegetation.
                     case 3:if(heightenLowVeg)z+=incrementor;break;
                     //Heighten medium vegetation.
                     case 4:if(heightenMedVeg)z+=incrementor;break;
                     //Heighten hig vegetation.
                     case 5:if(heightenHighVeg)z+=incrementor;break;
                     //Heighten buildings.
                     case 6:if(heightenBuildings)z+=incrementor;break;
                     //Heighten noise.
                     case 7:if(heightenNoise)z+=incrementor;break;
                     //Heighten mass points.
                     case 8:if(heightenMass)z+=incrementor;break;
                     //Heighten water.
                     case 9:if(heightenWater)z+=incrementor;break;
                     //Heighten overlaps.
                     case 12:if(heightenOverlap)z+=incrementor;break;
                     //Heighten anything else.
                     default:if(heightenUndefined)z+=incrementor;break;
                  }
                  // This is to prevent the points ever obscuring the overlays. 
                  // Note that this can handle well anything up to a height of 
                  // 90 000 metres (including the increase from above, but it 
                  // should still be able to handle the Himalayas); above that 
                  // and the points will be drawn at the same height.
                  if(z>rmaxz+900)
                  {
                	 z = rmaxz+900+z/1000;
                     if(z>rmaxz+990)z=rmaxz+990;
                  }
               }
               if(raiseline)
                  if(linetoraise == buckets[i]->getPoint(j,resolutionindex).getFlightline())
                  {
                  z += 100+abs(rmaxz-rminz);

                  // This is to prevent the points ever obscuring the overlays. 
                  // Note that this can handle well anything up to a height of 
                  // 90 000 metres (including the increase from above, but it 
                  // should still be able to handle the Himalayas); above that 
                  // and the points will be drawn at the same height.
                  if(z>rmaxz+900)
                  {
                     z = rmaxz+900+z/1000;
                     if(z>rmaxz+990)z=rmaxz+990;
                  }
               }
               //If all is normal, the height is z.
               if(!reversez)
                  vertices[3*pointcount+2]=z;
               else 
                  // If the z values are to be reversed, the height is made so 
                  // that, within the range they all occupy, the values are 
                  // reversed.
                  vertices[3*pointcount+2]= rmaxz + rminz - z;

               colours[3*pointcount]=tempColour.getR();		//red;
               colours[3*pointcount+1]=tempColour.getG();	//green;
               colours[3*pointcount+2]=tempColour.getB();	//blue;
               pointcount++;
            }
         }
         pbkt_lock.release();

#ifdef DEBUG_THREAD
         cout << pointcount << endl;
         cout << vertices[3*pointcount/2] << endl;
#endif

#ifdef DEBUG_THREAD
            cout << "Sending draw signal." << endl;
#endif

         if (interrupt_drawing)
         {
            Glib::Mutex::Lock lock (GL_action);

            // release access to GL data
            GL_data_impede = false;
            GL_data_condition.signal();

            return false;
         }
         
         else
         {
            signal_DrawGLToCard();

            // Main thread must not attempt to create a new thread like this 
            // while flushing has yet to occur.
            if(i >= (numbuckets-1) || numbuckets > 10)
               if( (i + 1) % 10  == 0)
               {
#ifdef DEBUG_THREAD
                  cout << "Sending flush signal." << endl;
#endif
                  if (awaitClearGLControl(true))
                     return false;
                  signal_FlushGLToScreen();
               }
         }

#ifdef DEBUG_THREAD
         else
         {
            cout << "Draw interrupted." << endl;
         }
#endif
      }
   }
#ifdef DEBUG_THREAD
   cout << "Checking for drawing to make sure there is no \
                           deadlock. Might wait." << endl;
#endif

   // Asserts that GL is finished sending data
   //if (awaitClearGLData(false))
   //   return false;

   return true;
}

/*
==================================
 TwoDeeOverview::threadend

 This clears up after and ends a drawing thread. It works like this:
 Tell the main thread that this thread is no longer (significantly)
 running.
 Delete data array.
 Signal main thread to clear up the OpenGL settings for drawing.
 Delete vertices and colours arrays.
 Set it so that subsequent threads will not be interrupted immediately.
 Allow subsequent threads to act.
==================================
*/
void TwoDeeOverview::threadend(PointBucket** buckets)
{
   //This is up here so that buckets is deleted before it is newed again.
   delete[] buckets;

   // For the sake of neatness, clear up. This comes before allowing the main 
   // thread to create another thread like this to ensure that this signal is 
   // processed before, say, a signal to prepare OpenGL for drawing again.
   if (awaitClearGLControl(true))
      return;
   signal_EndGLDraw();

#ifdef THREAD_DEBUG
   cout << "Drawing thread cycle ending" << endl;
#endif
}

/*
==================================
 TwoDeeOverview::drawbuckets

 This method draws a preview version of the image for any situations where
 it must be drawn quickly. It does this by first electing to draw directly
 to the front buffer and to flush it, rather than using double buffering
 and the swap_buffers() command. It then clears the front buffer using
 glClear() and then builds the profile box, the ruler or the fence box in
 the event that one of them is active. The method then copies from the back
 buffer to the front buffer a region of pixels that corresponds with a
 rectangle that just covers all of the buckets drawn before. This way, if
 the entire image is loaded then the user sees it all moving, perfectly. If
 some of the image is "off the edge of the screen" then when it moves the
 uncovered areas will show the "skeleton" of the buckets. The user will also
 see the "skeleton" of the buckets if they elect to do something that will
 cause a preview to be drawn before the main image is complete, as only the
 complete portions will be drawn. The drawing buffer is then set back to the
 back. The method is ordered so that the top-most things are drawn first.
 This is because it is thought that having previously-drawn things obscure
 latterly-drawn things will reduce flicker.
==================================
*/
bool TwoDeeOverview::drawbuckets(PointBucket** buckets,int numbuckets)
{
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))
      return false;
   
   //We want to copy from here.
   glReadBuffer(GL_BACK);
   //We want to draw to here.
   glDrawBuffer(GL_FRONT);
   //Need to clear screen because of gaps.
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   drawoverlays();

   // This makes sure the preview grid and the framebuffer are drawn under the 
   // profile, ruler etc..
   double altitude = rminz-1000;

   // Copy pixels from back buffer
   // The position of the bottom left corner of where the region is to be copied 
   // TO. In world coordinates.
   double xpos = drawnsofarminx-centre.getX();
   double ypos = drawnsofarminy-centre.getY();

   // These offsets are used for when the position of the bottom left corner of 
   // the destination region would go off the screen to the left or bottom 
   // (which would cause NOTHING to be drawn). In pixels.
   double xoffset = 0;
   double yoffset = 0;
   GLint viewport[4];
   GLdouble modelview[16];
   GLdouble projection[16];
   glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
   glGetDoublev(GL_PROJECTION_MATRIX,projection);
   glGetIntegerv(GL_VIEWPORT,viewport);

   //The world coordinates of the origin for the screen coordinates.
   GLdouble origx,origy,origz;
   gluUnProject(0,0,0,modelview,projection,viewport,&origx,&origy,&origz);

   if(xpos < origx)
   {
      // Converts the difference between the 'old' xpos and the edge of the 
      // screen into pixel values for modification of the region copied from.
      xoffset = imageUnitsToPixels(origx-xpos);
      xpos = origx;
   }
   if(ypos < origy)
   {
      // Converts the difference between the 'old' ypos and the edge of the 
      // screen into pixel values for modification of the region copied from.
      yoffset = imageUnitsToPixels(origy-ypos);
      ypos = origy;
   }
   // Finally set the position of the bottom left corner of the destination 
   // region. This takes world coordinates and converts them into pixels. 
   // glWindowPos does the same thing directly with pixels, but it requires 
   // OpenGL 1.4.
   glRasterPos3f(xpos,ypos,altitude+1);

   // These define the boundaries of the region to be copied FROM.
   // ...The offsets are here so that if the destination position should be 
   // too far left or down then these account for it. Otherwise the skeleton 
   // and copy part ways.
   double bucketminx = imageUnitsToPixels(drawnsofarminx-centreSafe.getX()) + 
                       (double)(get_width())/2 + xoffset;
   double bucketminy = imageUnitsToPixels(drawnsofarminy-centreSafe.getY()) + 
                       (double)(get_height())/2 + yoffset;
   double bucketmaxx = imageUnitsToPixels(drawnsofarmaxx-centreSafe.getX()) + 
                       (double)(get_width())/2;
   double bucketmaxy = imageUnitsToPixels(drawnsofarmaxy-centreSafe.getY()) + 
                       (double)(get_height())/2;

   // I think that having ANYTHING going beyond the boundaries of the screen 
   // will cause NOTHING to be drawn. Apparently bottom left corner does not 
   // matter here, though, only the top right (???).
   if(bucketmaxx > get_width())
      bucketmaxx = get_width();
   if(bucketmaxy > get_height())
      bucketmaxy = get_height();

   // ...Oh, and the bottom left corner must be further left and down than 
   // the top right corner.
   if(bucketminx > bucketmaxx)
      bucketminx = bucketmaxx;
   if(bucketminy > bucketmaxy)
      bucketminy = bucketmaxy;

   // The business end, at last. Copies from the region defined to the current 
   // raster position.
   glCopyPixels(bucketminx,bucketminy,bucketmaxx-bucketminx, bucketmaxy-bucketminy,GL_COLOR);

   //The world coordinates of the origin for the screen coordinates.
   GLdouble endx,endy,endz;
   gluUnProject(get_width(),get_height(),0, modelview,projection,viewport, &endx,&endy,&endz);

   // If not all the buckets have been drawn OR the boundaries of the drawn
   // region extend beyond the screen AND the image has been moved in the 
   // opposite direction to that extension. i.e. if part of the image 
   // is/should be uncovered.
   if(!drawneverything ||
      (((drawnsofarmaxx-centreSafe.getX() > endx && centre.getX()-centreSafe.getX()>0) ||
      (drawnsofarminx-centreSafe.getX() < origx && centre.getX()-centreSafe.getX()<0) ||
      (drawnsofarmaxy-centreSafe.getY() > endy && centre.getY()-centreSafe.getY()>0) ||
      (drawnsofarminy-centreSafe.getY() < origy && centre.getY()-centreSafe.getY()<0))))
   {
      //Needed for the glDrawArrays() call further down.
      float* vertices = new float[12];
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, vertices);
      glColor3f(1.0,1.0,1.0);

      //For every bucket...
      for(int i=0;i<numbuckets;i++){
         vertices[0]=buckets[i]->getminX()-centre.getX();
         vertices[1]=buckets[i]->getminY()-centre.getY();
         vertices[2]=altitude;
         vertices[3]=buckets[i]->getminX()-centre.getX();
         vertices[4]=buckets[i]->getmaxY()-centre.getY();
         vertices[5]=altitude;
         vertices[6]=buckets[i]->getmaxX()-centre.getX();
         vertices[7]=buckets[i]->getmaxY()-centre.getY();
         vertices[8]=altitude;
         vertices[9]=buckets[i]->getmaxX()-centre.getX();
         vertices[10]=buckets[i]->getminY()-centre.getY();
         vertices[11]=altitude;
         glDrawArrays(GL_LINE_LOOP,0,4);
      }
      glDisableClientState(GL_VERTEX_ARRAY);
      delete[] vertices;
   }
   //After all this effort, something must be drawn to the screen.
   glFlush();

   //Reset the raster position.
   glRasterPos2s(0,0);
   glDrawBuffer(GL_BACK);
   glwindow->gl_end();

   return true;
}

/*
==================================
 TwoDeeOverview::drawviewable

 Gets the limits of the viewable area and passes them to the subsetting
 method of the quadtree to get the relevant data. It then converts from
 a vector to a pointer array to make data extraction faster. Then,
 depending on the imagetype requested, it either sets the detail level
 and then creates a thread for drawing the main image (imagetype==1) or
 calls drawbuckets in order to give a preview of the data when panning etc.
 (imagetype==2). When this is called from the expose event (imagetype==3)
 it draws the main image (drawing speed is not so urgent now that it is
 threaded).
==================================
*/
bool TwoDeeOverview::drawviewable(int imagetype)
{
   guard_against_interaction_between_GL_areas();

   if (lidardata == NULL)
	   return false;

   if (drawing_thread == NULL)
   {
      drawing_thread = new DrawWorker(this);

      drawing_thread->start();
   }

   // If there is an expose event while the image is already being drawn from 
   // scratch and the image has been drawn from scratch at least once then 
   // refresh the screen without interfering with the drawing from scratch:
   if(drawing_thread->isDrawing() && imagetype == 3 && drawnsinceload)
   {
      Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
      if (!glwindow->gl_begin(get_gl_context()))
         return false;
      if (glwindow->is_double_buffered())
         glwindow->swap_buffers();
      else 
         glFlush();
      glwindow->gl_end();
      return true;
   }

   // Draw the main image.
   if(imagetype==1 || (!drawnsinceload && imagetype == 3))
   {
#ifdef THREAD_DEBUG
      cout << "Am fluid" << endl;
      cout << "Changed offsets." << endl;
#endif

      // Expose events draw the image from scratch at least once so that the 
      // image will be shown immediately after loading a file for the first 
      // time.

      if(imagetype == 3)
    	  drawnsinceload = true;

      //Limits of viewable area:
      double minx = centre.getX()-pixelsToImageUnits(get_width()/2);
      double maxx = centre.getX()+pixelsToImageUnits(get_width()/2);
      double miny = centre.getY()+pixelsToImageUnits(get_height()/2);
      double maxy = centre.getY()-pixelsToImageUnits(get_height()/2);

      vector<double> xs(4);
      xs[0] = minx;
      xs[1] = minx;
      xs[2] = maxx;
      xs[3] = maxx;
      vector<double> ys(4);
      ys[0] = miny;
      ys[1] = maxy;
      ys[2] = maxy;
      ys[3] = miny;

      vector<PointBucket*> *pointvector = NULL;

      //Get data.
      bool gotdata = advsubsetproc(pointvector,xs,ys,4);

      if(!gotdata)
      {
         if(pointvector==NULL)
         {
            drawneverything = false;
            drawnsofarminx=0;
            drawnsofarminy=0;
            drawnsofarmaxx=1;
            drawnsofarmaxy=1;
            return clearscreen();
         }
         else return drawviewable(2);
      }

      // Earliest time in the function call that it is definitely known
      // drawing_thread->draw will occur, so abort any currently drawing frames
      if (drawing_thread->isDrawing())
         abortFrame(true);

      numbuckets = pointvector->size();

      //This is not deleted here but in the drawing thread.
      PointBucket** buckets = new PointBucket*[numbuckets];

      // Convert to pointer for faster access in for loops in image methods. 
      // Why? Expect >100000 points. ........Probably will not make any 
      // difference BUT it does mean that the data can be accessed without 
      // doing (*pointvector)[i] every time, as doing pointvector->at(i) may 
      // be slower due to checks.
      for(int i=0;i<numbuckets;i++)
      {
         buckets[i]=(*pointvector)[i];
      }

      // This thread will interpret the data before telling the main 
      // thread to draw.
      drawing_thread->draw(buckets, numbuckets);

      delete pointvector;
   }
   //Draw the preview.
   else if(imagetype==2||(imagetype==3 && !drawing_thread->isDrawing()
         && drawnsinceload))
   {
      //Limits of viewable area:
      double minx = centre.getX()-pixelsToImageUnits(get_width()/2);
      double maxx = centre.getX()+pixelsToImageUnits(get_width()/2);
      double miny = centre.getY()+pixelsToImageUnits(get_height()/2);
      double maxy = centre.getY()-pixelsToImageUnits(get_height()/2);
      vector<double> xs(4);
      xs[0] = minx;
      xs[1] = minx;
      xs[2] = maxx;
      xs[3] = maxx;
      vector<double> ys(4);
      ys[0] = miny;
      ys[1] = maxy;
      ys[2] = maxy;
      ys[3] = miny;
      vector<PointBucket*> *pointvector = NULL;

      //Get data.
      bool gotdata = advsubsetproc(pointvector,xs,ys,4);

      if(!gotdata)
      {
         if(pointvector==NULL)return clearscreen();
      }

      int numbuckets = pointvector->size();
      PointBucket** buckets = new PointBucket*[numbuckets];

      // Convert to pointer for faster access in for loops in image methods. 
      // Why? Expect >100000 points.
      for(int i=0;i<numbuckets;i++)
      {
         buckets[i]=(*pointvector)[i];
      }
      drawbuckets(buckets,numbuckets);
      delete pointvector;
      delete[] buckets;
   }
   glReadBuffer(GL_BACK);
   //Overlays go on top and should not be preserved otherwise you get shadowing.
   glDrawBuffer(GL_FRONT);
   drawoverlays();
   glFlush();
   glDrawBuffer(GL_BACK);

   return true;
}

/*
==================================
 TwoDeeOverview::returntostart

 Return to initial viewing position.
==================================
*/
bool TwoDeeOverview::returntostart()
{
   Boundary* lidarboundary = lidardata->getBoundary();
   double xdif = lidarboundary->maxX-lidarboundary->minX;
   double ydif = lidarboundary->maxY-lidarboundary->minY;

   // This ratio defines, along with zoomlevel, the translation from world 
   // coordinates to window coordinates.
   double xratio = xdif/get_parent()->get_width();
   double yratio = ydif/get_parent()->get_height();
   if(xratio>yratio)ratio = xratio;
   else ratio = yratio;

   // The image should appear slightly smaller so that its edges do not touch 
   // the edge of the drawing area.
   ratio *= 1.1;
   profbox->setratio(ratio);
   fencebox->setratio(ratio);
   //Image should be centred at its centre.
   centre.move(lidarboundary->minX+xdif/2, lidarboundary->minY+ydif/2, 0);

   // Back to the starting zoom, which should cause the entire image to be 
   // visible.
   zoomlevel=1;

   //Update matrices.
   resetview();
   set_overlay_zoomlevels(zoomlevel);
   set_overlay_centres(centre);
   delete lidarboundary;
   return drawviewable(1);
}

/*
==================================
 TwoDeeOverview::resetview

 This determines what part of the image is displayed with orthographic
 projection. It sets the active matrix to that of projection and makes
 it the identity matrix, and then defines the limits of the viewing area
 from the dimensions of the window. *ratio/zoomlevel is there to convert
 window coordinates to world coordinates.
==================================
*/
void TwoDeeOverview::resetview()
{
   //Extra space to work with is good.
   double altitude = rmaxz+5000;
   double depth = rminz-5000;
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-pixelsToImageUnits(get_width()/2),
           +pixelsToImageUnits(get_width()/2),
           -pixelsToImageUnits(get_height()/2),
           +pixelsToImageUnits(get_height()/2),
           -5*altitude,-5*depth);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

/*
==================================
 TwoDeeOverview::pointinfo

 This method causes the label below the toolbar to display information about
 a point that has been selected (using the right mouse button). It starts by
 setting the label to 0 0 0. It then determines the world coordinates
 selected from the window coordinates and calls a subset for that.
 However, assuming that any points are returned at all, an entire bucket's
 worth of points will be returned. These points are then narrowed down to one
 point firstly by calling the vetpoints() function to only get the points
 that lie in or overlap the square defined by the point's position and the
 pointsize and zoom position (i.e. the size of the point on the screen).
 After that, all remaining points are compared and the one on top, the one
 with the largest z, which would likely be the one the user sees or sees
 mostly, is selected as THE point. Data about the point is extracted from
 it, with a referral to the quadtree to get the filename. The information
 displayed is the following: X, Y and Z values; the time; the intensity;
 the classification; the filename of the file the point is from and the
 return number.
==================================
*/
bool TwoDeeOverview::pointinfo(double eventx,double eventy)
{
   string meh = "0\n0\n0";

   //This offset exists because, in world coordinates, 0 is the centre.
   double pointeroffx = eventx - get_width()/2;

   //...and the same for this one.
   double pointeroffy = eventy - get_height()/2;

   //Define an area of equal size to that of the points on the screen.
   double minx = centre.getX() + pixelsToImageUnits(pointeroffx - pointsize/2);
   double maxx = centre.getX() + pixelsToImageUnits(pointeroffx + pointsize/2);
   double miny = centre.getY() - pixelsToImageUnits(pointeroffy + pointsize/2);
   double maxy = centre.getY() - pixelsToImageUnits(pointeroffy - pointsize/2);

   vector<double> xs(4);
   xs[0] = minx;
   xs[1] = minx;
   xs[2] = maxx;
   xs[3] = maxx;
   vector<double> ys(4);
   ys[0] = miny;
   ys[1] = maxy;
   ys[2] = maxy;
   ys[3] = miny;

   vector<PointBucket*> *pointvector = NULL;

   //Get data.
   bool gotdata = advsubsetproc(pointvector,xs,ys,4);

   //If there aren't any points, don't bother.
   if(gotdata)
   {
      // Determines whether there are any points that the user could reasonably 
      // have meant to select.
      bool anypoint = false;
      int bucketno=0;
      int pointno=0;
      //Wants exclusive access to pointbucket::getpoint().
      Glib::Mutex::Lock pbkt_lock (*global_pointbucket_mutex);

      // For every bucket, in case of the uncommon instances where more than 
      // one bucket is returned. 
      for(unsigned int i=0;i<pointvector->size();i++)
      {
         // This returns an array of booleans saying whether or not each point 
         // (indicated by indices that are shared with pointvector) is in the 
         // area prescribed.
         bool* pointsinarea = vetpoints((*pointvector)[i],xs,ys,4, false);
         // For all points (no sorting as it seems pointless with a maximum of 
         // four buckets possible)...
         for(int j=0;j<(*pointvector)[i]->getNumberOfPoints(0);j++)
         {
            //If they are in the right area...
            if(pointsinarea[j])
            {
               if(!anypoint)
               {
                  bucketno=i;
                  pointno=j;
                  anypoint = true;
               }
               //...and if they are higher than the currently selected point 
               // assuming the z values are not being reversed.
               if(!reversez && (*pointvector)[i]->getPoint(j,0).getZ() >= 
                  (*pointvector)[bucketno]->getPoint(pointno,0).getZ())
               {
                  //Select them.
                  bucketno=i;
                  pointno=j;
               }
               //...or, alternatively, if they are lower than the currently 
               // selected point assuming the z values ARE being reversed.
               else if(reversez && (*pointvector)[i]->getPoint(j,0).getZ() <= (*pointvector)[bucketno]->getPoint(pointno,0).getZ())
               {
                  //Select them.
                  bucketno=i;
                  pointno=j;
               }
            }
         }
         delete pointsinarea;
      }
      if(anypoint)
      {
         // NOTE: This functionality has been retired for the time being, as it
         // seemed fairly pointless and used the old boolean race-condition
         // solving system. In future reimplementations, consider expanding
         // the selection from 2x2 pixels, as the 2x2 pixels are barely visible
         // (which warranted retiring this functionality in the first place)

         //if(drawing_to_GL ||
         //   initialising_GL_draw ||
         //   flushing ||
         //   thread_existsthread ||
         //   thread_existsmain);
         //else
         //{
         //   drawviewable(2);
         //   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
         //   if (!glwindow->gl_begin(get_gl_context()))
         //      return false;

         //   // This makes sure the highlight is drawn over the top of the 
         //   // flightlines.
         //   double altitude = rmaxz+1000;
         //   glReadBuffer(GL_BACK);
         //   glDrawBuffer(GL_FRONT);
         //   glColor3f(1.0,1.0,1.0);
         //   glBegin(GL_LINE_LOOP);
         //      glVertex3d((*pointvector)[bucketno]->
         //                  getPoint(pointno,0).getX()-centre.getX() -
         //                  0.5*pixelsToImageUnits(pointsize),
         //                  (*pointvector)[bucketno]->
         //                  getPoint(pointno,0).getY()-centre.getY() -
         //                  0.5*pixelsToImageUnits(pointsize),
         //                  altitude);
         //      glVertex3d((*pointvector)[bucketno]->
         //                  getPoint(pointno,0).getX()-centre.getX() -
         //                  0.5*pixelsToImageUnits(pointsize),
         //                  (*pointvector)[bucketno]->
         //                  getPoint(pointno,0).getY()-centre.getY() +
         //                  0.5*pixelsToImageUnits(pointsize),
         //                  altitude);
         //      glVertex3d((*pointvector)[bucketno]->
         //                  getPoint(pointno,0).getX()-centre.getX() +
         //                  0.5*pixelsToImageUnits(pointsize),
         //                  (*pointvector)[bucketno]->
         //                  getPoint(pointno,0).getY()-centre.getY() +
         //                  0.5*pixelsToImageUnits(pointsize),
         //                  altitude);
         //      glVertex3d((*pointvector)[bucketno]->
         //                  getPoint(pointno,0).getX()-centre.getX() +
         //                  0.5*pixelsToImageUnits(pointsize),
         //                  (*pointvector)[bucketno]->
         //                  getPoint(pointno,0).getY()-centre.getY() - 
         //                  0.5*pixelsToImageUnits(pointsize),
         //                  altitude);
         //   glEnd();
         //   glDrawBuffer(GL_BACK);
         //   glFlush();
         //   glwindow->gl_end();
         //}

         //Returns the filepath.
         boost::filesystem::path flightline(lidardata->
            getFileName((*pointvector)[bucketno]->
            getPoint(pointno,0).getFlightline()));

         flightline = flightline.filename();

         ostringstream x,y,z,time,intensity,classification,
                       rnumber,flightlinenumber;
         x    << std::setprecision(12);
         y    << std::setprecision(12);
         z    << std::setprecision(12);
         time << std::setprecision(12);


         if (!latlong)
         {
            x << (*pointvector)[bucketno]->getPoint(pointno,0).getX();
            y << (*pointvector)[bucketno]->getPoint(pointno,0).getY();
            z << (*pointvector)[bucketno]->getPoint(pointno,0).getZ();
         }
         else
         {
            double point[3];
            point[0] = (*pointvector)[bucketno]->getPoint(pointno,0).getX();
            point[1] = (*pointvector)[bucketno]->getPoint(pointno,0).getY();
            point[2] = (*pointvector)[bucketno]->getPoint(pointno,0).getZ();
            convert_to_latlong(point);

            x << point[0];
            y << point[1];
            z << point[2];
         }

         time << (*pointvector)[bucketno]->getPoint(pointno,0).getTime();
         intensity << (*pointvector)[bucketno]->getPoint(pointno,0).getIntensity();
         classification << (int)(*pointvector)[bucketno]->getPoint(pointno,0).getClassification();
         rnumber << (int)((*pointvector)[bucketno]->getPoint(pointno,0).getReturn());
         flightlinenumber << (int)((*pointvector)[bucketno]->getPoint(pointno,0).getFlightline());

         string pointstring = "X: " + x.str() + 
                            ", Y: " + y.str() + 
                            ", Z: " + z.str() + 
                         ", Time: " + time.str() + 
              ",\n" + "Intensity: " + intensity.str() + 
               ", Classification: " + classification.str() + 
             ",\n" + "Flightline: " + flightline.string() + 
                               " (" + flightlinenumber.str() + 
               "), Return number: " + rnumber.str() + ".";

         rulerlabel->set_text(pointstring);
      }
      else
      {
         rulerlabel->set_text(meh);
         //if(drawing_to_GL ||
         //   initialising_GL_draw ||
         //   flushing ||
         //   thread_existsthread ||
         //   thread_existsmain);
         //else drawviewable(2);
      }
   }
   else
   {
      rulerlabel->set_text(meh);
      //if(drawing_to_GL ||
      //   initialising_GL_draw ||
      //   flushing ||
      //   thread_existsthread ||
      //   thread_existsmain);
      //else drawviewable(2);
   }
   delete pointvector;

   // This causes the event box containing the overview to grab the focus, 
   // and so to allow keyboard control of the overview (this is not done 
   // directly as that wuld cause expose events to be called when focus 
   // changes, resulting in graphical glitches).
   get_parent()->grab_focus();
   return true;
}

/*
==================================
 TwoDeeOverview::makecolourlegend

 This draws a legend on the screen that explains what the colours represent.
 It draws different legends depending on the colouring mode except that it
 draws nothing when the colouring mode is by none or by flightline (the
 latter because flightline numbering is arbitrary, discrete and of
 potentially unlimited number).
==================================
*/
void TwoDeeOverview::makecolourlegend()
{
   //The height of the area in world coordinates.
   double rheight = pixelsToImageUnits(get_height());
   double padding = 0.05*rheight;

   //This makes sure the scale is drawn on top of the flightlines.
   double altitude = rmaxz+1000;

   //This is the gap between the legend and the right edge of the screen.
   double hoffset = 10;

   //This is the width of the coloured "key" part.
   double hwidth = 20;

   // This is the distance between the end of the text and the beginning 
   // of the key.
   double hgap = 5;
   GLint viewport[4];
   GLdouble modelview[16];
   GLdouble projection[16];

   //The world coordinates of the top right corner of the window.
   GLdouble cornx,corny,cornz;
   glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
   glGetDoublev(GL_PROJECTION_MATRIX,projection);
   glGetIntegerv(GL_VIEWPORT,viewport);

   //Get the world coordinates of the top-right corner.
   gluUnProject(get_width(), get_height() ,0 ,
                modelview, projection, viewport,
                &cornx, &corny, &cornz);

   //White text and lines.
   glColor3d(1.0,1.0,1.0);
   double stringwidth = 0;
   double cbmax, cbmin, length;
   Colour colour;
   string text;
   char number[30];
   switch (colourBy)
   {
      case colourByHeight:
         length = 0.9*rheight/6;
         //Getting the maxima and minima as far as the colouring is concerned.
         cbmax = cbmaxz;
         cbmin = cbminz;
         for(int i=0;i<7;++i)
         {
            sprintf(number, "%.2lf", ((6-i)*cbmax + i*cbmin)/6);
            stringwidth = FONT_CHAR_WIDTH * strlen(number); 
            printString(cornx - pixelsToImageUnits(hoffset+hwidth+stringwidth+hgap),
                        corny - padding - pixelsToImageUnits(0.5*FONT_CHAR_HEIGHT)- length*i, altitude);
         }
         // Draws a strip of quads with smooth colour transitions to give a 
         // spectrum-like effect (though the colours are in a different order 
         // from the real spectrum).
         glBegin(GL_QUAD_STRIP);
            for(int i=0;i<7;++i)
            {
               Colour colour;
               colour_by(((6-i)*cbmax + i*cbmin)/6,cbmax,cbmin,colour);
               glColor3fv(colour.getRGB());
               glVertex3d(cornx-pixelsToImageUnits(hoffset+hwidth),
                          corny-padding-i*length,
                          altitude);
               glVertex3d(cornx-pixelsToImageUnits(hoffset),
                          corny-padding-i*length,
                          altitude);
            }
         glEnd();
         break;

      case colourByIntensity:
         length = 0.9*rheight/6;
         
         for(int i=0;i<10;++i)
         {
            sprintf(number, "%.1lf", (double((6-i)*cbmaxintensity + i*cbminintensity))/6);
            stringwidth = FONT_CHAR_WIDTH * strlen(number); 
            glRasterPos3d(cornx - pixelsToImageUnits(hoffset + hwidth + stringwidth + hgap),
                          corny - padding - pixelsToImageUnits(0.5* FONT_CHAR_HEIGHT) - length*i, altitude);
         }
         // Draws a strip of quads with smooth colour transitions to give a 
         // spectrum-like effect (though the colours are in a different order 
         // from the real spectrum).
         glBegin(GL_QUAD_STRIP);
               glColor3d(1, 1, 1);
               glVertex3d(cornx-pixelsToImageUnits(hoffset+hwidth),
                          corny-padding-0*length,
                          altitude);
               glVertex3d(cornx-pixelsToImageUnits(hoffset),
                          corny-padding-0*length,
                          altitude);
               glColor3d(0, 0, 0);
               glVertex3d(cornx-pixelsToImageUnits(hoffset+hwidth),
                          corny-padding-6*length,
                          altitude);
               glVertex3d(cornx-pixelsToImageUnits(hoffset),
                          corny-padding-6*length,
                          altitude);
         glEnd();
         break;

      case colourByClassification:
         length = 0.9*rheight/10;
         for(int i=0;i<11;++i)
         {
            colour = getColourByClassification(i);
            switch(i)
            {
               case 0:  text = "Non-classified";break;
               case 1:  text = "Ground";break;
               case 2:  text = "Low vegetation";break;
               case 3:  text = "Medium vegetation";break;
               case 4:  text = "High vegetation";break;
               case 5:  text = "Buildings";break;
               case 6:  text = "Noise";break;
               case 7:  text = "Model key-point";break;
               case 8:  text = "Water";break;
               case 9:  text = "Overlap";break;
               default: text = "Undefined";break;
            }
            glColor3d(1.0,1.0,1.0);//White.
            stringwidth = FONT_CHAR_WIDTH*text.length();
            glRasterPos3d(cornx - pixelsToImageUnits(hoffset + hwidth + stringwidth + hgap),
                          corny - padding - pixelsToImageUnits(0.5 * FONT_CHAR_HEIGHT)- length * i,
                          altitude);

            glBegin(GL_QUADS);
               glColor3fv(colour.getRGB());
               glVertex3d(cornx-pixelsToImageUnits(hoffset+hwidth),
                          corny-padding-i*length+pixelsToImageUnits(hwidth/2),
                          altitude);
               glVertex3d(cornx-pixelsToImageUnits(hoffset),
                          corny-padding-i*length+pixelsToImageUnits(hwidth/2),
                          altitude);
               glVertex3d(cornx-pixelsToImageUnits(hoffset),
                          corny-padding-i*length-pixelsToImageUnits(hwidth/2),
                          altitude);
               glVertex3d(cornx-pixelsToImageUnits(hoffset+hwidth),
                          corny-padding-i*length-pixelsToImageUnits(hwidth/2),
                          altitude);
            glEnd();
         }
         break;

      case colourByReturn:
         length = 0.9*rheight/7;
         for(int i=0;i<8;++i)
         {
            colour = getColourByReturn(i);
            switch(i)
            {
               case 0: text = "First";break;
               case 1: text = "Second";break;
               case 2: text = "Third";break;
               case 3: text = "Fourth";break;
               case 4: text = "Fifth";break;
               case 5: text = "Sixth";break;
               case 6: text = "Seventh";break;
               default:text = "Trouble at mill";break;
            }
           glColor3d(1.0,1.0,1.0);//White.
            stringwidth = FONT_CHAR_WIDTH*text.length();
            glRasterPos3d(cornx - pixelsToImageUnits(hoffset + hwidth + stringwidth + hgap),
                          corny - padding - pixelsToImageUnits(0.5*FONT_CHAR_HEIGHT)- length*i,altitude);

            glBegin(GL_QUADS);
               glColor3fv(colour.getRGB());
               glVertex3d(cornx-pixelsToImageUnits(hoffset+hwidth),
                          corny-padding-i*length+pixelsToImageUnits(hwidth/2),
                          altitude);
               glVertex3d(cornx-pixelsToImageUnits(hoffset),
                          corny-padding-i*length+pixelsToImageUnits(hwidth/2),
                          altitude);
               glVertex3d(cornx-pixelsToImageUnits(hoffset),
                          corny-padding-i*length-pixelsToImageUnits(hwidth/2),
                          altitude);
               glVertex3d(cornx-pixelsToImageUnits(hoffset+hwidth),
                          corny-padding-i*length-pixelsToImageUnits(hwidth/2),
                          altitude);
            glEnd();
         }
         break;
      default:
         break;
   }
}

/*
==================================
 TwoDeeOverview::makedistancescale

 This draws a scale. It works out what order of magnitude to use for the
 scale and the number of intervals to have in it and then modifies these
 if there would be too few or too many intervals. It then draws the
 vertical line and the small horizontal markers before setting up the font
 settings and then drawing the numbers by the markers.
==================================
*/
void TwoDeeOverview::makedistancescale()
{
   double rheight = pixelsToImageUnits(get_height());
   double order=1;

   // This finds the order of magnitude (base 10) of rheight with the added 
   // proviso that rheight must be at least five times that order so that 
   // there are enough intervals to draw a decent scale. This gives a range 
   // of nummarks values (below) of 5-50. While it may seem that the i 
   // variable could be used instead of rheight/(order*10), this is not the 
   // case as the latter is a double calculation, while the former is a result 
   // of a series of integer calculations, so the results diverge.
   if(rheight>5)
      for(int i=rheight;i>10;i/=10)
         if(rheight/(order*10)>5)
            order*=10;
   //For when the user zooms really far in.
   if(rheight<=5)
      for(double i=rheight;i<10;i*=10)
         order/=10;

   // Again, it would be tempting to use i here, but this is only one integer 
   // calculation while i is the result (probably) of several such 
   // calculations, and so has lost more precision.
   int nummarks = (int)(0.9*rheight/order);

   // The original order we calculated would give a number of scale widths from 
   // 5-50, but anything more than 10 is probably too much, so this loop doubles 
   // the order value until nummarks falls below 10.
   while(nummarks>10)
   {
      order*=2;
      nummarks = (int)(0.9*rheight/order);
   }

   //It would be more aesthetically pleasing to centre the scale.
   double padding = (rheight - nummarks*order)/2;

   //This makes sure the scale is drawn on top of the flightlines.
   double altitude = rmaxz+1000;
   GLint viewport[4];
   GLdouble modelview[16];
   GLdouble projection[16];
   GLdouble origx,origy,origz;
   origx = origy = origz = 0.0;
   glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
   glGetDoublev(GL_PROJECTION_MATRIX,projection);
   glGetIntegerv(GL_VIEWPORT,viewport);
   gluUnProject(0, 0, 0,modelview,projection,viewport,&origx,&origy,&origz);
   glColor3f(1.0,1.0,1.0);
   glBegin(GL_LINES);
      //Vertical line.
      glVertex3d(origx + pixelsToImageUnits(50.0),
                 origy + padding,
                 altitude);
      glVertex3d(origx + pixelsToImageUnits(50.0),
                 origy + padding + nummarks*order,
                 altitude);
      //Horizontal lines.
      for(int i=0;i<=nummarks;++i)
      {
         glVertex3d(origx + pixelsToImageUnits(50.0),
                    origy + padding + i*order,
                    altitude);
         glVertex3d(origx + pixelsToImageUnits(80.0),
                    origy + padding + i*order,
                    altitude);
      }
   glEnd();
   for(int i=0;i<=nummarks;++i)
   {
      char number[30];
      sprintf(number, "%.1lf", i*order);
      glRasterPos3d(origx + pixelsToImageUnits(85.0),
                    origy + padding + i*order - pixelsToImageUnits(0.5*FONT_CHAR_HEIGHT),altitude);
   }
}

/*
==================================
 TwoDeeOverview::on_pan_start

 On a left click, this prepares for panning by storing the initial i
 position of the cursor.
==================================
*/
bool TwoDeeOverview::on_pan_start(GdkEventButton* event)
{
   if(event->button==1 || event->button==2)
   {
      panStart.move(event->x, event->y, 0);

      // This causes the event box containing the overview to grab the focus, 
      // and so to allow keyboard control of the overview (this is not done 
      // directly as that would cause expose events to be called when focus
      // changes, resulting in graphical glitches).
      get_parent()->grab_focus();
      return true;
   }
   else if(event->button==3)return pointinfo(event->x,event->y);
   else return false;
}

/*
==================================
 TwoDeeOverview::on_pan

 As the cursor moves while the left button is depressed, the image is
 dragged along as a preview to reduce lag. The centre point is modified
 by the negative of the distance
 the cursor has moved to make a dragging effect and then the
 current position of the cursor is taken to be the starting position for
 the next drag (if there is one). The view is then refreshed and then the
 image is drawn (as a preview).
==================================
*/
bool TwoDeeOverview::on_pan(GdkEventMotion* event)
{
   if((event->state & Gdk::BUTTON1_MASK) == Gdk::BUTTON1_MASK || (event->state & Gdk::BUTTON2_MASK) == Gdk::BUTTON2_MASK)
   {
      centre.translate(-pixelsToImageUnits(event->x-panStart.getX()), pixelsToImageUnits(event->y-panStart.getY()), 0);
  
      set_overlay_centres(centre);
      panStart.move(event->x, event->y, 0);
      return drawviewable(2);
   }
   else if((event->state & Gdk::BUTTON3_MASK) == Gdk::BUTTON3_MASK)
      return pointinfo(event->x,event->y);
   else 
      return false;
}

/*
==================================
 TwoDeeOverview::on_pan_end

 At the end of the pan draw the full image.
==================================
*/
bool TwoDeeOverview::on_pan_end(GdkEventButton* event)
{
   if(event->button==1 || event->button==2)
      return drawviewable(panningRefresh);
   else 
      return false;
}

/*
==================================
 TwoDeeOverview::on_pan_key

 Moves view depending on keyboard commands.
==================================
*/
bool TwoDeeOverview::on_pan_key(GdkEventKey* event,double scrollspeed)
{
   switch(event->keyval)
   {
      case GDK_w: // Up
         centre.translate(0, pixelsToImageUnits(scrollspeed), 0);
         set_overlay_centres(centre);
         return drawviewable(2);
         break;
      case GDK_s: // Down
         centre.translate(0, -pixelsToImageUnits(scrollspeed), 0);
         set_overlay_centres(centre);
         return drawviewable(2);
         break;
      case GDK_a: // Left
         centre.translate(-pixelsToImageUnits(scrollspeed), 0, 0);
         set_overlay_centres(centre);
         return drawviewable(2);
         break;
      case GDK_d: // Right
         centre.translate(pixelsToImageUnits(scrollspeed), 0, 0);
         set_overlay_centres(centre);
         return drawviewable(2);
         break;
      case GDK_z: // Redraw
      case GDK_Z:
         return drawviewable(1);
         break;
      default:
         return false;
         break;
   }
}

/*
==================================
 TwoDeeOverview::on_prof_start

 At the beginning of profiling, defines the start point and, for the
 moment, the end point of the profile, Prepares the profile box for
 drawing and then calls the drawing method.
==================================
*/
bool TwoDeeOverview::on_prof_start(GdkEventButton* event)
{
   if(event->button==1)
   {
      profbox->on_start(Point(event->x, event->y, 0),get_width(),get_height());

      // This causes the event box containing the overview to grab the focus, 
      // and so to allow keyboard control of the overview (this is not done 
      // directly as that would cause expose events to be called when focus
      // changes, resulting in graphical glitches).
      get_parent()->grab_focus();
      return drawviewable(2);
   }
   else if(event->button==2)
      return on_pan_start(event);
   else if(event->button==3)
      return pointinfo(event->x,event->y);
   else 
      return false;
}

/*
==================================
 TwoDeeOverview::on_prof

 Updates the end point of the profile and then gets the vertical and
 horizontal differences betweent the start and end points. These are used
 to determine the length of the profile and hence the positions of the
 vertices of the profile rectangle. Then the drawing method is called.
==================================
*/
bool TwoDeeOverview::on_prof(GdkEventMotion* event)
{
   if((event->state & Gdk::BUTTON1_MASK) == Gdk::BUTTON1_MASK)
   {
      profbox->on_(Point(event->x, event->y),get_width(),get_height());
      return drawviewable(2);
   }
   else if((event->state & Gdk::BUTTON2_MASK) == Gdk::BUTTON2_MASK)
      return on_pan(event);
   else if((event->state & Gdk::BUTTON3_MASK) == Gdk::BUTTON3_MASK)
      return pointinfo(event->x,event->y);
   else return false;
}

/*
==================================
 TwoDeeOverview::on_prof_end

 Draw the full image at the end of selecting a profile.
==================================
*/
bool TwoDeeOverview::on_prof_end(GdkEventButton* event)
{
   if(event->button==1)
   {
      profbox->makeboundaries();
      return drawviewable(2);
   }
   else if(event->button==2)
      return on_pan_end(event);
   else 
      return false;
}

/*
==================================
 TwoDeeOverview::on_prof_key

 Moves the profile depending on keyboard commands.
==================================
*/
bool TwoDeeOverview::on_prof_key(GdkEventKey* event,double scrollspeed,bool fractionalshift)
{
   //5 means 0.5 etc. as fraction.
   if(fractionalshift)
      scrollspeed /= 10;
   else 
      scrollspeed *= ratio/zoomlevel;
   //Delegate to profile overlay object.
   bool moved = profbox->on_key(event,scrollspeed,fractionalshift);
   if(!moved)
      return false;
   profbox->makeboundaries();
   return 
      drawviewable(2);
}

/*
==================================
 TwoDeeOverview::on_fence_start

 Initialises the coordinates of the fence and then draws preview.
==================================
*/
bool TwoDeeOverview::on_fence_start(GdkEventButton* event)
{
   if(event->button==1)
   {
      fencebox->on_start(Point(event->x, event->y, 0),get_width(),get_height());

      // This causes the event box containing the overview to grab the focus, 
      // and so to allow keyboard control of the overview (this is not done 
      // directly as that would cause expose events to be called when focus
      // changes, resulting in graphical glitches).
      get_parent()->grab_focus();
      return drawviewable(2);
   }
   else if(event->button==2)
      return on_pan_start(event);
   else if(event->button==3)
      return pointinfo(event->x,event->y);
   else 
      return false;
}

/*
==================================
 TwoDeeOverview::on_fence

 Updates end coordinates of the fence and then draws preview.
==================================
*/
bool TwoDeeOverview::on_fence(GdkEventMotion* event)
{
   if((event->state & Gdk::BUTTON1_MASK) == Gdk::BUTTON1_MASK)
   {
      fencebox->on_(Point(event->x, event->y) ,get_width(),get_height());
      fencebox->drawinfo();
      return drawviewable(2);
   }
   else if((event->state & Gdk::BUTTON2_MASK) == Gdk::BUTTON2_MASK)
      return on_pan(event);
   else if((event->state & Gdk::BUTTON3_MASK) == Gdk::BUTTON3_MASK)
      return pointinfo(event->x,event->y);
   else return false;
}

/*
==================================
 TwoDeeOverview::on_fence_end

 Draws the main image once more.
==================================
*/
bool TwoDeeOverview::on_fence_end(GdkEventButton* event)
{
   if(event->button==1)
   {
      fencebox->makeboundaries();
      return drawviewable(2);
   }
   else if(event->button==2)
      return on_pan_end(event);
   else 
      return false;
}

/*
==================================
 TwoDeeOverview::on_fence_key

 Moves the fence depending on keyboard commands.
==================================
*/
bool TwoDeeOverview::on_fence_key(GdkEventKey* event,double scrollspeed)
{
   bool moved = fencebox->on_key(event,pixelsToImageUnits(scrollspeed),false);
   if(!moved)
      return false;

   fencebox->makeboundaries();
   return drawviewable(2);
}

/*
==================================
 TwoDeeOverview::on_ruler_start

 Find the starting coordinates of the ruler and set the label values to zero.
==================================
*/
bool TwoDeeOverview::on_ruler_start(GdkEventButton* event)
{
   if(event->button==1)
   {
      rulerEnd.move(centre.getX() + pixelsToImageUnits(event->x-get_width()/2),
                    centre.getY() - pixelsToImageUnits(event->y-get_height()/2), 
                    0);
      rulerStart = rulerEnd;
      ostringstream xpos,ypos;
      xpos << rulerEnd.getX();
      ypos << rulerEnd.getY();
      rulerlabel->set_text("Distance: 0\nX: 0 Pos: "+ xpos.str() + 
                                      "\nY: 0 Pos: " + ypos.str());

      rulerEventStart = Point(event->x, event->y);

      // This causes the event box containing the overview to grab the focus, 
      // and so to allow keyboard control of the overview (this is not done 
      // directly as that wuld cause expose events to be called when focus 
      // changes, resulting in graphical glitches).
      get_parent()->grab_focus();
      return drawviewable(2);
   }
   else if(event->button==2)
      return on_pan_start(event);
   else if(event->button==3)
      return pointinfo(event->x,event->y);
   else 
      return false;
}

/*
==================================
 TwoDeeOverview::on_ruler

 Find the current cursor coordinates in image terms (as opposed to
 window/screen terms) and then update the label with the distances.
 Then draw the ruler.
==================================
*/
bool TwoDeeOverview::on_ruler(GdkEventMotion* event)
{
   if((event->state & Gdk::BUTTON1_MASK) == Gdk::BUTTON1_MASK)
   {
      rulerEnd.move(centre.getX() + pixelsToImageUnits(event->x-get_width()/2),
                    centre.getY() - pixelsToImageUnits(event->y-get_height()/2), 
                    0);
      double d,xd,yd;
      xd = abs(rulerEnd.getX()-rulerStart.getX());
      yd = abs(rulerEnd.getY()-rulerStart.getY());
      d = rulerStart.distanceTo(rulerEnd); //sqrt(xd*xd+yd*yd);
      ostringstream dist,xdist,ydist,xpos,ypos;
      dist << d;
      xdist << xd;
      ydist << yd;
      xpos << rulerEnd.getX();
      ypos << rulerEnd.getY();
      string rulerstring = "Distance: " + dist.str() +
                                "\nX: " + xdist.str() + 
                               " Pos: " + xpos.str() + 
                                "\nY: " + ydist.str() + 
                               " Pos: " + ypos.str();

      rulerlabel->set_text(rulerstring);
      return drawviewable(2);
   }
   else if((event->state & Gdk::BUTTON2_MASK) == Gdk::BUTTON2_MASK)
      return on_pan(event);
   else if((event->state & Gdk::BUTTON3_MASK) == Gdk::BUTTON3_MASK)
      return pointinfo(event->x,event->y);
   else return false;
}

/*
==================================
 TwoDeeOverview::on_ruler_end

 Draw again.
==================================
*/
bool TwoDeeOverview::on_ruler_end(GdkEventButton* event)
{
   if(event->button==1)
      return drawviewable(2);
   else if(event->button==2)
      return on_pan_end(event);
   else 
      return false;
}

/*
==================================
 TwoDeeOverview::makerulerbox

 Make the ruler as a thick line.
==================================
*/
void TwoDeeOverview::makerulerbox()
{
   //This makes sure the ruler is drawn on top of the flightlines.
   double altitude = rmaxz+1000;
   glColor3f(1.0,1.0,1.0);
   glLineWidth(3);
   glBegin(GL_LINES);
      glVertex3d(rulerStart.getX()-centre.getX(),rulerStart.getY()-centre.getY(),altitude);
      glVertex3d(rulerEnd.getX()-centre.getX(),rulerEnd.getY()-centre.getY(),altitude);
   glEnd();
   glLineWidth(1);
}

/*
==================================
  TwoDeeOverview::on_zoom

 First, half the distance between the centre of the window and the window
 position of the event is converted to image coordinates and added to
 the image centre. This is analogous to moving the centre to where the
 event occured. Then, depending on the direction of the scroll, the
 zoomlevel is increased or decreased. Then the centre is moved to where
 the centre of the window will now lie. The image is then drawn.

 (Anything that happens for centrex is reversed for centrey).
==================================
*/
bool TwoDeeOverview::on_zoom(GdkEventScroll* event)
{
	double factor = 2;

	if (superzoom)
		factor = 0.2;

   centre.translate(pixelsToImageUnits(event->x-get_width()/2),
                    -pixelsToImageUnits(event->y-get_height()/2), 
                    0);

   if(zoomlevel>=1)
   {
      if(event->direction==GDK_SCROLL_UP)
         zoomlevel+=pow(zoomlevel,zoompower)/factor;
      else if(event->direction==GDK_SCROLL_DOWN)
         zoomlevel-=pow(zoomlevel,zoompower)/factor;
   }
   else if(zoomlevel>=0.2)
   {
      if(event->direction==GDK_SCROLL_UP)
         zoomlevel+=0.1;
      else if(event->direction==GDK_SCROLL_DOWN)
         zoomlevel-=0.1;
   }
   else if(event->direction==GDK_SCROLL_UP)
      zoomlevel+=0.1;
   if(zoomlevel<0.2)
      zoomlevel=0.2;

   centre.translate(-pixelsToImageUnits(event->x-get_width()/2),
                    pixelsToImageUnits(event->y-get_height()/2),
                    0);
   resetview();

   // This causes the event box containing the overview to grab the focus, 
   // and so to allow keyboard control of the overview (this is not done 
   // directly as that would cause expose events to be called when focus
   // changes, resulting in graphical glitches).
   get_parent()->grab_focus();
   set_overlay_centres(centre);
   set_overlay_zoomlevels(zoomlevel);

   return drawviewable(1);
}

/*
==================================
 TwoDeeOverview::on_zoom_key

 Zooms depending on keyboard signals.
==================================
*/
bool TwoDeeOverview::on_zoom_key(GdkEventKey* event)
{
   if(zoomlevel>=1)
      switch(event->keyval)
      {
         case GDK_i: // In
         case GDK_I:
         case GDK_g:
         case GDK_G:
            zoomlevel+=pow(zoomlevel,zoompower)/2;
            break;
         case GDK_o: // Out
         case GDK_O:
         case GDK_b:
         case GDK_B:
            zoomlevel-=pow(zoomlevel,zoompower)/2;
            break;
         default:
            return false;
            break;
   }
   else if(zoomlevel>=0.2)
      switch(event->keyval)
      {
         case GDK_i: // In
         case GDK_I:
         case GDK_g:
         case GDK_G:
            zoomlevel+=0.1;
            break;
         case GDK_o: // Out
         case GDK_O:
         case GDK_b:
         case GDK_B:
            zoomlevel-=0.1;
            break;
         default:
            return false;
            break;
   }
   else switch(event->keyval)
   {
         case GDK_i: // In only
         case GDK_I:
         case GDK_g:
         case GDK_G:
            zoomlevel+=0.1;
            break;
         default:
            return false;
            break;
   }
   // Don't zoom too far
   if(zoomlevel<0.2)
      zoomlevel=0.2;

   resetview();
   set_overlay_zoomlevels(zoomlevel);
   return drawviewable(1);
}

/*
==================================
 TwoDeeOverview::toggleNoise

 Hides noisy points (classification 7).
==================================
*/
void TwoDeeOverview::toggleNoise()
{
   tdoDisplayNoise = !tdoDisplayNoise;
   drawviewable(1);
}

/*
==================================
 TwoDeeOverview::set_superzoom

 Increases the sensitivity of the mouse wheel so the zooming is much faster.
 Also increases the point size.
==================================
*/
void TwoDeeOverview::set_superzoom(bool zoom)
{
	superzoom = zoom;

	if (superzoom)
	{
	    Gdk::Cursor cursor(Gdk::CROSSHAIR);
	   	if(is_realized())get_window()->set_cursor(cursor);

		setpointwidth(2);

		for (int i = 0; i < 10; ++i)
		{
			zoomlevel+=pow(zoomlevel, zoompower)/2;
		}
	}
	else
	{
	   	if(is_realized())get_window()->set_cursor();
		setpointwidth(1.0);
	}

	resetview();
	set_overlay_zoomlevels(zoomlevel);
	drawviewable(1);
}

