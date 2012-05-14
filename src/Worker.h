/*
==================================
 Worker.h

  Created on: 24 Apr 2012
      Author: jaho
==================================
*/

#ifndef WORKER_H_
#define WORKER_H_

#include <gtkmm.h>


class Worker
{
public:
	Worker() :
		thread(0),
		stop(false)
	{}

	virtual ~Worker()
	{
		{
			Glib::Mutex::Lock lock (mutex);
			stop = true;
		}
		if (thread)
			thread->join();
	}

	void start()
	{
		thread = Glib::Thread::create(sigc::mem_fun(*this, &Worker::run), true);
	}

	Glib::Dispatcher sig_done;

protected:
	virtual void run() = 0;

	Glib::Thread* thread;
	Glib::Mutex mutex;
	bool stop;
};


#endif /* WORKER_H_ */
