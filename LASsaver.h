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

    LASsaver(const char *outputfilename, const char *inputfilename);

    void savepoints(int n, point *points);

    void finalizesave();

};

#endif	/* _LASSAVER_H */

