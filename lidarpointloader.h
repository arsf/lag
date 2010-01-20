/* 
 * File:   LASloader.h
 * Author: chrfi
 *
 * Created on November 23, 2009, 10:31 AM
 */

#ifndef _LIDARPOINTLOADER_H
#define	_LIDARPOINTLOADER_H


#include "quadtreestructs.h"


class lidarpointloader
{
public:
   // this method is expected to take the number of points to pull out, the number of points
   // to skip, an array, and four values defining an area. it then pulls out the number of points, 
   // checks them against the area and then puts them into the array. it returns the number of points
   // it pulls out, this should be equal to n except when the method hits the end of the file
   virtual int load(int n, int nth, point *points, int flightlinenum, double minX, double minY, double maxX, double maxY) = 0;
   // as above but pulls every point selected (still skips nth)
   virtual int load(int n, int nth, point *points, int flightlinenum) = 0;

   // WARNING: placeholder
   virtual bool compatibleCheck(const char *filename) = 0;

   virtual boundary* getboundary() = 0;
   
   virtual const char* getfilename() = 0;
};

#endif	/* _LIDARPOINTLOADER_H */

