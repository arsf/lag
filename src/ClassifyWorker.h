/*
==================================
 ClassifyWorker.h

  Created on: 11 May 2012
      Author: jaho
==================================
*/

#ifndef CLASSIFYWORKER_H_
#define CLASSIFYWORKER_H_

#include <gtkmm.h>
#include <stdint.h>

class Profile;

class ClassifyWorker
{
public:
	ClassifyWorker(Profile* prof, uint8_t clas);

	~ClassifyWorker();

	void start()
	{
		thread = Glib::Thread::create(sigc::mem_fun(*this, &ClassifyWorker::run), true);
	}

	Glib::Dispatcher sig_done;

protected:
	void run();

	Profile* profile;
	uint8_t classification;

	Glib::Thread* thread;
	Glib::Mutex mutex;
	bool stop;

};


#endif /* CLASSIFYWORKER_H_ */
