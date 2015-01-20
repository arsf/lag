/*
 ===============================================================================

 ClassifyWorker.h

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

#ifndef CLASSIFYWORKER_H_
#define CLASSIFYWORKER_H_

#include <stdint.h>
#include "Worker.h"
#include "ProfileTypes.h"

class Profile;

/*
 ===============================================================================

 ClassifyWorker - a worker class for points classification.

 ===============================================================================
 */
class ClassifyWorker: public Worker
{
   public:
      ClassifyWorker(Profile* prof);
      ~ClassifyWorker();
      void nudge();
      void stop();

      ClassificationJob getCurrentJob();

   protected:
      void run();

   private:
      Profile* profile;
      ClassificationJob currentjob;
      bool stopFlag;

      // Threading control
      Glib::Cond classify_condition;
      Glib::Mutex internal_mutex;
};

#endif /* CLASSIFYWORKER_H_ */
