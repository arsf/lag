/* 
 * File:   loader.h
 * Author: chrfi
 *
 * Created on October 9, 2009, 11:09 AM
 */

#ifndef _QUADLOADER_H
#define	_QUADLOADER_H

#include "quadtreestructs.h"

class quadtree;

// abstract class for all lidar point loading classes
class quadloader
{
public:
   // this method is expected to load ever nth point from the filename passed into the 
   // quadtree passed
   virtual bool load(const char *filename, quadtree *qt, int nth) = 0;
   // this method is expected to return true or false based on wether
   // this implemented version of this class can load the passed file
   virtual bool compatibleCheck(const char *filename) = 0;
   // this method is expected to return a boundary object representing the
   // data from the file header
   virtual boundary* getboundary(const char *filename) = 0;
};



#endif	/* _QUADLOADER_H */

