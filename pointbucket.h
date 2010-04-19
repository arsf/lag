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
#include <stdint.h>

using namespace std;

class cacheminder;

/**
 * this class represents a bucket which holds a colletion of points. it stores
 * metadata about the points it holds and manages the caching and uncaching of the points
 *
 * while to other classes it appears that this class contains the points it infact dosen't.
 * it has a pointer to a serializableinnerbucket which is a lightweight serilizable class which
 * contains the point array. When the pointbucket caches or uncaches the points it infact remains in memory
 * so that the meta data is always available and it is just the serilizableinnerbucket that is cached and uncached.
 */
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
    boost::recursive_mutex getmutex;
    string instancedirectory;
    int innerbucketsize;
    bool updated;

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

    /**
     * a method which removes the associated SerializableInnerBucket and writes
     * it to secondary memory if neccessary, it then informs the cacheminder that the memory has been freed
     */
    void uncache();

    /**
     * a method that requests some space in main memory and then loads the point data from secondary memory to main memory into it.
     * this is only done if the pointdata is not already in cache.
     *
     * @note this requests space equal to the current size of the serilizableinnerbucket
     *
     * @param force this boolean indicates wether other buckets should be forced out of main memory to make space
     * 
     * @return true=memory assigned use it
     */
    bool cache(bool force);

    /**
     * a method that requests some space in main memory
     *
     * @note this requests space equal to i. because cache is only freed by the uncache method which frees
     * an amount based on the size of serilizableinnerbucket the increasecache method should only be used
     * to increase the cache usage to the same level as the serilizableinnerbucket size when the size has increased after it was cached.
     * (in other words be very very very carefull using this method)
     *
     * @param force this boolean indicates wether other buckets should be forced out of main memory to make space
     *
     * @return true=memory assigned use it
     */
    bool increasecache(bool force, int i);

    /**
     * a method that adds a layer between outside classes and the SerializableInnerBucket. this prevents
     * outside classes from accessing the SerializableInnerBucket without the pointbuckets knowledge. This
     * is important as the SerializableInnerBucket may not be cached. by providing this method all access to
     * SerializableInnerBucket prompts the pointbucket to check if its cached and cache if neccessary.
     *
     * @param i the index of the point to get
     *
     * @return a reference to the desired point
     */
    inline point& getpoint(int i)
    {
        //boost::recursive_mutex::scoped_lock mylock(getmutex);
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

    /**
     * a method to allow the classification of a point in this bucket to be set to a desired value,
     * this method provides an interface between users and the points which insures the points are cached
     * before they are manipulated
     *
     * @param i the index of the point to set
     * @param classification the new classification value of the point
     */
    inline void setclassification(int i, uint8_t classification)
    {
        if(incache)
        {
            innerbucket->points[i].classification = classification;
        }
        else
        {
            cache(true);
            innerbucket->points[i].classification = classification;
        }
        updated = true;
    }




    /**
     * a method to add a new point to the serializableinnerbucket.
     * it also checks the new point and updates the min and max values accordingly
     *
     * @param newP the new point to be added
     */
    void setpoint(point& newP);


    void generateraster(double width);

    // getters
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

