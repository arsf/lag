/*
==================================
 ProfileWorker.cpp

  Created on: 10 May 2012
      Author: jaho
==================================
*/

#include "ProfileWorker.h"
#include "Profile.h"

ProfileWorker::ProfileWorker(Profile* prof, double* xs, double* ys, int ps, bool changeview) :
		profile		(prof),
		profxs		(xs),
		profys		(ys),
		profps		(ps),
		changeview	(changeview)
{}

ProfileWorker::~ProfileWorker()
{
	{
		Glib::Mutex::Lock lock (mutex);
		stop = true;
	}
	if (thread)
		thread->join();
}

void ProfileWorker::run()
{
	profile->showprofile(profxs,profys,profps,changeview);
	sig_done();
}
