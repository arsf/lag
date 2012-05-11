/*
==================================
 ProfileWorker.h

  Created on: 10 May 2012
      Author: jaho
==================================
*/

#ifndef PROFILEWORKER_H_
#define PROFILEWORKER_H_

#include <gtkmm.h>

class Profile;

class ProfileWorker
{
public:
	ProfileWorker(Profile* prof, double* xs, double* ys, int ps);

	~ProfileWorker();

	void start()
	{
		thread = Glib::Thread::create(sigc::mem_fun(*this, &ProfileWorker::run), true);
	}

	Glib::Dispatcher sig_done;

protected:
	void run();

	Profile* profile;
	double* profxs;
	double* profys;
	int profps;

	Glib::Thread* thread;
	Glib::Mutex mutex;
	bool stop;
};


#endif /* PROFILEWORKER_H_ */
