/* 
 * File:   ASCIIloader.h
 * Author: chrfi
 *
 * Created on November 25, 2009, 1:04 PM
 */

#ifndef _ASCIILOADER_H
#define	_ASCIILOADER_H


#include "quadtreestructs.h"
#include <fstream>
#include "lidarpointloader.h"

using namespace std;

/**
 * This class handles the loading of ascci lidar point files.
 *
 * it uses a format string to determin the format of the ascii file.
 * the first character of this string is used as the deliminator each subsequent character represents the data held in the corrisponding column.
 * the characters that can be used are as follows
 * 'x' X cartiesian value<br>
 * 'y' Y cartiesian value<br>
 * 'z' Z cartiesian value<br>
 * 'i' intensity<br>
 * 'c' classification<br>
 * 'r' return number<br>
 * 't' time<br>
 * 's' scanangle<br>
 * 'd' scan direction flag<br>
 * 'n' number of returns<br>
 * 'e' edge of scan flag<br>
 * 'p' point source id<br>
 * '#' ignore (any column that does not fall into the above catagorys must be marked ignore)<br>
 *
 * for example:<br>
 * " txyzicr#"<br>
 * means that the file contains columns deliminated by a space and in the order<br>
 * time X Y Z intensity classification returnnumber ignore
 */
class ASCIIloader : public lidarpointloader
{
   const char *formatstring;
   const char *filename;
   ifstream ifs;
   boundary *b;
   FILE *fp;
public:
   int linecounter;
   
   /**
    * a constructor.
    * @param filename the path of the file to be associated with this loader
    * @param formatstring a string containing the format of the ascii file, the first character is taken as the deliminator
    */
   ASCIIloader(const char *filename, const char *formatstring);

   /**
    * a destructor
    */
   ~ASCIIloader();

   /**
    * a method to load points from the file.
    * this method takes argurments that cause it to load points seperated by a defined number of points into the provided array that are within a specific area.
    * @note caution, in general this method is called by quadtree and is not used by the user.
    * @param n the number of points to load
    * @param nth the number of points to skip between loaded points
    * @param points the array to copy the loaded points into (should have n elements)
    * @param flightlinenum the number assigned to every point loaded as its flight line number
    * @param Xs an array of doubles each of which is the x componant of a point of the polygon (in sequence)
    * @param Ys an array of doubles each of which is the x componant of a point of the polygon (in sequence)
    * @param size of the Xs and Ys arrays (therefore the number of points that define the polygon)
    * @return the number of points actually loaded (either the same as n or if the end of the file is reached less)
    */
   int load(int n, int nth, point *points, int flightlinenum, double *Xs, double *Ys, int size);


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


#endif	/* _ASCIILOADER_H */

