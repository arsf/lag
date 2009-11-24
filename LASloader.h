/* 
 * File:   newloader.h
 * Author: chrfi
 *
 * Created on November 20, 2009, 12:21 PM
 */

#ifndef _NEWLOADER_H
#define	_NEWLOADER_H

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
   
   // see quadloader.h
   LASloader(const char *filename);
   ~LASloader();
   int load(int n, int nth, point *points, double minX, double minY, double maxX, double maxY);
   int load(int n, int nth, point *points);
   // see quadloader.h
   // WARNING: placeholder
   bool compatibleCheck(const char *filename){return false;};
   // see quadloader.h
   boundary* getboundary();
};

#endif	/* _NEWLOADER_H */

