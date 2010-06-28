/* 
 * File:   LASsaver.h
 * Author: chrfi
 *
 * Created on April 16, 2010, 3:38 PM
 */

#ifndef _LASSAVER_H
#define	_LASSAVER_H


#include "quadtreestructs.h"
#include "LasSaver.h"
#include "LidarPointSaver.h"

#include <fstream>

#include "liblas/laspoint.hpp"
#include "liblas/laswriter.hpp"

using namespace std;

/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * this class implements the interface provided in lidarpointsave to handle saving to the las format
 */
class LasSaver : public LidarPointSaver
{
public:
    /**
     * a constructor for the class
     *
     * @param outputFileName the absolute or relative path of the file the points will be saved to
     * @param inputFileName the absolute or relative path of the file that the points to be saved came from (used to copy the header across)
     */
    LasSaver(const char *outputFileName, const char *inputFileName);

    ~LasSaver();

    /**
     * the method saves the array of points to a file in the LAS format
     *
     * @param n the number of points in the array
     * @param points the array of points to be saved
     */
    void savepoints(int n, Point *points);

private:
    const char *outputFileName_;
    const char *inputFileName_;
    ofstream ofs_;
    ifstream ifs_;

    double maxX_, maxY_, maxZ_, minX_, minY_, minZ_;
    int numOfReturn_;
    int *numOfEachReturn_;

    liblas::LASWriter *writer_;
    liblas::LASReader *reader_;
    liblas::LASHeader oldHeader_;
    liblas::LASHeader newHeader_;
};

#endif	/* _LASSAVER_H */

