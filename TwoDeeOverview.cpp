/*
 * File: TwoDeeOverview.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: November 2009 - February 2010
 *
 * WARNING! THIS IS A THREADED ENVIRONMENT. CARELESSNESS WILL BE REWARDED WITH EXTREME PREJUDICE (YES, YOU READ THAT CORRECTLY)!
 *
 * */
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <vector>
#include <iostream>
#include "quadtree.h"
#include "quadtreestructs.h"
#include "pointbucket.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "TwoDeeOverview.h"
#include "MathFuncs.h"

TwoDeeOverview::TwoDeeOverview(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit,Gtk::Label *rulerlabel)  : Display(config,lidardata,bucketlimit){
   pointcount = 0;
   threaddebug = false;
   zoompower = 0.5;
   maindetailmod = 0.01;
   //Limits of pixel copying for the preview:
   drawnsofarminx=0;
   drawnsofarminy=0;
   drawnsofarmaxx=1;
   drawnsofarmaxy=1;
   //Threading:
   thread_existsmain = false;
   thread_existsthread = false;
   interruptthread = false;
   drawing_to_GL = false;
   initialising_GL_draw = false;
   flushing = false;
   extraDrawing = false;
   //Profiling:
   profwidth=30;
   profiling=false;
   showprofile=false;
   //Fencing:
   fencing=false;
   showfence=false;
   //Rulering:
   rulering=false;
   rulerwidth=2;
   this->rulerlabel = rulerlabel;
   //Classification heightening:
   heightenNonC = false;
   heightenGround = false;
   heightenLowVeg = false;
   heightenMedVeg = false;
   heightenHighVeg = false;
   heightenBuildings = false;
   heightenNoise = false;
   heightenMass = false;
   heightenWater = false;
   heightenOverlap = false;
   heightenUndefined = false;
   //Events and signals:
   add_events(Gdk::SCROLL_MASK   |   Gdk::BUTTON1_MOTION_MASK   |   Gdk::BUTTON3_MOTION_MASK   |   Gdk::BUTTON_PRESS_MASK   |   Gdk::BUTTON_RELEASE_MASK);
      //Zooming:
      signal_scroll_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_zoom));
      //Panning:
      sigpanstart = signal_button_press_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_pan_start));
      sigpan = signal_motion_notify_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_pan));
      sigpanend = signal_button_release_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_pan_end));
      //Profiling:
      sigprofstart = signal_button_press_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_prof_start));
      sigprof = signal_motion_notify_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_prof));
      sigprofend = signal_button_release_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_prof_end));
      sigprofstart.block();//Not in profiling mode initially.
      sigprof.block();//...
      sigprofend.block();//...
      //Fencing:
      sigfencestart = signal_button_press_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_fence_start));
      sigfence = signal_motion_notify_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_fence));
      sigfenceend = signal_button_release_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_fence_end));
      sigfencestart.block();//Not in fencing mode initially.
      sigfence.block();//...
      sigfenceend.block();//...
      //Rulering:
      sigrulerstart = signal_button_press_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_ruler_start));
      sigruler = signal_motion_notify_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_ruler));
      sigrulerend = signal_button_release_event().connect(sigc::mem_fun(*this,&TwoDeeOverview::on_ruler_end));
      sigrulerstart.block();//Not in rulering mode initially.
      sigruler.block();//...
      sigrulerend.block();//...
      //Dispatchers (threading):
      signal_InitGLDraw.connect(sigc::mem_fun(*this,&TwoDeeOverview::InitGLDraw));
      signal_DrawGLToCard.connect(sigc::mem_fun(*this,&TwoDeeOverview::DrawGLToCard));
      signal_FlushGLToScreen.connect(sigc::mem_fun(*this,&TwoDeeOverview::FlushGLToScreen));
      signal_EndGLDraw.connect(sigc::mem_fun(*this,&TwoDeeOverview::EndGLDraw));
      signal_extraDraw.connect(sigc::mem_fun(*this,&TwoDeeOverview::extraDraw));
}

TwoDeeOverview::~TwoDeeOverview(){}

//Dispatcher handlers{
//This handler prepares OpenGL for drawing the buckets, by "beginning" OpenGL and then clearing the buffers. It also then draws any profile or fencing boxes or the ruler.
void TwoDeeOverview::InitGLDraw(){
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Need to clear screen because of gaps.
   glwindow->gl_end();
   initialising_GL_draw = false;//The drawing thread running mainimage() may now continue. Also, new drawing threads may now be started.
}
//This handler draws the contents of the arrays vertices and colours to the framebuffer. Note that the reason the OpenGL vertex array stuff is in here is because, apparently, the glDisableClientState() calls try to access the arrays, and this caused valgrind to squeal. It would make the program marginally faster to add a boolean variable "ending_GL_draw" so that EndGLDraw could end the vertex array enablement (and InitGLDraw could start it), but is it worth the global variable and the extra effort? Also, it is not certain, but it seems that since the vertex array stuff has moved here the missing bucket problem has gone, and it is also now very very difficult to make the thread cause a crash.
void TwoDeeOverview::DrawGLToCard(){
   if(threaddebug)cout << "Boo!" << endl;
//   vertex_array_mutex.lock();
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return;
   glEnableClientState(GL_VERTEX_ARRAY);//These relate to the enabling of vertex arrays and assigning the arrays to GL.
   glEnableClientState(GL_COLOR_ARRAY);//...
   glVertexPointer(3, GL_FLOAT, 0, vertices);//...
   glColorPointer(3, GL_FLOAT, 0, colours);//...
   glDrawArrays(GL_POINTS,0,pointcount);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glwindow->gl_end();
   drawing_to_GL = false;//The drawing thread running mainimage() may now continue. Also, new drawing threads may now be started.
//   vertex_array_condition.signal();
//   vertex_array_mutex.unlock();
}
//This handler exists so that flushing of the framebuffer can be done independently of drawing to it. This allows an arbitrary frequency of flushes to be easily set, to compromise between showing the user things are happening and the desire to reduce the flicker and the fact that more flushes will make drawing slower.
void TwoDeeOverview::FlushGLToScreen(){
   if(threaddebug)cout << "Lalalalalaaa!" << endl;
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return;
   if (glwindow->is_double_buffered())glwindow->swap_buffers();//Draw to screen every (few) bucket(s) to show user stuff is happening.
   else glFlush();
   glwindow->gl_end();
   flushing = false;//The main thread may now create new drawing threads.
}
//This cleans up when a drawing thread is ending/has ended. It flushes once more so that everything is visible on the screen, ends the OpenGL session and then records that new threads may be made.
void TwoDeeOverview::EndGLDraw(){
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return;
   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   else glFlush();
   glReadBuffer(GL_BACK);
   glDrawBuffer(GL_FRONT);
   if(profiling||showprofile)makeprofbox();//Draw the profile box if profile mode is on.
   if(rulering)makerulerbox();//Draw the ruler if ruler mode is on.
   if(fencing||showfence)makefencebox();//Draw the fence box if fence mode is on.
   glFlush();
   glDrawBuffer(GL_BACK);
   glwindow->gl_end();
   thread_existsmain = false;//This tells the main thread that it can now produce a new drawing thread.
}
//This tells the main thread to draw the main image again after all its tasks currently in its queue are complete.
void TwoDeeOverview::extraDraw(){
   extraDrawing = false;//Now any subsequent calls to drawiewable, including the one immediately below this line, may trigger another extraDraw() if interrupted by the fact that the drawing thread is waiting for this (the main) thread to carry our OpenGL operations.
   drawviewable(1);
}
//...}

/*This method draws the main image. Note that redundancy in telling the main thread what to do is because the main thread's execution of the signals it receives is not deterministic because of the possibility of user interference.
 *
 *   Tell the main thread not to create any more threads until this one has finished most of its running.
 *   Wait until all other drawing threads have ended.
 *   Tell any new threads that are created to wait until this one is finished if this one still exists.
 *   Tell the main thread not to create any more threads until the OpenGL has been initialised.
 *   Signal main thread to initialise OpenGL for drawing.
 *   FOR every bucket:
 *      Wait until the main thread has finished drawing to the framebuffer.
 *      IF the thread is being interrupted:
 *         Delete data array.
 *         Signal main thread to clear up the OpenGL settings for drawing.
 *         Tell the main thread that this thread is no longer (significantly) running.
 *         Delete vertices and colours arrays.
 *         Set it so that subsequent threads will not be interrupted immediately.
 *         Allow subsequent threads to act.
 *         End thread (return).
 *      Determine the boundary of the rectangle that just encompasses all the buckets being drawn so far.
 *      FOR every point:
 *         determine colour and brightness of point
 *         IF any classifications are selected for increased prominence, set the z coordinate of the point to be higher than any other points.
 *         place point
 *      IF the thread is not being interrupted:
 *         Tell the main thread not to create any more threads until the current bucket has been drawn to the framebuffer.
 *         Signal the main thread to draw the current bucket to the framebuffer.
 *         Every Nth bucket (currently 10th):
 *            Tell main thread not to create any more threads until the frambuffer has been flushed to the screen.
 *            Signal the main thread to flush the framebuffer to the screen.
 *   Wait until the main thread has finished drawing to the framebuffer.
 *   Delete data array.
 *   Signal main thread to clear up the OpenGL settings fo drawing.
 *   Tell the main thread that this thread is no longer (significantly) running.
 *   Delete vertices and colours arrays.
 *   Set it to that subsequent threads will not be interrupted immediately.
 *   Allow subsequent threads to act.
 *   (Thread ends).
 * */
void TwoDeeOverview::mainimage(pointbucket** buckets,int numbuckets,int detail){
   if(threaddebug)cout << "Wait?" << endl;
   while(thread_existsthread){usleep(100);}//If another thread still exists (i.e. it has not cleared itself up yet) then wait until it is cleared.
   if(threaddebug)cout << "***Finished waiting." << endl;
   thread_existsthread = true;//Any subsequent threads must wait until this becomes false again.
   centrexsafe = centrex;//These are "safe" versions of the centre coordinates, as they will not change while this thread is running, while the originals might.
   centreysafe = centrey;//...
   int line=0,intensity=0,classification=0,rnumber=0;
   double x=0,y=0,z=0;//Point values
   double red,green,blue;//Colour values
//   vertex_array_mutex.lock();
   if(threaddebug)cout << "First array" << endl;
   vertices = new float[3*bucketlimit];
   if(threaddebug)cout << "Second array" << endl;
   colours = new float[3*bucketlimit];
   if(threaddebug)cout << "Initialise GL drawing." << endl;
   initialising_GL_draw = true;//The main thread must not create any new threads like this while also being told to initialise OpenGL for drawing.
   signal_InitGLDraw();//Prepare OpenGL.
   boundary* lidarboundary = lidardata->getboundary();
   drawnsofarminx=lidarboundary->maxX;//Preparing to extract boundaries of drawn area.
   drawnsofarminy=lidarboundary->maxY;//...
   drawnsofarmaxx=lidarboundary->minX;//...
   drawnsofarmaxy=lidarboundary->minY;//...
   delete lidarboundary;
   for(int i=0;i<numbuckets;i++){//For every bucket...
      if(threaddebug)cout << i << " " << numbuckets << endl;
      if(threaddebug)cout << buckets[i]->getnumberofpoints() << endl;
      if(threaddebug)cout << detail << endl;
      if(threaddebug)cout << "If drawing, pause." << endl;
      while(drawing_to_GL){usleep(10);}//Under no circumstances may the arrays be modified until their contents have been sent to the framebuffer.
      if(threaddebug)cout << "Not drawing (anymore)." << endl;
      if(threaddebug)cout << "Interrupt?" << endl;
      if(interruptthread){
         if(threaddebug)cout << "Interrupted." << endl;
         if(threaddebug)cout << "Delete data array." << endl;
         delete[] buckets;//This is up here so that buckets is deleted before it is newed again.
         if(threaddebug)cout << "End drawing" << endl;
         signal_EndGLDraw();//For the sake of neatness, clear up. This comes before allowing the main thread to create another thread like this to ensure that this signal is processed before, say, a signal to prepare OpenGL for drawing again.
         if(threaddebug)cout << "Allowing main thread to start new thread... DANGER!" << endl;
         if(threaddebug)cout << "Delete vertex array." << endl;
         delete[] vertices;//These are here, before a new thread like this is allowed to do anything, so that they are deleted before they are newed again.
         if(threaddebug)cout << "Delete colour array." << endl;
         delete[] colours;//...
//         vertex_array_mutex.unlock();
         if(threaddebug)cout << "Booleans." << endl;
         interruptthread = false;//New threads like this will now not be interrupted.
         thread_existsthread = false;//New threads like this will now be allowed to act.
         if(threaddebug)cout << "*********Finished thread!" << endl;
         return;
      }
      if(threaddebug)cout << "No interrupt." << endl;
      pointcount=0;//This is needed for putting values in the right indices for the above arrays. j does not suffice because of the detail variable.
      if(buckets[i]->getminX()<drawnsofarminx)drawnsofarminx = buckets[i]->getminX();//Set the boundary of the buckets selected so far.
      if(buckets[i]->getminY()<drawnsofarminy)drawnsofarminy = buckets[i]->getminY();//...
      if(buckets[i]->getmaxX()>drawnsofarmaxx)drawnsofarmaxx = buckets[i]->getmaxX();//...
      if(buckets[i]->getmaxY()>drawnsofarmaxy)drawnsofarmaxy = buckets[i]->getmaxY();//...
      for(int j=0;j<buckets[i]->getnumberofpoints();j+=detail){//... and for every point, determine point colour and position:
         red = 0.0; green = 1.0; blue = 0.0;//Default colour.
//         if(threaddebug)cout << "Get coords." << endl;
         x = buckets[i]->getpoint(j).x;
         y = buckets[i]->getpoint(j).y;
         z = buckets[i]->getpoint(j).z;
         intensity = buckets[i]->getpoint(j).intensity;
//         if(threaddebug)cout << "Colours!" << endl;
         if(heightcolour){//Colour by elevation.
            red = colourheightarray[3*(int)(10*(z-rminz))];
            green = colourheightarray[3*(int)(10*(z-rminz)) + 1];
            blue = colourheightarray[3*(int)(10*(z-rminz)) + 2];
         }
         else if(intensitycolour){//Colour by intensity.
            red = colourintensityarray[3*(int)(intensity-rminintensity)];
            green = colourintensityarray[3*(int)(intensity-rminintensity) + 1];
            blue = colourintensityarray[3*(int)(intensity-rminintensity) + 2];
         }
         else if(linecolour){//Colour by flightline. Repeat 6 distinct colours.
             line = buckets[i]->getpoint(j).flightline;
             int index = line % 6;
             switch(index){
                case 0:red=0;green=1;blue=0;break;//Green
                case 1:red=0;green=0;blue=1;break;//Blue
                case 2:red=1;green=0;blue=0;break;//Red
                case 3:red=0;green=1;blue=1;break;//Cyan
                case 4:red=1;green=1;blue=0;break;//Yellow
                case 5:red=1;green=0;blue=1;break;//Purple
                default:red=green=blue=1;break;//White in the event of strangeness.
             }
         }
         else if(classcolour){//Colour by classification.
             classification = buckets[i]->getpoint(j).classification;
             int index = classification;
             switch(index){
                case 0:case 1:red=1;green=1;blue=0;break;//Yellow for non-classified.
                case 2:red=0.6;green=0.3;blue=0;break;//Brown for ground.
                case 3:red=0;green=0.3;blue=0;break;//Dark green for low vegetation.
                case 4:red=0;green=0.6;blue=0;break;//Medium green for medium vegetation.
                case 5:red=0;green=1;blue=0;break;//Bright green for high vegetation.
                case 6:red=0;green=1;blue=0;break;//Cyan for buildings.
                case 7:red=1;green=0;blue=1;break;//Purple for low point (noise).
                case 8:red=0.5;green=0.5;blue=0.5;break;//Grey for model key-point (mass point).

                case 9:red=0;green=0;blue=1;break;//Blue for water.
                case 12:red=1;green=1;blue=1;break;//White for overlap points.
                default:red=1;green=0;blue=0;cout << "Undefined point." << endl;break;//Red for undefined.
             }
         }
         else if(returncolour){//Colour by flightline. Repeat 6 distinct colours.
             rnumber = buckets[i]->getpoint(j).rnumber;
             int index = rnumber;
             switch(index){
                case 1:red=0;green=0;blue=1;break;//Blue
                case 2:red=0;green=1;blue=1;break;//Cyan
                case 3:red=0;green=1;blue=0;break;//Green
                case 4:red=1;green=0;blue=0;break;//Red
                case 5:red=1;green=0;blue=1;break;//Purple
                default:red=green=blue=1;break;//White in the event of strangeness.
             }
         }
         if(heightbrightness){//Shade by height.
            red *= brightnessheightarray[(int)(z-rminz)];
            green *= brightnessheightarray[(int)(z-rminz)];
            blue *= brightnessheightarray[(int)(z-rminz)];
         }
         else if(intensitybrightness){//Shade by intensity.
            red *= brightnessintensityarray[(int)(intensity-rminintensity)];
            green *= brightnessintensityarray[(int)(intensity-rminintensity)];
            blue *= brightnessintensityarray[(int)(intensity-rminintensity)];
         }
         vertices[3*pointcount]=x-centrexsafe;
         vertices[3*pointcount+1]=y-centreysafe;
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
            heightenUndefined){
            classification = buckets[i]->getpoint(j).classification;
            int index = classification;
            double incrementor = 2*abs(rmaxz-rminz);
            switch(index){
               case 0:case 1:if(heightenNonC)z+=incrementor;break;//Heighten non-classified.
               case 2:if(heightenGround)z+=incrementor;break;//Heighten the ground.
               case 3:if(heightenLowVeg)z+=incrementor;break;//Heighten low vegetation.
               case 4:if(heightenMedVeg)z+=incrementor;break;//Heighten medium vegetation.
               case 5:if(heightenHighVeg)z+=incrementor;break;//Heighten hig vegetation.
               case 6:if(heightenBuildings)z+=incrementor;break;//Heighten buildings.
               case 7:if(heightenNoise)z+=incrementor;break;//Heighten noise.
               case 8:if(heightenMass)z+=incrementor;break;//Heighten mass points.
               case 9:if(heightenWater)z+=incrementor;break;//Heighten water.
               case 12:if(heightenOverlap)z+=incrementor;break;//Heighten overlaps.
               default:if(heightenUndefined)z+=incrementor;break;//Heighten anything else.
            }
         }
         vertices[3*pointcount+2]=z;
         colours[3*pointcount]=red;
         colours[3*pointcount+1]=green;
         colours[3*pointcount+2]=blue;
         pointcount++;
      }
      if(threaddebug)cout << pointcount << endl;
      if(threaddebug)cout << vertices[3*pointcount/2] << endl;
      if(threaddebug)cout << "Draw if not interrupted." << endl;
      if(!interruptthread){
         if(threaddebug)cout << "Sending draw signal." << endl;
         if(threaddebug)cout << "Yes!" << endl;
         drawing_to_GL = true;//Main thread must not attempt to create a new thread like this while this is waiting for a draw to the framebuffer.
//         while(drawing_to_GL)vertex_array_condition.wait(vertex_array_mutex);
         signal_DrawGLToCard();
         if(i>=(numbuckets-1)||numbuckets>10)if((i+1)%10==0){
            flushing = true;//Main thread must not attempt to create a new thread like this while flushing has yet to occur.
            if(threaddebug)cout << "Sending flush signal." << endl;
            signal_FlushGLToScreen();
         }
      }
      else if(threaddebug)cout << "Draw interrupted." << endl;
   }
   while(drawing_to_GL){usleep(10);}
   if(threaddebug)cout << "Ending..." << endl;
   if(threaddebug)cout << "Delete data array." << endl;
   delete[] buckets;//This is up here so that buckets is deleted before it is newed again.
   if(threaddebug)cout << "Allowing main thread to start new thread... DANGER!" << endl;
   if(threaddebug)cout << "End drawing" << endl;
   signal_EndGLDraw();//For the sake of neatness, clear up. This comes before allowing the main thread to create another thread like this to ensure that this signal is processed before, say, a signal to prepare OpenGL for drawing again.
   if(threaddebug)cout << "Delete vertex array." << endl;
   delete[] vertices;//These are here, before a new thread like this is allowed to do anything, so that they are deleted before they are newed again.
   if(threaddebug)cout << "Delete colour array." << endl;
   delete[] colours;//...
//   vertex_array_mutex.unlock();
   if(threaddebug)cout << "Booleans." << endl;
   interruptthread = false;//New threads like this will now not be interrupted.
   thread_existsthread = false;//New threads like this will now be allowed to act.
   if(threaddebug)cout << "***Finished thread!" << endl;
   return;
}

//This method draws a preview version of the image for any situations where it must be drawn quickly. It does this by first electing to draw directly to the front buffer and to flush it, rather than using double buffering and the swap_buffers() command. It then clears the front buffer using glClear() and then builds the profile box, the ruler or the fence box in the event that one of them is active. After that it draws the outline of every bucket in the subset, in order to give the user a skeletal idea of position. The method then copies from the back buffer to the front buffer a region of pixels that corresponds with a rectangle that just covers all of the buckets drawn before. This way, if the entire image is loaded then the user sees it all moving, perfectly. If some of the image is "off the edge of the screen" then when it moves the uncovered areas will show the "skeleton" of the buckets. The user will also see the "skeleton" of the buckets if they elect to do something that will cause a preview to be drawn before the main image is complete, as only the complete portions will be drawn. The drawing buffer is then set back to the back.
bool TwoDeeOverview::drawbuckets(pointbucket** buckets,int numbuckets){
   glReadBuffer(GL_BACK);//We want to copy from here.
   glDrawBuffer(GL_FRONT);//We want to draw to here.
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return false;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Need to clear screen because of gaps.
   if(profiling||showprofile)makeprofbox();//Draw the profile box if profile mode is on.
   if(rulering)makerulerbox();//Draw the ruler if ruler mode is on.
   if(fencing||showfence)makefencebox();//Draw the fence box if fence mode is on.
   double altitude = rminz-1000;//This makes sure the preview box is drawn under the flightlines.
   double xpos = drawnsofarminx-centrex;//The position of the bottom left corner of where the region is to be copied TO. In world coordinates.
   double ypos = drawnsofarminy-centrey;//...
   double xoffset = 0;//These offsets are used for when the position of the bottom left corner of the destination region would go off the screen to the left or bottom (which would cause NOTHING to be drawn). In pixels.
   double yoffset = 0;//...
   if(xpos < -(get_width()/2)*ratio/zoomlevel){//If the destination region's bottom left corner is off to the left:
      xoffset = -xpos*zoomlevel/ratio - get_width()/2;//Minus sign because must be positive and xpos will be negative. Makes xpos into pixels and then subtracts half the width of the window, as 0 (world coordinates) is in the centre.
      xpos = -(get_width()/2)*ratio/zoomlevel;//This is set to the left edge of the screen.
   }
   if(ypos < -(get_height()/2)*ratio/zoomlevel){//If the destination region's bottom left corner is off beyond the bottom:
      yoffset = -ypos*zoomlevel/ratio - get_height()/2;//Minus sign because must be positive and ypos will be negative. Makes ypos into pixels and then subtracts half the height of the windo, as 0 (world coordinates) is in the centre. 
      ypos = -(get_height()/2)*ratio/zoomlevel;//This is set to the bottom edge of the screen.
   }
   double* params = new double[4];//This section if for making sure that the xpos and ypos numbers are greater than or equal to zero, for otherwise NOTHING will be drawn:
   glGetDoublev(GL_CURRENT_RASTER_POSITION,params);//...
   if(params[0]<0)xpos++;//...For x.
   if(params[1]<0)ypos++;//...For y.
   delete[] params;//...Do not need the other two elements: x and w.
   glRasterPos3f(xpos,ypos,altitude+1);//Finally set the position of the bottom left corner of the destination region. This takes world coordinates and converts them into pixels. glWindowPos does the same thing directly with pixels, but it requires OpenGL 1.4.
   double bucketminx = (drawnsofarminx-centrexsafe)*zoomlevel/ratio + get_width()/2 + xoffset;//These define the boundaries of the region to be copied FROM.
   double bucketminy = (drawnsofarminy-centreysafe)*zoomlevel/ratio + get_height()/2 + yoffset;//...The offsets are here so that if the destination position should be too far left or down then these account for it. Otherwise the skeleton and copy part ways.
   double bucketmaxx = (drawnsofarmaxx-centrexsafe)*zoomlevel/ratio + get_width()/2;//...
   double bucketmaxy = (drawnsofarmaxy-centreysafe)*zoomlevel/ratio + get_height()/2;//...
   if(bucketmaxx > get_width())bucketmaxx = get_width();//I think that having ANYTHING going beyond the boundaries of the screen will cause NOTHING to be drawn. Apparently bottom left corner does not matter here, though, only the top right (???).
   if(bucketmaxy > get_height())bucketmaxy = get_height();//...
   if(bucketminx > bucketmaxx)bucketminx = bucketmaxx;//...Oh, and the bottom left corner must be further left and down than the top right corner.
   if(bucketminy > bucketmaxy)bucketminy = bucketmaxy;//...
//   if(bucketminx < 1)bucketminx = 1;//...
//   if(bucketminy < 1)bucketminy = 1;//...
   glCopyPixels(bucketminx,bucketminy,bucketmaxx-bucketminx,bucketmaxy-bucketminy,GL_COLOR);//The business end, at last. Copies from the region defined to the current raster position.
   //******************************************************************************Still not sure whether to draw the overlays (ruler,profile,fence) in GTK or OpenGL. GTK would require more work and duplication, and more complexity, but would allow the user to draw over the flightline while it was still drawing in full detail and from scratch. Hmmmmmmm...
//   int line=0,intensity=0,classification=0,rnumber=0;
//   double x=0,y=0,z=0;//Point values
//   double red,green,blue;//Colour values
   float* vertices = new float[15];//Needed for the glDrawArrays() call further down.
//   float* colours = new float[3*bucketlimit];//...
   glEnableClientState(GL_VERTEX_ARRAY);//...
//   glEnableClientState(GL_COLOR_ARRAY);//...
   glVertexPointer(3, GL_FLOAT, 0, vertices);//...
//   glColorPointer(3, GL_FLOAT, 0, colours);//...
   glColor3f(1.0,1.0,1.0);
   for(int i=0;i<numbuckets;i++){//For every bucket...
//      red = 0.0; green = 1.0; blue = 0.0;//Default colour.
      vertices[0]=buckets[i]->getminX()-centrex;
      vertices[1]=buckets[i]->getminY()-centrey;
      vertices[2]=altitude;
      vertices[3]=buckets[i]->getminX()-centrex;
      vertices[4]=buckets[i]->getmaxY()-centrey;
      vertices[5]=altitude;
      vertices[6]=buckets[i]->getmaxX()-centrex;
      vertices[7]=buckets[i]->getmaxY()-centrey;
      vertices[8]=altitude;
      vertices[9]=buckets[i]->getmaxX()-centrex;
      vertices[10]=buckets[i]->getminY()-centrey;
      vertices[11]=altitude;
//      colours[3*count]=red;
//      colours[3*count+1]=green;
//      colours[3*count+2]=blue;
      glDrawArrays(GL_LINE_LOOP,0,4);
   }
   glFlush();//After all this effort, something must be drawn to the screen.
   glRasterPos2s(0,0);//Reset the raster position.
   glDisableClientState(GL_VERTEX_ARRAY);
//   glDisableClientState(GL_COLOR_ARRAY);
   glDrawBuffer(GL_BACK);
   glwindow->gl_end();
   delete[] vertices;
//   delete[] colours;
   return true;
}

//Gets the limits of the viewable area and passes them to the subsetting method of the quadtree to get the relevant data. It then converts from a vector to a pointer array to make data extraction faster. Then, depending on the imagetype requested, it either sets the detail level and then creates a thread for drawing the main image (imagetype==1) or calls drawbuckets in order to give a preview of the data when panning etc. (imagetype==2).
bool TwoDeeOverview::drawviewable(int imagetype){
   interruptthread = true;//This causes any existing drawing thread to stop.
   glPointSize(pointsize);
   get_gl_window()->make_current(get_gl_context());//These are done so that graphical artefacts through changes of view to not occur. This is because of being a multiwindow application.
   glViewport(0, 0, get_width(), get_height());//...
   resetview();//...
   if(imagetype==1){//Draw the main image.
      if(drawing_to_GL||initialising_GL_draw||flushing||thread_existsthread||thread_existsmain){//If any of these conditions are true and a new thread is created now, deadlock is possible.
         if(threaddebug)cout << "Help! Am stalling!" << endl;
         if(!extraDrawing){//If not doing so already, prepare to draw again after the interrupt.
            extraDrawing = true;
            signal_extraDraw();
         }
         return true;
      }
      if(threaddebug)cout << "Am fluid" << endl;
      if(threaddebug)cout << "Changed offsets." << endl;
      double minx = centrex-(get_width()/2)*ratio/zoomlevel;//Limits of viewable area:
      double maxx = centrex+(get_width()/2)*ratio/zoomlevel;//...
      double miny = centrey-(get_height()/2)*ratio/zoomlevel;//...
      double maxy = centrey+(get_height()/2)*ratio/zoomlevel;//...
      vector<pointbucket*> *pointvector;
      try{
         //Remember to change this to uncachesubset() later!
   //      pointvector = lidardata->advsubset(minx,centrey,maxx,centrey,get_height()*ratio/zoomlevel);//Get data.
         pointvector = lidardata->subset(minx,miny,maxx,maxy);//Get data.
      }catch(const char* e){
         cout << e << endl;
         cout << "No points returned." << endl;
         return false;
      }
      int numbuckets = pointvector->size();
      pointbucket** buckets = new pointbucket*[numbuckets];
      for(int i=0;i<numbuckets;i++){//Convert to pointer for faster access in for loops in image methods. Why? Expect >100000 points.
         buckets[i]=(*pointvector)[i];
      }
      int detail=1;//This determines how many points are skipped between reads, to make drawing faster when zoomed out.
      detail=(int)(numbuckets*maindetailmod);//...
      if(detail<1)detail=1;//...
      interruptthread = false;//New threads should not be immediately interrupted.
      thread_existsmain = true;//No more threads should be made for now.
      Glib::Thread* data_former_thread;
      data_former_thread = Glib::Thread::create(sigc::bind(sigc::mem_fun(*this,&TwoDeeOverview::mainimage),buckets,numbuckets,detail),false);//This thread will interpret the data before telling the main thread to draw.
      delete pointvector;
   }
   else if(imagetype==2){//Draw the preview.
      double minx = centrex-(get_width()/2)*ratio/zoomlevel;//Limits of viewable area:
      double maxx = centrex+(get_width()/2)*ratio/zoomlevel;//...
      double miny = centrey-(get_height()/2)*ratio/zoomlevel;//...
      double maxy = centrey+(get_height()/2)*ratio/zoomlevel;//...
      vector<pointbucket*> *pointvector2;
      try{
         //Remember to change this to uncachesubset() later!
      //      pointvector = lidardata->advsubset(minx,centrey,maxx,centrey,get_height()*ratio/zoomlevel);//Get data.
         pointvector2 = lidardata->subset(minx,miny,maxx,maxy);//Get data.
      }catch(const char* e){
         cout << e << endl;
         cout << "No points returned." << endl;
         return false;
      }
      int numbuckets2 = pointvector2->size();
      pointbucket** buckets2 = new pointbucket*[numbuckets2];
      for(int i=0;i<numbuckets2;i++){//Convert to pointer for faster access in for loops in image methods. Why? Expect >100000 points.
         buckets2[i]=(*pointvector2)[i];
      }
      drawbuckets(buckets2,numbuckets2);
      delete pointvector2;
      delete[] buckets2;
   }
   return true;
}

//Return to initial viewing position.
bool TwoDeeOverview::returntostart(){
   boundary* lidarboundary = lidardata->getboundary();
   double xdif = lidarboundary->maxX-lidarboundary->minX;
   double ydif = lidarboundary->maxY-lidarboundary->minY;
   double xratio = xdif/get_screen()->get_width();//This ratio defines, along with zoomlevel, the translation from world coordinates to window coordinates.
   double yratio = ydif/get_screen()->get_height();//...
   yratio*=1.3;//... (This accounts for some amount of "clutter" at the top and bottom of the screen in the form of taskbars etc.).
   if(xratio>yratio)ratio = xratio;//...
   else ratio = yratio;//...
   centrex = lidarboundary->minX+xdif/2;//Image should be centred at its centre.
   centrey = lidarboundary->minY+ydif/2;//...
   zoomlevel=1;//Back to the starting zoom, which should cause the entire image to be visible.
   resetview();//Update matrices.
   delete lidarboundary;
   return drawviewable(1);
}

//This determines what part of the image is displayed with orthographic projection. It sets the active matrix to that of projection and makes it the identity matrix, and then defines the limits of the viewing area from the dimensions of the window. *ratio/zoomlevel is there to convert window coordinates to world coordinates.
void TwoDeeOverview::resetview(){
   double altitude = rmaxz+5000;
   double depth = rminz-5000;
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-(get_width()/2)*ratio/zoomlevel,
           +(get_width()/2)*ratio/zoomlevel,
           -(get_height()/2)*ratio/zoomlevel,
           +(get_height()/2)*ratio/zoomlevel,
           -5*altitude,-5*depth);
   glMatrixMode(GL_MODELVIEW);//Missing this causes nothing to be visible (???).
   glLoadIdentity();//...
}

//Not totally bug-free:
//This method causes the label below the toolbar to display information about a point that has been selected (using the right mouse button). It starts by setting the label to 0 0 0. It then determines the world coordinates selected from the window coordinates and calls a subset for that. However, assuming that any points are returned at all, an entire bucket's worth of points will be returned. These points are then narrowed down to one point firstly by calling the vetpoints() function to only get the points that lie in or overlap (I think) the square defined by the point's position and the pointsize and zoom position (i.e. the size of the point on the screen). After that, all remaining points are compared and the one on top, the one with the largest z, which would likely be the one the user see or sees mostly, is selected as THE point. Data about the point is extracted from it, with a reference to the quadtree to get the filename. The information displayed is the following: X, Y and Z values; the time; the intensity; the classification; the filename of the file the point is from and the return number.
bool TwoDeeOverview::pointinfo(double eventx,double eventy){
   string meh = "0\n0\n0";
   double pointeroffx = eventx - get_width()/2;//This offset exists because, in world coordinates, 0 is the centre.
   double pointeroffy = eventy - get_height()/2;//...and the same for this one.
   double minx = centrex + (pointeroffx - pointsize/2)*ratio/zoomlevel;//Define an area of equal size to that of the points on the screen.
   double miny = centrey + (-pointeroffy - pointsize/2)*ratio/zoomlevel;//...
   double maxx = centrex + (pointeroffx + pointsize/2)*ratio/zoomlevel;//...
   double maxy = centrey + (-pointeroffy + pointsize/2)*ratio/zoomlevel;//...
   vector<pointbucket*> *pointvector;
   try{
      //Remember to change this to uncachesubset() later!
      pointvector = lidardata->subset(minx,miny,maxx,maxy);//Get data.
   }catch(const char* e){
      cout << e << endl;
      cout << "No points returned." << endl;
      return false;
   }
   if(pointvector->size()>0){//If there aren't any points, don't bother.
      double midx = centrex + pointeroffx * ratio/zoomlevel;//This is needed for the vetpoints() function.
      bool anypoint = false;
      int bucketno=0;
      int pointno=0;
      for(unsigned int i=0;i<pointvector->size();i++){//For every bucket, in case of the uncommon (unlikely?) instances where more than one bucket is returned.
         bool* pointsinarea = vetpoints(pointvector->at(i),midx,miny,midx,maxy,pointsize);//This returns an array of booleans saying whether or not each point (indicated by indices that are shared with pointvector) is in the area prescribed.
         for(int j=0;j<pointvector->at(i)->getnumberofpoints();j++){//For all points...
            if(pointsinarea[j]){//If they are in the right area...
               anypoint = true;
               if(pointvector->at(i)->getpoint(j).z >= pointvector->at(bucketno)->getpoint(pointno).z){//...and if they are higher than the currently selected point
                  bucketno=i;//Select them.
                  pointno=j;//...
               }
            }
         }
         delete pointsinarea;
      }
      string flightline = lidardata->getfilename(pointvector->at(bucketno)->getpoint(pointno).flightline);//Returns the filepath.
      unsigned int index = flightline.rfind("/");//Only the filename is desired, not the filepath.
      if(index==string::npos)index=0;//...
      else index++;//...
      flightline = flightline.substr(index);//...
      ostringstream x,y,z,time,intensity,classification,rnumber;
      x << pointvector->at(bucketno)->getpoint(pointno).x;
      y << pointvector->at(bucketno)->getpoint(pointno).y;
      z << pointvector->at(bucketno)->getpoint(pointno).z;
      time << pointvector->at(bucketno)->getpoint(pointno).time;
      intensity << pointvector->at(bucketno)->getpoint(pointno).intensity;
      classification << (int)pointvector->at(bucketno)->getpoint(pointno).classification;
      rnumber << (int)pointvector->at(bucketno)->getpoint(pointno).rnumber;
      string pointstring = "X: " + x.str() + ", Y: " + y.str() + ", Z:" + z.str() + ", Time: " + time.str() + ",\n" + "Intensity: " + intensity.str() + ", Classification: " + classification.str() + ",\n" + "Flightline: " + flightline + ", Return number: " + rnumber.str() + ".";
      if(anypoint)rulerlabel->set_text(pointstring);
      else rulerlabel->set_text(meh);
   }
   else rulerlabel->set_text(meh);
   delete pointvector;
   return true;
}

//On a left click, this prepares for panning by storing the initial position of the cursor.
bool TwoDeeOverview::on_pan_start(GdkEventButton* event){
   if(event->button==1){
      origpanstartx = panstartx = event->x;
      origpanstarty = panstarty = event->y;
      return true;
   }
   else if(event->button==3)return pointinfo(event->x,event->y);
   else return false;
}
//As the cursor moves while the left button is depressed, the image is dragged along as a preview to reduce lag. The centre point is modified by the negative of the distance (in image units, hence the ratio/zoomlevel mention) the cursor has moved to make a dragging effect and then the current position of the cursor is taken to be the starting position for the next drag (if there is one). The view is then refreshed and then the image is drawn (as a preview).
bool TwoDeeOverview::on_pan(GdkEventMotion* event){
   if((event->state & Gdk::BUTTON1_MASK) == Gdk::BUTTON1_MASK){
      centrex -= (event->x-panstartx)*ratio/zoomlevel;
      centrey += (event->y-panstarty)*ratio/zoomlevel;//Y is reversed because gtk has origin at top left and opengl has it at bottom left.
      panstartx=event->x;
      panstarty=event->y;
      return drawviewable(2);
   }
   else if((event->state & Gdk::BUTTON3_MASK) == Gdk::BUTTON3_MASK)return pointinfo(event->x,event->y);
   else return false;
}
//At the end of the pan draw the full image.
bool TwoDeeOverview::on_pan_end(GdkEventButton* event){
   if(event->button==1){
      origpanstartx=panstartx;
      origpanstarty=panstarty;
      return drawviewable(1);
   }
   else return false;
}

//At the beginning of profiling, defines the start point and, for the moment, the end point of the profile, Prepares the profile box for drawing and then calls the drawing method.
bool TwoDeeOverview::on_prof_start(GdkEventButton* event){
   if(event->button==1){
      profstartx = profendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
      profstarty = profendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
//      profeventstartx = event->x;
//      profeventstarty = event->y;
      return drawviewable(2);
   }
   else if(event->button==3)return pointinfo(event->x,event->y);
   else return false;
}
//Updates the end point of the profile and then gets the vertical and horisontal differences betweent the start and end points. These are used to determine the length of the profile and hence the positions of the vertices of the profile rectangle. Then the drawing method is called.
bool TwoDeeOverview::on_prof(GdkEventMotion* event){
   if((event->state & Gdk::BUTTON1_MASK) == Gdk::BUTTON1_MASK){
      profendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
      profendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
   //   Gdk::Color *col = new Gdk::Color("White");
   //   get_style()->get_fg_gc(get_state())->set_rgb_fg_color(*col);
   //   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   //   if (!glwindow->gl_begin(get_gl_context()))return false;
   //   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   //   else glFlush();
   //   glwindow->gl_end();
   //   get_gl_window()->draw_line(get_style()->get_fg_gc(get_state()),profeventstartx,profeventstarty,event->x,event->y);
   ////   double breadth = profendx - profstartx;
   ////   double height = profendy - profstarty;
   ////   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   ////   double startpointx,startpointy,endpointx,endpointy;
   ////   startpointx = (profstartx-(profwidth/2)*height/length-centrex)*zoomlevel/ratio;
   ////   startpointy = (profstarty+(profwidth/2)*breadth/length-centrey)*zoomlevel/ratio;
   ////   endpointx = (profstartx+(profwidth/2)*height/length-centrex)*zoomlevel/ratio;
   ////   endpointy = (profstarty-(profwidth/2)*breadth/length-centrey)*zoomlevel/ratio;
   ////   get_gl_window()->draw_line(get_style()->get_fg_gc(get_state()),startpointx,startpointy,endpointx,endpointy);
   ////   startpointx = (profstartx+(profwidth/2)*height/length-centrex)*zoomlevel/ratio;
   ////   startpointy = (profstarty-(profwidth/2)*breadth/length-centrey)*zoomlevel/ratio;
   ////   endpointx = (profstartx+(profwidth/2)*height/length-centrex)*zoomlevel/ratio;
   ////   endpointy = (profstarty-(profwidth/2)*breadth/length-centrey)*zoomlevel/ratio;
   ////   get_gl_window()->draw_line(get_style()->get_fg_gc(get_state()),startpointx,startpointy,endpointx,endpointy);
   ////   startpointx = (profstartx+(profwidth/2)*height/length-centrex)*zoomlevel/ratio;
   ////   startpointy = (profstarty-(profwidth/2)*breadth/length-centrey)*zoomlevel/ratio;
   ////   endpointx = (profstartx-(profwidth/2)*height/length-centrex)*zoomlevel/ratio;
   ////   endpointy = (profstarty+(profwidth/2)*breadth/length-centrey)*zoomlevel/ratio;
   ////   get_gl_window()->draw_line(get_style()->get_fg_gc(get_state()),startpointx,startpointy,endpointx,endpointy);
   ////   startpointx = (profstartx-(profwidth/2)*height/length-centrex)*zoomlevel/ratio;
   ////   startpointy = (profstarty+(profwidth/2)*breadth/length-centrey)*zoomlevel/ratio;
   ////   endpointx = (profstartx-(profwidth/2)*height/length-centrex)*zoomlevel/ratio;
   ////   endpointy = (profstarty+(profwidth/2)*breadth/length-centrey)*zoomlevel/ratio;
   ////   get_gl_window()->draw_line(get_style()->get_fg_gc(get_state()),startpointx,startpointy,endpointx,endpointy);
   //   delete col;
   //   return true;
      return drawviewable(2);
   }
   else if((event->state & Gdk::BUTTON3_MASK) == Gdk::BUTTON3_MASK)return pointinfo(event->x,event->y);
   else return false;
}
//Draw the full image at the end of selecting a profile.
bool TwoDeeOverview::on_prof_end(GdkEventButton* event){return drawviewable(2);}
//This makes the box showing the profile area. It calculates the ratio between the length of the profile and its x and y dimensions. It then draws the rectangle.
void TwoDeeOverview::makeprofbox(){
   double breadth = profendx - profstartx;
   double height = profendy - profstarty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   double altitude = rmaxz+1000;//This makes sure the profile box is drawn over the top of the flightlines.
   if(length==0)length=1;
      glColor3f(1.0,1.0,1.0);
      glBegin(GL_LINE_LOOP);
         glVertex3d(profstartx-(profwidth/2)*height/length-centrex,profstarty+(profwidth/2)*breadth/length-centrey,altitude);
         glVertex3d(profstartx+(profwidth/2)*height/length-centrex,profstarty-(profwidth/2)*breadth/length-centrey,altitude);
         glVertex3d(profendx+(profwidth/2)*height/length-centrex,profendy-(profwidth/2)*breadth/length-centrey,altitude);
         glVertex3d(profendx-(profwidth/2)*height/length-centrex,profendy+(profwidth/2)*breadth/length-centrey,altitude);
      glEnd();
}

//Initialises the coordinates of the fence and then draws preview.
bool TwoDeeOverview::on_fence_start(GdkEventButton* event){
   if(event->button==1){
      fencestartx = fenceendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
      fencestarty = fenceendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
   //   fenceeventstartx = event->x;
   //   fenceeventstarty = event->y;
      return drawviewable(2);
   }
   else if(event->button==3)return pointinfo(event->x,event->y);
   else return false;
}
//Updates end coordinates of the fence and then draws preview.
bool TwoDeeOverview::on_fence(GdkEventMotion* event){
   if((event->state & Gdk::BUTTON1_MASK) == Gdk::BUTTON1_MASK){
      fenceendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
      fenceendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
   //   Gdk::Color *col = new Gdk::Color("White");
   //   get_style()->get_fg_gc(get_state())->set_rgb_fg_color(*col);
   //   double boxstartx,boxstarty,boxendx,boxendy;
   //   if(event->x>fenceeventstartx){
   //      boxstartx = fenceeventstartx;
   //      boxendx = event->x;
   //   }
   //   else{
   //      boxstartx = event->x;
   //      boxendx = fenceeventstartx;
   //   }
   //   if(event->y>fenceeventstarty){
   //      boxstarty = fenceeventstarty;
   //      boxendy = event->y;
   //   }
   //   else{
   //      boxstarty = event->y;
   //      boxendy = fenceeventstarty;
   //   }
   //   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   //   if (!glwindow->gl_begin(get_gl_context()))return false;
   //   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   //   else glFlush();
   //   glwindow->gl_end();
   //   get_gl_window()->draw_rectangle(get_style()->get_fg_gc(get_state()),false,boxstartx,boxstarty,boxendx-boxstartx,boxendy-boxstarty);
   //   delete col;
   //   return true;
      return drawviewable(2);
   }
   else if((event->state & Gdk::BUTTON3_MASK) == Gdk::BUTTON3_MASK)return pointinfo(event->x,event->y);
   else return false;
}
//Draws the main image one more.
bool TwoDeeOverview::on_fence_end(GdkEventButton* event){return drawviewable(2);}
//Makes the fence box.
void TwoDeeOverview::makefencebox(){
   double altitude = rmaxz+1000;//This makes sure the fence box is drawn over the top of the flightlines.
      glColor3f(1.0,1.0,1.0);
      glBegin(GL_LINE_LOOP);
         glVertex3d(fencestartx-centrex,fencestarty-centrey,altitude);
         glVertex3d(fencestartx-centrex,fenceendy-centrey,altitude);
         glVertex3d(fenceendx-centrex,fenceendy-centrey,altitude);
         glVertex3d(fenceendx-centrex,fencestarty-centrey,altitude);
      glEnd();
}

//Find the starting coordinates of the ruler and set the label values to zero.
bool TwoDeeOverview::on_ruler_start(GdkEventButton* event){
   if(event->button==1){
      rulerstartx = rulerendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
      rulerstarty = rulerendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
      rulerlabel->set_text("Distance: 0\nX: 0\nY: 0");
      rulereventstartx = event->x;
      rulereventstarty = event->y;
      return drawviewable(2);
   }
   else if(event->button==3)return pointinfo(event->x,event->y);
   else return false;
}
//Find the current cursor coordinates in image terms (as opposed to window/screen terms) and then update the label with the distances. Then draw the ruler.
bool TwoDeeOverview::on_ruler(GdkEventMotion* event){
   if((event->state & Gdk::BUTTON1_MASK) == Gdk::BUTTON1_MASK){
      rulerendx = centrex + (event->x-get_width()/2)*ratio/zoomlevel;
      rulerendy = centrey - (event->y-get_height()/2)*ratio/zoomlevel;
      double d,xd,yd;
      xd = abs(rulerendx-rulerstartx);
      yd = abs(rulerendy-rulerstarty);
      d = sqrt(xd*xd+yd*yd);
      ostringstream dist,xdist,ydist;
      dist << d;
      xdist << xd;
      ydist << yd;
      string rulerstring = "Distance: " + dist.str() +"\nX: " + xdist.str() + "\nY: " + ydist.str();
      rulerlabel->set_text(rulerstring);
   //   Gdk::Color *col = new Gdk::Color("White");
   //   get_style()->get_fg_gc(get_state())->set_rgb_fg_color(*col);
   //   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   //   if (!glwindow->gl_begin(get_gl_context()))return false;
   //   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   //   else glFlush();
   //   glwindow->gl_end();
   //   get_gl_window()->draw_line(get_style()->get_fg_gc(get_state()),rulereventstartx,rulereventstarty,event->x,event->y);
   //   delete col;
   //   return true;
      return drawviewable(2);
   }
   else if((event->state & Gdk::BUTTON3_MASK) == Gdk::BUTTON3_MASK)return pointinfo(event->x,event->y);
   else return false;
}
//Draw again.
bool TwoDeeOverview::on_ruler_end(GdkEventButton* event){return drawviewable(2);}
//Make the ruler as a thick line.
void TwoDeeOverview::makerulerbox(){
   double altitude = rmaxz+1000;//This makes sure the profile box is drawn over the top of the flightlines.
   glColor3f(1.0,1.0,1.0);
   glLineWidth(3);
   glBegin(GL_LINES);
      glVertex3d(rulerstartx-centrex,rulerstarty-centrey,altitude);
      glVertex3d(rulerendx-centrex,rulerendy-centrey,altitude);
   glEnd();
   glLineWidth(1);
}

//(Anything that happens for centrex is reversed for centrey). First, half the distance between the centre of the window and the window position of the event is converted to image coordinates and added to the image centre. This is analogous to moving the centre to where the event occured. Then, depending on the direction of the scroll, the zoomlevel is increased or decreased. Then the centre is moved to where the centre of the window will now lie. The image is then drawn.
bool TwoDeeOverview::on_zoom(GdkEventScroll* event){
   centrex += (event->x-get_width()/2)*ratio/zoomlevel;
   centrey -= (event->y-get_height()/2)*ratio/zoomlevel;//Y is reversed because gtk has origin at top left and opengl has it at bottom left.
   if(zoomlevel>=1){
      if(event->direction==GDK_SCROLL_UP)zoomlevel+=pow(zoomlevel,zoompower)/2;
      else if(event->direction==GDK_SCROLL_DOWN)zoomlevel-=pow(zoomlevel,zoompower)/2;
   }
   else if(zoomlevel>=0.2){
      if(event->direction==GDK_SCROLL_UP)zoomlevel+=0.1;
      else if(event->direction==GDK_SCROLL_DOWN)zoomlevel-=0.1;
   }
   else if(event->direction==GDK_SCROLL_UP)zoomlevel+=0.1;
   if(zoomlevel<0.2)zoomlevel=0.2;
   centrex -= (event->x-get_width()/2)*ratio/zoomlevel;
   centrey += (event->y-get_height()/2)*ratio/zoomlevel;//Y is reversed because gtk has origin at top left and opengl has it at bottom left.
   resetview();
   return drawviewable(1);
}

void TwoDeeOverview::clippy(string picturename){//Annoy the user with an easter egg.
   get_gl_window()->draw_pixbuf(get_style()->get_fg_gc(get_state()),Gdk::Pixbuf::create_from_file(picturename),0,0,0,0,-1,-1,Gdk::RGB_DITHER_NONE,0,0);
}
