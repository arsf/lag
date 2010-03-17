/* 
 * File:   pointbucket.h
 * Author: chrfi
 *
 * Created on February 11, 2010, 12:08 PM
 */

#ifndef _POINTBUCKET_H
#define	_POINTBUCKET_H

#include <stdint.h>
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"
#include "boost/thread.hpp"
#include <stdio.h>
#include <string>
#include "quadtreestructs.h"
#include "quadtreeexceptions.h"

using namespace std;

class cacheminder;

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
    int numberofserializedpoints;
    bool serialized;
    bool incache;
    int cap;
    string filepath;
    SerializableInnerBucket *innerbucket;
    boost::recursive_mutex cachemutex;
    string instancedirectory;
    int innerbucketsize;


    // constructer which initilizes the capacity of the bucket along with the boundary from
    // parameters and the other varibles to defaults
    pointbucket(int cap, double minx, double miny, double maxx, double maxy, cacheminder *MCP, string instancedirectory);
    ~pointbucket();

    // the uncache method removes the associated SerializableInnerBucket and writes
    // it to secondary memory if neccessary, it then informs the cacheminder that the memory has been freed
    void uncache();

    // the cache method requests some space in main memory and then loads the SerializableInnerBucket into it.
    // this is only done if the SerializableInnerBucket is not already in cache.
    // the parameter "force" defines wether the another bucket can be forced out of cache to accomodate this one
    // if space cannot be found false is returned
    bool cache(bool force);
    bool increasecache(bool force, int i);

    // the getpoint method adds a layer between outside classes and the SerializableInnerBucket. this prevents
    // outside classes from accessing the SerializableInnerBucket without the pointbuckets knowledge. This
    // is important as the SerializableInnerBucket may not be cached. by providing this method all access to
    // SerializableInnerBucket prompts the pointbucket to check if its cached and cache if neccessary.
    inline point& getpoint(int i)
    {
        if (incache)
        {
            return innerbucket->points[i];
        }
        else
        {
            cache(true);
            return innerbucket->points[i];
        }
    }

    inline void setpoint(point& newP)
    {
        if (!incache)
        {
            cache(true);

        }
        if (innerbucket->size == innerbucket->numpoints)
        {
            if(!increasecache(true, innerbucket->increase))
            {
                throw ramallocationexception("failed to acquire extra ram to allow more points to be inserted");
            }
            innerbucket->setpoint(newP);
            innerbucketsize = innerbucket->size;
            return;
        }

        innerbucket->setpoint(newP);
        return;
    }

};


#endif	/* _POINTBUCKET_H */

