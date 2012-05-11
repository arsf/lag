/*
==================================
 ClassifyWorker.h

  Created on: 11 May 2012
      Author: jaho
==================================
*/

#ifndef CLASSIFYWORKER_H_
#define CLASSIFYWORKER_H_


#include <stdint.h>
#include "Worker.h"


class Profile;

class ClassifyWorker : public Worker
{
public:
	ClassifyWorker(Profile* prof, uint8_t clas);

protected:
	void run();

	Profile* profile;
	uint8_t classification;

};


#endif /* CLASSIFYWORKER_H_ */
