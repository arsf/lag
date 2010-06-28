/* 
 * File:   ASCIIloader.h
 * Author: chrfi
 *
 * Created on November 25, 2009, 1:04 PM
 */

#ifndef _ASCIILOADER_H
#define	_ASCIILOADER_H


#include "quadtreestructs.h"
#include "LidarPointLoader.h"

#include <fstream>

using namespace std;

/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
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
class AsciiLoader : public LidarPointLoader
{
public:

    /**
     * a constructor.
     * @param fileName the path of the file to be associated with this loader
     * @param formatString a string containing the format of the ascii file, the first character is taken as the deliminator
     */
    AsciiLoader(const char *fileName, const char *formatString);

    /**
     * a destructor
     */
    ~AsciiLoader();



    /**
     * a method to load points in a fence from the file.
     * this method takes argurments that cause it to load points seperated by a defined number of points into the provided array that are within a specific area.
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
    int load(int n, int nth, Point *points, int flightLineNumber, double *horizontalCornerValues, double *verticalCornerValues, int size);


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
    int load(int n, int nth, Point *points, int flightLineNumber);

    /**
     * this is a placeholder and should not be used
     */
    bool compatibleCheck(const char *fileName)
    {
        return false;
    };

    /**
     * a method to get the boundary of the points contained in the file that this loader is associated with
     * @return the boundary of the flight line in a boundary object
     */
    Boundary* getBoundary();

    /**
     * a method to get the filename of this loaders associated file
     * @return associated filename
     */
    const char* getFileName();

private:
    const char *formatString_;
    const char *fileName_;
    ifstream ifs_;
    Boundary *boundary_;
    FILE *file_;
};


#endif	/* _ASCIILOADER_H */

