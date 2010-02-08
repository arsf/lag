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
    int length;

    SerializableInnerBucket() { };

    SerializableInnerBucket(int cap)
    {
        points = new point[cap];
        size = cap;
        length = 0;
    };

    ~SerializableInnerBucket()
    {
        if (points != NULL)
        {
            delete[] points;
        }
    };

private:
    friend class boost::serialization::access;

    // the serialization methods are split as the actions do not mirror each other
    // both use the length value to only write the minimum number of points to disk
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

    // this tells the serialization librarys that the serilization method is split into save() and load()
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
    double minx, miny, maxx, maxy;
    cacheminder *MCP;
    int numberofcachedpoints;
    bool serialized;
    bool incache;
    int cap;
    char serialfile[512];
    SerializableInnerBucket *b;
    boost::recursive_mutex cachemutex;


    // constructer which initilizes the capacity of the bucket along with the boundary from
    // parameters and the other varibles to defaults
    pointbucket(int cap, double minx, double miny, double maxx, double maxy, cacheminder *MCP);
    ~pointbucket();

    // the uncache method removes the associated SerializableInnerBucket and writes
    // it to secondary memory if neccessary, it then informs the cacheminder that the memory has been freed
    void uncache();

    // the cache method requests some space in main memory and then loads the SerializableInnerBucket into it.
    // this is only done if the SerializableInnerBucket is not already in cache.
    // the parameter "force" defines wether the another bucket can be forced out of cache to accomodate this one
    // if space cannot be found false is returned
    bool cache(bool force);


    // the getpoint method adds a layer between outside classes and the SerializableInnerBucket. this prevents
    // outside classes from accessing the SerializableInnerBucket without the pointbuckets knowledge. This
    // is important as the SerializableInnerBucket may not be cached. by providing this method all access to
    // SerializableInnerBucket prompts the pointbucket to check if its cached and cache if neccessary.
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

