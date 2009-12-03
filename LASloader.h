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
   int linecounter;
   
   // see lidarpointloader.h
   LASloader(const char *filename);
   ~LASloader();
   int load(int n, int nth, point *points, int flightlinenum, double minX, double minY, double maxX, double maxY);
   int load(int n, int nth, point *points, int flightlinenum);
   // WARNING: placeholder
   bool compatibleCheck(const char *filename){return false;};
   boundary* getboundary();
};

#endif	/* _LASLOADER_H */

