/* 
 * File:   newloader.h
 * Author: chrfi
 *
 * Created on November 20, 2009, 12:21 PM
 */

#ifndef _LASLOADER_H
#define	_LASLOADER_H


#include "quadtreestructs.h"
#include "LidarPointLoader.h"

#include <fstream>

#include "liblas/laspoint.hpp"
#include "liblas/lasreader.hpp"

using namespace std;

/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * this class handles the loading of points from .LAS files.
 */
class LASloader : public LidarPointLoader
{
public:
    /**
     * a constructor
     * @param fileName the path of the file to be associated with this loader
     */
    LASloader(const char *fileName);

    /**
     * a deconstructor
     */
    ~LASloader();

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
     * @return the boundary of the flight line
     */
    Boundary* getBoundary();

    /**
     * a method to get the filename of this loaders associated file
     * @return associated filename
     */
    const char* getFileName();

private:
    const char *fileName_;
    ifstream ifs_;
    liblas::LASReader *reader_;
    liblas::LASHeader header_;
};

#endif	/* _LASLOADER_H */

