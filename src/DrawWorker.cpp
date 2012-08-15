/*
================================================================================

 DrawWorker.cpp

 Created on: 1 August 2012
 Authors: Berin Smaldon

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

================================================================================
*/

#include "DrawWorker.h"
#include "Profile.h"

/*
================================================================================
 DrawWorker::DrawWorker

 Parameters:
   disp  - The LagDisplay object that this thread is to be associated with
================================================================================
*/
DrawWorker::DrawWorker(LagDisplay* disp) : Worker(),
      display  (disp),
      goFlag   (false),
      stopFlag (false),
      drawing  (false)
{
}

/*
================================================================================
 DrawWorker::~DrawWorker
================================================================================
*/
DrawWorker::~DrawWorker()
{
   if (thread)
   {
      stop();
      this->~Worker();
   }
}

/*
================================================================================
 DrawWorker::stop

 Stops this drawing thread from executing as soon as possible, and may wake
 it up to do so.
================================================================================
*/
void DrawWorker::stop()
{
   Glib::Mutex::Lock internal_lock (internal_mutex);

   stopFlag = true;

   // unlock GL to make sure the thread is able to proceed without deadlock
   display->abortFrame(true);

   goFlag = true;
   draw_frame.signal();
}

/*
================================================================================
 DrawWorker::run

 Used internally by all Worker objects, main function of execution, thread
 terminates on completion.
================================================================================
*/
void DrawWorker::run()
{
   // Initialisation now that Glib is set up
   PointBucket** copyof_buckets;
   int copyof_numbuckets;

   Glib::Mutex::Lock internal_lock (internal_mutex);

   stopFlag = false;
   drawing = false;

   while (!stopFlag)
   {
      // awaits another process to give the signal to proceed
      while (!goFlag)
         draw_frame.wait(internal_mutex);
      goFlag = false;
      drawing = true;

      // per-thread copies, safe to call display->mainimage with without the
      // internal mutex being locked
      copyof_buckets    = internal_buckets;
      copyof_numbuckets = internal_numbuckets;

      // aborting is now futile, since buckets and numbuckets are as up-to-date
      // as they can be
      display->clear_abortFrame();

      if (!stopFlag)
      {
         internal_lock.release();
         display->mainimage(copyof_buckets, copyof_numbuckets);
         internal_lock.acquire();

         drawing = goFlag;
      }

      sig_done();
   }
}

/*
================================================================================
 DrawWorker::draw

 Notes down what values to draw when the next frame happens (which can be any
 time in the future), and may trigger the next frame, but not necessarily

 Parameters:
   buckets     - Buckets to draw the main image with
   numbuckets  - Count of buckets in given array
================================================================================
*/
void DrawWorker::draw(PointBucket** buckets, int numbuckets)
{
   // attain exclusive rights to internal_buckets and internal_numbuckets
   Glib::Mutex::Lock lock (internal_mutex);

   internal_buckets = buckets;
   internal_numbuckets = numbuckets;

   goFlag = true;
   draw_frame.signal();
}

/*
================================================================================
 DrawWorker::isDrawing

 Returns whether or not the thread is presently drawing. Accurate at exactly
 the moment of use, but only for an undefined period of time (due to threading
 policies in the kernel) - use sparingly if at all.
================================================================================
*/
bool DrawWorker::isDrawing()
{
   Glib::Mutex::Lock lock (internal_mutex);

   return drawing;
}
