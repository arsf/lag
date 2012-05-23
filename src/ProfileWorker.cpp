/*
==================================
 ProfileWorker.cpp

  Created on: 10 May 2012
      Author: jaho
==================================
*/

#include "ProfileWorker.h"
#include "Profile.h"


ProfileWorker::ProfileWorker(Profile* prof, double* xs, double* ys, int ps) : Worker(),
		profile		(prof),
		profxs		(xs),
		profys		(ys),
		profps		(ps)
{}


void ProfileWorker::run()
{
	profile->loadprofile(profxs,profys,profps);
	sig_done();
}
