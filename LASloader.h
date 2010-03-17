/* 
 * File:   newloader.h
 * Author: chrfi
 *
 * Created on November 20, 2009, 12:21 PM
 */

#ifndef _LASLOADER_H
#define	_LASLOADER_H

#include "liblas/laspoint.hpp"
#include "liblas/lasreader.hpp"
#include "quadtreestructs.h"
#include <fstream>
#include "lidarpointloader.h"

using namespace std;

class LASloader : public lidarpointloader
{
   const char *filename;
   ifstream ifs;
   liblas::LASReader *reader;
   liblas::LASHeader  header;
public:   
   /**
    * a constructor.
    * @param filename the path of the file to be associated with this loader
    */
   LASloader(const char *filename);

   /**
    * a deconstructor
    */
   ~LASloader();

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
   int load(int n, int nth, point *points, int flightlinenum, double minX, double minY, double maxX, double maxY);

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
   int load(int n, int nth, point *points, int flightlinenum);
   // WARNING: placeholder

   /**
    * this is a placeholder and should not be used
    */
   bool compatibleCheck(const char *filename){return false;};

   /**
    * a method to get the boundary of the points contained in the file that this loader is associated with
    * @return the boundary of the flight line
    */
   boundary* getboundary();

   /**
    * a method to get the filename of this loaders associated file
    * @return associated filename
    */
   const char* getfilename();
};

#endif	/* _LASLOADER_H */

