/*
 ===============================================================================

 PriofileWorker.cpp

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

#include "ProfileWorker.h"
#include "Profile.h"

/*
 ==================================
 ProfileWorker::ProfileWorker
 ==================================
 */
ProfileWorker::ProfileWorker(Profile* prof, vector<double> xs, vector<double> ys, int ps) :
      Worker(), profile(prof), profxs(xs), profys(ys), profps(ps)
{
}

/*
 ==================================
 ProfileWorker::run
 ==================================
 */
void ProfileWorker::run()
{
   // Improve this
   profile->loadprofile(profxs, profys, profps);
   sig_done();
}
