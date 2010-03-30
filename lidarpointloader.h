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
   /**
    * a method to load points from the file.
    * this method takes argurments that cause it to load points seperated by a defined number of points into the provided array that are within a specific area.
    * @note caution, in general this method is called by quadtree and is not used by the user.
    * @param n the number of points to load
    * @param nth the number of points to skip between loaded points
    * @param points the array to copy the loaded points into (should have n elements)
    * @param flightlinenum the number assigned to every point loaded as its flight line number
    * @param minX X value of the lower left corner of the area
    * @param minY Y value of the lower left corner of the area
    * @param maxX X value of the upper right corner of the area
    * @param maxY Y value of the upper right corner of the area
    * @return the number of points actually loaded (either the same as n or if the end of the file is reached less)
    */
   virtual int load(int n, int nth, point *points, int flightlinenum, double minX, double minY, double maxX, double maxY) = 0;


   virtual int load(int n, int nth, point *points, int flightlinenum, double *Xs, double *Ys, int size) = 0;


   /**
    * a method to load points from the file.
    * this method takes argurments that cause it to load points seperated by a defined number of points into the provided array.
    * @note caution, in general this method is called by quadtree and is not used by the user.
    * @param n the number of points to load
    * @param nth the number of points to skip between loaded points
    * @param points the array to copy the loaded points into (should have n elements)
    * @param flightlinenum the number assigned to every point loaded as its flight line number
    * @return the number of points actually loaded (either the same as n or if the end of the file is reached less)
    */
   virtual int load(int n, int nth, point *points, int flightlinenum) = 0;

   /**
    * this is a placeholder and should not be used
    */
   virtual bool compatibleCheck(const char *filename) = 0;

   /**
    * a method to get the boundary of the points contained in the file that this loader is associated with
    * @return the boundary of the flight line
    */
   virtual boundary* getboundary() = 0;

   /**
    * a method to get the filename of this loaders associated file
    * @return associated filename
    */
   virtual const char* getfilename() = 0;
};

#endif	/* _LIDARPOINTLOADER_H */

