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


/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * A data container class representing a single lidar point
 */
class Point
{
public:
    double x;
    double y;
    double z;
    double time;
    unsigned short int intensity;
    uint8_t classification;
    uint8_t flightLine;
    // a byte containing returnnumber, numberofreturns, scandirectionflag and scanedgeflag
    uint8_t packedByte;
    uint8_t scanAngle;
    unsigned short int pointSourceId;
};


/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * a data container class sotring a boundary(min and max XY values
 */
class Boundary
{
public:
    double minX;
    double minY;
    double maxX;
    double maxY;
};







#endif	/* _QUADTREESTRUCTS_H */

