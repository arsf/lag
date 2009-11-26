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

class ASCIIloader : public lidarpointloader
{
   const char *formatstring;
   const char *filename;
   ifstream ifs;
   boundary *b;
   FILE *fp;
public:
   int linecounter;
   
   // see lidarpointloader.h
   ASCIIloader(const char *filename, const char *formatstring);
   ~ASCIIloader();
   int load(int n, int nth, point *points, double minX, double minY, double maxX, double maxY);
   int load(int n, int nth, point *points);
   // WARNING: placeholder
   bool compatibleCheck(const char *filename){return false;};
   boundary* getboundary();
};


#endif	/* _ASCIILOADER_H */

