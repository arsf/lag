/*
 ===============================================================================

 Worker.h

 Created on: 24 Apr 2012
 Authors: Jan Holownia, Berin Smaldon

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

#ifndef WORKER_H_
#define WORKER_H_

#include <gtkmm.h>

/*
 ===============================================================================

 Worker - a generic worker class.

 ===============================================================================
 */
class Worker
{
   public:
      Worker() :
            thread(0), stopped(false)
      {
      }
      
      virtual ~Worker()
      {
         {
            Glib::Mutex::Lock lock(mutex);
            stopped = true;
         }
         this->join();
      }
      
      void start()
      {
         thread = Glib::Thread::create(sigc::mem_fun(*this, &Worker::run), true);
      }
      
      void join()
      {
         if(thread)
            thread->join();
         thread = 0;
      }
      
      Glib::Dispatcher sig_done;

   protected:
      virtual void run() = 0;

      Glib::Thread* thread;
      Glib::Mutex mutex;
      bool stopped;
};

#endif /* WORKER_H_ */
