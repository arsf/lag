/* 
 * File:   loader.h
 * Author: chrfi
 *
 * Created on October 9, 2009, 11:09 AM
 */

#ifndef _QUADLOADER_H
#define	_QUADLOADER_H

#include "quadtreestructs.h"
#include <stdlib.h>

class quadtree;

// abstract class for all lidar point loading classes
class quadloader
{
public:
   // this method is expected to load ever nth point from the filename passed into the 
   // quadtree passed
   virtual bool load(const char *filename, quadtree *qt, int nth) = 0;
   
   // this method is expected to load ever nth point from the filename passed into the 
   // quadtree passed if they fall within the area of interest defined by the xy points
   virtual bool load(const char *filename, quadtree *qt, int nth, double minX, double minY, double maxX, double maxY) = 0;
   
   
   // this method is expected to return true or false based on wether
   // this implemented version of this class can load the passed file
   virtual bool compatibleCheck(const char *filename) = 0;
  
   // this method is expected to return a boundary object representing the
   // data from the file header
   
   // NOTE : this makes sal cry, you can't declare a virtual static method
   // therefore i can't enforce that this is replaced by implementing classes
   // however if anyone adds code that dosen't override this i will exact my revenge... slowly
   virtual boundary* getboundary(const char *filename){return NULL;};
};



#endif	/* _QUADLOADER_H */

