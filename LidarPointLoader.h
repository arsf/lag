/* 
 * File:   LASloader.h
 * Author: chrfi
 *
 * Created on November 23, 2009, 10:31 AM
 */

#ifndef _LIDARPOINTLOADER_H
#define	_LIDARPOINTLOADER_H


#include "quadtreestructs.h"

/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * This interface defines how classes which provide loading logic for lidar points must look
 * the interface allows the quadtree to simply take any subclass and call its methods without understanding how the loading is being done
 * this means that the logic for loading different formats is encapsulated in classes independant of the quadtree
 */
class LidarPointLoader
{
public:
   /**
    * a method to load points from the file.
    * this method takes argurments that cause it to load points seperated by a defined number of points into the provided array that are within a specific area defined by convex polygon.
    * @note caution, in general this method is called by quadtree and is not used by the user.
    * @param n the number of points to load
    * @param nth the number of points to skip between loaded points
    * @param points the array to copy the loaded points into (should have n elements)
    * @param flightLineNumber the number assigned to every point loaded as its flight line number
     * @param horizontalCornerValues an array of doubles each of which is the x componant of a point of the polygon (in sequence)
     * @param verticalCornerValues an array of doubles each of which is the y componant of a point of the polygon (in sequence)
     * @param size the number of corners that make up the polygon (the length of the HorizontalCornerValues and VerticalCornerValues arrays)
    * @return the number of points actually loaded (either the same as n or if the end of the file is reached less)
    */
   virtual int load(int n, int nth, Point *points, int flightLineNumber, double *horizontalCornerValues, double *verticalCornerValues, int size) = 0;


   /**
    * a method to load points from the file.
    * this method takes argurments that cause it to load points seperated by a defined number of points into the provided array.
    * @note caution, in general this method is called by quadtree and is not used by the user.
    * @param n the number of points to load
    * @param nth the number of points to skip between loaded points
    * @param points the array to copy the loaded points into (should have n elements)
    * @param flightLineNumber the number assigned to every point loaded as its flight line number
    * @return the number of points actually loaded (either the same as n or if the end of the file is reached less)
    */
   virtual int load(int n, int nth, Point *points, int flightLineNumber) = 0;

   /**
    * this is a placeholder and should not be used
    */
   virtual bool compatibleCheck(const char *fileName) = 0;

   /**
    * a method to get the boundary of the points contained in the file that this loader is associated with
    * @return the boundary of the flight line
    */
   virtual Boundary* getBoundary() = 0;

   /**
    * a method to get the filename of this loaders associated file
    * @return associated filename
    */
   virtual const char* getFileName() = 0;
};

#endif	/* _LIDARPOINTLOADER_H */

