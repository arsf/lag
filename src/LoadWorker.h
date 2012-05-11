/*
==================================
 LoadWorker.h

  Created on: 24 Apr 2012
      Author: jaho
==================================
*/

#ifndef LOADWORKER_H_
#define LOADWORKER_H_


#include <gtkmm.h>
#include "SelectionBox.h"


class FileOpener;

class LoadWorker
{
public:
	LoadWorker(FileOpener* fo, int point_offset, std::vector<std::string> filenames, bool s, bool usearea, int resolutiondepth, int resolutionbase,
			int bucketlevels, int bucketlimit, int cachelimit, bool default_scale_factors, double scale_factor[3],
			std::string ascii_code, SelectionBox fence);

	~LoadWorker();

	void start()
	{
		thread = Glib::Thread::create(sigc::mem_fun(*this, &LoadWorker::run), true);
	}

	Glib::Dispatcher sig_done;
	Glib::Dispatcher sig_message;
	Glib::Dispatcher sig_file_loaded;

protected:
	void run();

	FileOpener* fileopener;
	int point_offset;
	std::vector<std::string> filenames;
	bool create_new_quadtree;
	bool usearea;
	int resolutiondepth;
	int resolutionbase;
	int bucketLevels;
	int bucketlimit;
	int cachelimit;
	bool use_default_scale_factors;
	double scale_factor[3];
	std::string ascii_code;
	SelectionBox fence;

	Glib::Thread* thread;
	Glib::Mutex mutex;
	bool stop;

};


#endif /* LOADWORKER_H_ */
