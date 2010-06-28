/* 
 * File:   cacheminder.h
 * Author: chrfi
 *
 * Created on January 26, 2010, 3:57 PM
 */


#include "quadtreestructs.h"
#include "PointBucket.h"

#include <queue>

using namespace std;

#ifndef _CACHEMINDER_H
#define	_CACHEMINDER_H

/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * this class keeps track of the number of points that are currently in main memory (cached).
 * it dosen't track specific memory locations just the overal memory usage.
 *
 * a normal quadtree will normally have a single cacheminder that tracks the memory usage for
 * all the buckets within the quadtree.
 */
class CacheMinder
{
public:
    /**
     * a constructer
     *
     * @param cacheSize the number of points that can be cache at any one time
     */
    CacheMinder(int cacheSize);

    /**
     * a destructer
     */
    ~CacheMinder();

    /**
     * a method to update the amount of cache being used by a bucket. if the amount of
     * free cache is insufficant buckets will be uncached untill enough space is free.
     *
     * @note updateing the cache used by a bucket to zero does not remove it from the cache list
     * to remove a bucket from cache completely call release cache.
     * @param requestSize the number of points to increase (or if negative decrease) the number of cache points by
     * @param pointBucket a pointer to the bucket asking for the cache (used for uncache calls)
     * @param force true=remove other buckets from cache to make room.
     *
     * @return true=space found and allocated. only possible to return false if force=false
     */
    bool updateCache(int requestSize, PointBucket *pointBucket, bool force);

    /**
     * a method to release the cache being used by a bucket and remove the bucket from the cached list
     *
     * @note this can only be called on a bucket that has been cached. Additionally
     * this method must be called to completely remove a bucket from cache, even if
     * updatecache has been used to reduce the bucket's cache usage to zero
     * @param releaseSize the amount of cache to be realse (MUST match the amount used by the bucket)
     * @param pointBucket the bucket whose cache is to be freed
     */
    void releaseCache(int releaseSize, PointBucket *pointBucket);

    /**
     * a method that removes all buckets from cache
     */
    void clearCache();

private:
    deque<PointBucket*> bucketsInCache_;
    int cacheUsed_;
    int totalCache_;
    bool cacheing_;
};

#endif	/* _CACHEMINDER_H */

