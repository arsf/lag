/*
==================================
 ClassifyWorker.cpp

  Created on: 11 May 2012
      Author: jaho
==================================
*/

#include "ClassifyWorker.h"
#include "Profile.h"

ClassifyWorker::ClassifyWorker(Profile* prof, uint8_t clas) :
		profile	(prof),
		classification (clas)
{}


ClassifyWorker::~ClassifyWorker()
{
	{
		Glib::Mutex::Lock lock (mutex);
		stop = true;
	}
	if (thread)
		thread->join();
}


void ClassifyWorker::run()
{
	profile->classify(classification);
	sig_done();
}
