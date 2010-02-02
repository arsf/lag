/* 
 * File:   teststruct.h
 * Author: chrfi
 *
 * Created on October 8, 2009, 11:06 AM
 */

#ifndef _TESTSTRUCT_H
#define	_TESTSTRUCT_H

#include <stdint.h>
#include "/users/rsg/chrfi/boost_install/include/boost/archive/binary_oarchive.hpp"
#include "/users/rsg/chrfi/boost_install/include/boost/archive/binary_iarchive.hpp"
#include "boost/thread.hpp"

class cacheminder;

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

class bucket
{
public:
    point *points;
    int size;
    int length;

    bucket() { };

    bucket(int cap)
    {
        points = new point[cap];
        size = cap;
        length = 0;
    };

    ~bucket()
    {
        if (points != NULL)
        {
            delete[] points;
        }
    };

private:
    friend class boost::serialization::access;

    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar & length;
        ar & size;
        for (int k=0; k<length; k++)
        {
            ar & points[k];
        }
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        ar & length;
        ar & size;
        if (size != 0)
        {
            points = new point[size];

            for (int k = 0; k < length; k++)
            {
                ar & points[k];
            }
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
};


// this class is used to store the array of points in a node
// it is neccessary because the array must be accompanied by 
// the number of points actually contained.

class pointbucket
{
public:
    int numberofpoints;
    unsigned short int minintensity, maxintensity;
    double minz, maxz;
    point *points;
    double minx, miny, maxx, maxy;
    cacheminder *MCP;
    int numberofcachedpoints;
    char serialfile[512];
    bool serialized;
    bool incache;
    int cap;
    bucket *b;
    boost::recursive_mutex cachemutex;

    pointbucket(int cap, double minx, double miny, double maxx, double maxy, cacheminder *MCP);
    ~pointbucket();

    void uncache();
    bool cache(bool force);

    inline point& getpoint(int i)
    {
        if (incache)
        {
            return b->points[i];
        }
        else
        {
            cache(true);
            return b->points[i];
        }
    }

};


#endif	/* _TESTSTRUCT_H */

