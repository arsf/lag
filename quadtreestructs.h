/* 
 * File:   quadtreestructs.h
 * Author: chrfi
 *
 * Created on October 8, 2009, 11:06 AM
 */

#ifndef _QUADTREESTRUCTS_H
#define	_QUADTREESTRUCTS_H

#include <stdint.h>
#include <stdio.h>






// mask values for packedbyte
#define returnnumber 7
#define numberofreturn 56
#define scandirectionflag 64
#define scanedgeflag 128



// this struct contains the data for a single lidar point
struct point
{
    double x;
    double y;
    double z;
    double time;
    unsigned short int intensity;
    uint8_t classification;
    uint8_t flightline;
    // a byte containing returnnumber, numberofreturns, scandirectionflag and scanedgeflag
    uint8_t packedbyte;
    uint8_t scanangle;
    unsigned short int pointsourceid;
};


// this struct is a simple data transfer struct for the
// boundary values of a node
struct boundary
{
    double minX;
    double minY;
    double maxX;
    double maxY;
};







#endif	/* _QUADTREESTRUCTS_H */

