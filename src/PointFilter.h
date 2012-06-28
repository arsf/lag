/*
==================================
 PointFilter.h

  Created on: 27 Jun 2012
      Author: jaho
==================================
*/

#ifndef POINTFILTER_H_
#define POINTFILTER_H_

struct PointFilter
{
	PointFilter(): argc(0) {};
	~PointFilter() {};
	int argc;
	std::vector<std::string> args;
};

#endif /* POINTFILTER_H_ */
