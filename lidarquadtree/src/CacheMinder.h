/* 
 * File:   cacheminder.h
 * Authors: chrfi, jaho
 *
 * Created on January 26, 2010, 3:57 PM
 *
 * ----------------------------------------------------------------
 *
 * This file is part of lidarquadtree, a library providing a data
 * structure for storing and indexing LiDAR points.
 *
 * Copyright (C) 2008-2012 Plymouth Marin Laboratory
 *    arsfinternal@pml.ac.uk
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * The GNU General Public License is contained in the file COPYING. *
 */

#ifndef _CACHEMINDER_H
#define	_CACHEMINDER_H

#include "PointBucket.h"
#include <queue>

using namespace std;


/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * this class keeps track of the number of points that are currently in main 
 * memory (cached).
 * it dosen't track specific memory locations just the overal memory usage.
 *
 * a normal quadtree will normally have a single cacheminder that tracks the 
 * memory usage for all the buckets within the quadtree.
 *
 * @warning The CacheMinder makes no attempt to keep track of the cache used by
 * individual pointbuckets, therefore the pointbuckets must ensure that they
 * request and free the same amount otherwise things go badly fast (equivalent
 * of a memory leak)
 */
class CacheMinder
{
public:
    /**
     * a constructor
     *
     * @param cacheSize the number of points that can be cache at any one time
     */
    CacheMinder(int cacheSize);

    /**
     * a destructor
     */
    ~CacheMinder();

    /**
     * a method to update the amount of cache being used by a bucket. if the 
     * amount of
     * free cache is insufficient buckets will be uncached until enough space
     * is free.
     *
     * @note updating the cache used by a bucket to zero does not remove it
     * from the cache list
     * to remove a bucket from cache completely call release cache.
     * @param requestSize the number of points to increase (or if negative 
     * decrease) the number of cache points by
     * @param pointBucket a pointer to the bucket asking for the cache (used for 
     * uncache calls)
     * @param force true=remove other buckets from cache to make room.
     *
     * @return true=space found and allocated. only possible to return false if 
     * force=false
     */
    bool updateCache(int requestSize, PointBucket *pointBucket, bool force);

    /**
     * A method to release the cache being used by a bucket and remove the 
     * bucket from the cached list.
     * this should only be called if your really really sure that you have 
     * already freed the ram
     *
     * @warning as this is dealing with keeping a count of ram it is 
     * imperitive that
     * any object calling releasecache gets its sums right, if your cumulative 
     * cacheUpdates and releases don't
     * match up all hell will break loose.
     *
     * @note this can only be called on a bucket that has been cached. 
     * Additionally
     * this method must be called to completely remove a bucket from cache, 
     * even if
     * updatecache has been used to reduce the bucket's cache usage to zero
     *
     * @param releaseSize the amount of cache to be realse (MUST match the 
     * amount used by the bucket)
     * @param pointBucket the bucket whose cache is to be freed
     */
    void releaseCache(int releaseSize, PointBucket *pointBucket);

    /**
     * a method that removes all buckets from cache
     */
    void clearCache();

    int get_cacheused()
    {
    	return this->cacheUsed_;
    }

protected:
    deque<PointBucket*> bucketsInCache_;
    int cacheUsed_;
    int totalCache_;
    bool cacheing_;
};

#endif	/* _CACHEMINDER_H */

