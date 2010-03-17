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


    int numberofpoints;


    unsigned short int minintensity, maxintensity;
    double minZ, maxZ;
    double minX, minY, maxX, maxY;
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

public:
    /**
     *  constructer which initilizes the capacity of the bucket along with the boundary from
     * parameters and the other varibles to defaults
     *
     * @param cap the number of points the bucket can hold
     * @param
     * @param minX X value of the lower left corner of the boundary
     * @param minY Y value of the lower left corner of the boundary
     * @param maxX X value of the upper right corner of the boundary
     * @param maxY Y value of the upper right corner of the boundary
     * @param MCP the cacheminder for this quadtree instance
     * @param instanceddirectory string containing a path to a directory where temporary files will be saved
     */
    pointbucket(int cap, double minX, double minY, double maxX, double maxY, cacheminder *MCP, string instancedirectory);
   
    /**
     * deconstructor
     */
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

    void setpoint(point& newP)
    {
        if (!incache)
        {
            cache(true);

        }

        if (numberofpoints == 0)
         {
            maxintensity = newP.intensity;
            minintensity = newP.intensity;
            maxZ = newP.z;
            minZ = newP.z;
         }
         if (newP.intensity > maxintensity)
         {
            maxintensity = newP.intensity;
         }
         if (newP.intensity < minintensity)
         {
            minintensity = newP.intensity;
         }
         if (newP.z > maxZ)
         {
            maxZ = newP.z;
         }
         if (newP.z < minZ)
         {
            minZ = newP.z;
         }

        if (innerbucket->size == innerbucket->numpoints)
        {
            if(!increasecache(true, innerbucket->increase))
            {
                throw ramallocationexception("failed to acquire extra ram to allow more points to be inserted");
            }
            innerbucket->setpoint(newP);
            innerbucketsize = innerbucket->size;
            numberofpoints++;
            
        }


        innerbucket->setpoint(newP);
        numberofpoints++;
        return;
    }

    inline int getnumberofpoints() const
    {
        return numberofpoints;
    }

    inline double getmaxX() const
    {
        return maxX;
    }

    inline double getmaxY() const
    {
        return maxY;
    }

    inline double getmaxZ() const
    {
        return maxZ;
    }

    inline double getminX() const
    {
        return minX;
    }

    inline double getminY() const
    {
        return minY;
    }

    inline double getminZ() const
    {
        return minZ;
    }

    bool isincache() const
    {
        return incache;
    }

    unsigned short int getmaxintensity() const
    {
        return maxintensity;
    }

    unsigned short int getminintensity() const
    {
        return minintensity;
    }
};


#endif	/* _POINTBUCKET_H */

