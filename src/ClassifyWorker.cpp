/*
===============================================================================

 ClassifyWorker.cpp

 Created on: 11 May 2012
 Authors: jaho, Berin Smaldon

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

#include "ClassifyWorker.h"
#include "Profile.h"


/*
==================================
 ClassifyWorker::ClassifyWorker
==================================
*/
ClassifyWorker::ClassifyWorker(Profile* prof) : Worker(),
		profile  (prof),
      stopFlag (false)
{
}

/*
================================================================================
 ClassifyWorker::~ClassifyWorker
================================================================================
*/
ClassifyWorker::~ClassifyWorker()
{
   stop();
}

/*
================================================================================
 ClassifyWorker::stop
================================================================================
*/
void ClassifyWorker::stop()
{
   Glib::Mutex::Lock internal_lock (internal_mutex);

   stopFlag = true;
   classify_condition.signal();
}

/*
================================================================================
 ClassifyWorker::nudge

 Indicates to this thread that jobs are/could be available
================================================================================
*/
void ClassifyWorker::nudge()
{
   classify_condition.signal();
}

/*
================================================================================
 ClassifyWorker::run
================================================================================
*/
void ClassifyWorker::run()
{
   Glib::Mutex::Lock internal_lock (internal_mutex);

   // internal copies
   ClassificationJob thisjob = NULL;
   FenceType thisfence;

   while (!stopFlag)
   {
      for ( thisJob = popNextClassify(); thisJob == NULL || stopFlag;
            thisJob = popNextClassify())
         classify_condition.wait(internal_mutex);

      if (!stopFlag)
      {
         thisfence = thisjob.first;

            internal_lock.release();
         profile->classify(thisfence.first, thisfence.second, thisjob.second);
            internal_lock.acquire();

         sig_done();
      }
   }
}
