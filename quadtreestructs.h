/* 
 * File:   teststruct.h
 * Author: chrfi
 *
 * Created on October 8, 2009, 11:06 AM
 */

#ifndef _TESTSTRUCT_H
#define	_TESTSTRUCT_H

#include <stdint.h>
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"
#include "boost/thread.hpp"
#include <stdio.h>
#include <string>
#include <stdlib.h>



// this struct contains the data for a single lidar point

struct point
{
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


private:
    friend class boost::serialization::access;
    // this method provides the serlization logic (order, both in and out)
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & x;
        ar & y;
        ar & z;
        ar & time;
        ar & intensity;
        ar & classification;
        ar & flightline;
        ar & rnumber;
    }
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

// this class is used to split the serilizable points in a pointbucket from the meta data
// it is simple a wrapper round an array to provide some basic information to the serialization methods
class SerializableInnerBucket
{
public:
    point *points;
    int size;
    int numpoints;
    int increase;

    SerializableInnerBucket(){};

    SerializableInnerBucket(int initialsize, int increase)
    {
        points = (point*) malloc(initialsize * sizeof(point));
        size = initialsize;
        numpoints = 0;
        this->increase = increase;
    };

    ~SerializableInnerBucket()
    {
        if (points != NULL)
        {
            free(points);
        }
    };

    inline void setpoint(point& newP)
    {
        if (numpoints == size)
        {
            size+=increase;
            points = (point*) realloc(points, size * sizeof(point));
        }
        points[numpoints]=newP;
        numpoints++;
    }

private:
    friend class boost::serialization::access;

    // the serialization methods are split as the actions do not mirror each other
    // both use the length value to only write the minimum number of points to disk
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar & increase;
        ar & numpoints;
        ar & size;
        for (int k=0; k<numpoints; k++)
        {
            ar & points[k];
        }
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        ar & increase;
        ar & numpoints;
        ar & size;
        if (size != 0)
        {
            points = new point[size];

            for (int k = 0; k < numpoints; k++)
            {
                ar & points[k];
            }
        }
    }

    // this tells the serialization librarys that the serilization method is split into save() and load()
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};





#endif	/* _TESTSTRUCT_H */

