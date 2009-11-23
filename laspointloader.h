/* 
 * File:   LASloader.h
 * Author: chrfi
 *
 * Created on November 23, 2009, 10:31 AM
 */

#ifndef _LASLOADER_H
#define	_LASLOADER_H


#include "quadtreestructs.h"

 // see quadloader.h
class laspointloader
{
public:
   virtual int load(int n, int nth, point *points, double minX, double minY, double maxX, double maxY) = 0;
   virtual int load(int n, int nth, point *points) = 0;
   // see quadloader.h
   // WARNING: placeholder
   virtual bool compatibleCheck(const char *filename) = 0;
   // see quadloader.h
   virtual boundary* getboundary() = 0;
};

#endif	/* _LASLOADER_H */

