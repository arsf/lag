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
#include "lzo/lzo1b.h"

using namespace std;

class cacheminder;

/**
 * this class represents a bucket which holds a colletion of points. it stores
 * metadata about the points it holds and manages the caching and uncaching of the points
 * <br>
 * The pointbucket class can hold several subsets of the points that fall within it
 * depending on arguements when it is constructed. The point bucket always stores a full list
 * of points but also possibly several progressivly smaller subsets. these are defined by a base number and a
 * number of levels so that base 10 levels 4 results in 4 subsets
 * 10^0:every point, the second 10^1:every 10th point, 10^2:every 100th point, 10^3: every 1000th point
 * points that fall into multiple subsets will be present in all those subsets so the first point always falls into
 * every subset.<br>
 * <br>
 * This is mostly hidden from other classes
 * except when getting a point or the number of points both of which depend on the resolution desired.
 *
 * @note because the point bucket holds several sets of data of the same type many of its attributes
 * are stored in arrays with each index holding the value that relates to a different subset.
 */
class pointbucket
{
    unsigned short int minintensity, maxintensity;
    double minZ, maxZ;
    double minX, minY, maxX, maxY;
    cacheminder *MCP;
    int splitvalue;
    int numberofsplitlevels;   
    int cap;    
    string instancedirectory;
    
    // arrays containing different values for these attributes for each subset of points
    int *numberofpoints;
    string *filepath;
    bool *updated;
    int *numberofserializedpoints;
    bool *serialized;
    bool *incache;
    int *pointarraysize;
    int increaseamount;
    point **points;
    lzo_uint *compresseddatasize;
    
    

public:

    static unsigned char * workingmemory;
    static unsigned char * compresseddata;
    static long o_counter;
    static long i_counter;
    /**
     *  constructer which initilizes the capacity of the bucket along with the boundary from
     * parameters and the other varibles to defaults.
     *
     *
     * @param cap the number of points the bucket can hold
     * @param minX X value of the lower left corner of the boundary
     * @param minY Y value of the lower left corner of the boundary
     * @param maxX X value of the upper right corner of the boundary
     * @param maxY Y value of the upper right corner of the boundary
     * @param MCP the cacheminder for this quadtree instance
     * @param instanceddirectory string containing a path to a directory where temporary files will be saved
     * @param resolutionbase the base number for subset calculation (see class description for more detail)
     * @param numresolutionlevels the number of resolution levels (see class description for more detail)
     */
    pointbucket(int cap, double minX, double minY, double maxX, double maxY, cacheminder *MCP, string instancedirectory, int resolutionbase, int numresolutionlevels);
   
    /**
     * deconstructor
     */
    ~pointbucket();

    /**
     * a method which removes the smallest cached subset and writes
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
     * @param resolution indicates which subset to cache
     * 
     * @return true=memory assigned use it
     */
    bool cache(bool force, int resolution);

    

    /**
     * a method that adds a layer between outside classes and the SerializableInnerBucket. this prevents
     * outside classes from accessing the subset array without the pointbuckets knowledge. This
     * is important as the subset may not be cached. by providing this method all access to
     * the subset array prompts the pointbucket to check if its cached and cache if neccessary.
     *
     * @param i the index of the point to get
     * @param resolution index of resolution level to get the point from (0 to (the number of levels-1))
     *
     * @return a reference to the desired point
     */
    inline point& getpoint(int i, int resolution)
    {
        //boost::recursive_mutex::scoped_lock mylock(getmutex);
        if (incache[resolution])
        {
            return points[resolution][i];
        }
        else
        {
            cache(true, resolution);
            return points[resolution][i];
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
    void setclassification(int i, uint8_t classification);
    




    /**
     * a method to add a new point to the serializableinnerbucket.
     * it also checks the new point and updates the min and max values accordingly
     *
     * @param newP the new point to be added
     */
    void setpoint(point& newP);




    // getters
    /**
     * returns the number of points in the specified resolution bucket
     *
     * @param resolution the index of the resolution level
     */
    inline int getnumberofpoints(int resolution) const
    {
        if (resolution > numberofsplitlevels)
        {
            throw "resolution index out of bounds";
        }
        return numberofpoints[resolution];
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

