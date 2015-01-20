/*
 ================================================================================

 DrawWorker.h

 Created on: 1 August 2012
 Author: Berin Smaldon

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

#ifndef DRAWWORKER_H_
#define DRAWWORKER_H_

#include <PointBucket.h>
#include "Worker.h"

class LagDisplay;

/*
 ================================================================================

 DrawWorker - a worker class for drawing lag information in gtkglextmm viewports

 ================================================================================
 */
class DrawWorker: public Worker
{
   public:
      DrawWorker(LagDisplay* disp);

      ~DrawWorker();

      void stop();
      void run();

      void draw(PointBucket** buckets, int numbuckets);

      bool isDrawing();

   private:
      bool goFlag;
      bool stopFlag;
      bool drawing;

      LagDisplay* display;

      // arguments for draw
      PointBucket** internal_buckets;
      int internal_numbuckets;

      // Signal to indicate to the drawing thread to wake up and check the goFlag
      // to proceed to draw a frame
      Glib::Cond draw_frame;

      // Exclusive access to internal values
      Glib::Mutex internal_mutex;
};

#endif /* DRAWWORKER_H_ */
