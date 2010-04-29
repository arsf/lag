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
    cacheminder(int cachesize);
    ~cacheminder();

    /**
     * a method to request some cache space
     *
     * @param requestsize the number of points requested
     * @pbucket a pointer to the bucket asking for the cache (used for uncache calls)
     * @force true=remove other buckets from cache to make room.
     *
     * @return true=space found and allocated. only possible to return false if force=false
     */
    bool requestcache(int requestsize, pointbucket *pbucket, bool force);

    /**
     * a method to release the cache being used by a bucket
     *
     * @param release the amount of cache to be realse (MUST match the amount used by the bucket)
     * @param pbucket the bucket whose cache is to be freed
     */
    void releasecache(int release, pointbucket *pbucket);

    /**
     * multithreaded cache, ignore
     */
    void cachelist(vector<pointbucket*> *bucketlist);
    /**
     * multithreaded cache, ignore
     */
    void setcachetodo(vector<pointbucket *> *tasks);
    /**
     * multithreaded cache, ignore
     */
    void pushcachetodo(vector<pointbucket *> *tasks);
    /**
     * multithreaded cache, ignore
     */
    void popcachetodo();
    /**
     * multithreaded cache, ignore
     */
    void deletecachetodo(vector<pointbucket *> *tasks);
    /**
     * multithreaded cache, ignore
     */
    void clearcachetodo();
    /**
     * multithreaded cache, ignore
     */
    void cachethread();
    /**
     * multithreaded cache, ignore
     */
    void startcachethread();
    /**
     * multithreaded cache, ignore
     */
    void stopcachethread();

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

