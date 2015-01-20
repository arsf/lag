/*
 ===============================================================================

 PriofileWorker.h

 Created on: 10 May 2012
 Author: Jan Holownia

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

#ifndef PROFILEWORKER_H_
#define PROFILEWORKER_H_

#include "Worker.h"

class Profile;

/*
 ===============================================================================

 ProfileWorker - a worker class for loading points into the profile.

 ===============================================================================
 */
class ProfileWorker: public Worker
{
   public:
      ProfileWorker(Profile* prof, std::vector<double> xs, std::vector<double> ys, int ps);

   protected:
      void run();

      Profile* profile;
      std::vector<double> profxs;
      std::vector<double> profys;
      int profps;
};

#endif /* PROFILEWORKER_H_ */
