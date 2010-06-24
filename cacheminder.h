/* 
 * File:   cacheminder.h
 * Author: chrfi
 *
 * Created on January 26, 2010, 3:57 PM
 */

#include <queue>
#include "quadtreestructs.h"
#include "pointbucket.h"
#include "boost/thread.hpp"

using namespace std;

#ifndef _CACHEMINDER_H
#define	_CACHEMINDER_H


/**
 * this class keeps track of the number of points that are currently in main memory (cached).
 * it dosen't track specific memory locations just the overal memory usage.
 *
 * a normal quadtree will normally have a single cacheminder that tracks the memory usage for
 * all the buckets within the quadtree.
 */
class cacheminder {
public:
    /**
     * a constructer
     *
     * @param cachesize the number of points that can be cache at any one time
     */
    cacheminder(int cachesize);

    /**
     * a destructer
     */
    ~cacheminder();

    /**
     * a method to update the amount of cache being used by a bucket. if the amount of
     * free cache is insufficant buckets will be uncached untill enough space is free.
     *
     * @note updateing the cache used by a bucket to zero does not remove it from the cache list
     * to remove a bucket from cache completely call release cache.
     * @param requestsize the number of points to increase (or if negative decrease) the number of cache points by
     * @param pbucket a pointer to the bucket asking for the cache (used for uncache calls)
     * @param force true=remove other buckets from cache to make room.
     *
     * @return true=space found and allocated. only possible to return false if force=false
     */
    bool updatecache(int requestsize, pointbucket *pbucket, bool force);

    /**
     * a method to release the cache being used by a bucket and remove the bucket from the cached list
     *
     * @note this can only be called on a bucket that has been cached. Additionally
     * this method must be called to completely remove a bucket from cache, even if
     * updatecache has been used to reduce the bucket's cache usage to zero
     * @param release the amount of cache to be realse (MUST match the amount used by the bucket)
     * @param pbucket the bucket whose cache is to be freed
     */
    void releasecache(int release, pointbucket *pbucket);

    

    /**
     * a method that removes all buckets from cache
     */
    void clearcache();

private:
    deque<pointbucket*> bucketsincache;
    int cacheused;
    int totalcache;
    deque<pointbucket *> *cachetodo;
    boost::recursive_mutex quemutex;
    boost::recursive_mutex todomutex;
    boost::thread thread1;
    bool cacheing;
};

#endif	/* _CACHEMINDER_H */

