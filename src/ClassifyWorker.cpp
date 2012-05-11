/*
==================================
 ClassifyWorker.cpp

  Created on: 11 May 2012
      Author: jaho
==================================
*/

#include "ClassifyWorker.h"
#include "Profile.h"

ClassifyWorker::ClassifyWorker(Profile* prof, uint8_t clas) : Worker(),
		profile	(prof),
		classification (clas)
{}

void ClassifyWorker::run()
{
	{
		Glib::Mutex::Lock lock (mutex);
		profile->classify(classification);
	}

	sig_done();
}
