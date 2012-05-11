/*
==================================
 ProfileWorker.h

  Created on: 10 May 2012
      Author: jaho
==================================
*/

#ifndef PROFILEWORKER_H_
#define PROFILEWORKER_H_


#include "Worker.h"

class Profile;

class ProfileWorker : public Worker
{
public:
	ProfileWorker(Profile* prof, double* xs, double* ys, int ps);

protected:
	void run();

	Profile* profile;
	double* profxs;
	double* profys;
	int profps;
};


#endif /* PROFILEWORKER_H_ */
