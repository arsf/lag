/* 
 * File:   teststruct.h
 * Author: chrfi
 *
 * Created on October 8, 2009, 11:06 AM
 */

#ifndef _TESTSTRUCT_H
#define	_TESTSTRUCT_H

#include <stdint.h>

// this struct contains the data for a single lidar point
struct point{
    double x;
    double y;
    double z;
    double time;
    unsigned short int intensity;
    // we could possibly but won't use one 
    //byte for both classification and return number
    uint8_t classification;
    uint8_t flightline;
    uint8_t rnumber;
    
};


// this struct is a simple data transfer struct for the
// boundary values of a node
struct boundary{
   double minX;
   double minY;
   double maxX;
   double maxY;
};
  

// this class is used to store the array of points in a node
// it is neccessary because the array must be accompanied by 
// the number of points actually contained.
class pointbucket{
public:
   int numberofpoints;
   unsigned short int minintensity,maxintensity;
   double minz,maxz;
   point *points;
   double minx,miny,maxx,maxy;

   pointbucket(int cap, double minx,  double miny,  double maxx,  double maxy);
   ~pointbucket();
};

#endif	/* _TESTSTRUCT_H */

