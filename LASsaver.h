/* 
 * File:   LASsaver.h
 * Author: chrfi
 *
 * Created on April 16, 2010, 3:38 PM
 */

#ifndef _LASSAVER_H
#define	_LASSAVER_H

#include "liblas/laspoint.hpp"
#include "liblas/laswriter.hpp"
#include "quadtreestructs.h"
#include <fstream>
#include "LASsaver.h"
#include "lidarpointsaver.h"

using namespace std;

/*
 * this class implements the interface provided in lidarpointsave
 */
class LASsaver : public lidarpointsaver
{
    const char *outputfilename;
    const char *inputfilename;
    ofstream ofs;
    ifstream ifs;

    double maxX, maxY, maxZ, minX, minY, minZ;
    int numofreturn;
    int *numofeachreturn;


    liblas::LASWriter *writer;
    liblas::LASReader *reader;
    liblas::LASHeader oldheader;
    liblas::LASHeader newheader;

public:

    /**
     * a constructor for the class
     *
     * @param outputfilename the absolute or relative path of the file the points will be saved to
     * @param inputfilename the absolute or relative path of the file that the points to be saved came from (used to copy the header across)
     */
    LASsaver(const char *outputfilename, const char *inputfilename);

    /**
     * the method saves the array of points to a file in the LAS format
     *
     * @param n the number of points in the array
     * @param points the array of points to be saved
     */
    void savepoints(int n, point *points);

    /**
     * the method uses values calculated during the point saving to save an updated header
     */
    void finalizesave();

};

#endif	/* _LASSAVER_H */

