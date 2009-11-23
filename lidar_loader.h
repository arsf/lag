/* 
 * File:   lidar_loader.h
 * Author: chrfi
 *
 * Created on October 12, 2009, 4:06 PM
 */

#ifndef _LIDAR_LOADER_H
#define	_LIDAR_LOADER_H

#include "quadloader.h"

class quadtree;
class lidar_loader: public quadloader
{
public:
   int linecounter;
   // see quadloader.h
   bool load(const char *filename, quadtree *qt, int nth);
   bool load(const char *filename, quadtree *qt, int nth, double minX, double minY, double maxX, double maxY);
   // see quadloader.h
   // WARNING: placeholder
   bool compatibleCheck(const char *filename){return false;};
   // see quadloader.h
   virtual boundary* getboundary(const char *filename);
};

#endif	/* _LIDAR_LOADER_H */

