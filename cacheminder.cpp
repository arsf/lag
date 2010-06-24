/* 
 * File:   cacheminder.cpp
 * Author: chrfi
 * 
 * Created on January 26, 2010, 3:57 PM
 */

#include "cacheminder.h"

#include <deque>
#include "quadtreeexceptions.h"

cacheminder::cacheminder(int cachesize)
{
    totalcache = cachesize;
    cacheused = 0;
    cachetodo = new deque<pointbucket *>;
    cacheing = false;
}

// WANRING!!!: it is imperitive that the requestsize is correct and is always
// identical to the relasesize given to the releasecache method below
// if they don't match up things go downhill fast, see releasecache for justification

void cacheminder::clearcache()
{
    while (!bucketsincache.empty())
    {
        bucketsincache.front()->uncache();
    }
}

bool cacheminder::updatecache(int requestsize, pointbucket *pbucket, bool force)
{
    //boost::recursive_mutex::scoped_lock mylock(quemutex);
    if (requestsize > totalcache)
    {
        throw ramallocationexception("request has been made to cache minder asking for more ram in a single block than available in total");
    }

    deque<pointbucket *>::iterator ity;
    for(ity=bucketsincache.begin(); ity<bucketsincache.end(); ity++)
    {
        if (*ity == pbucket)
        {
            bucketsincache.erase(ity);
            break;
        }
    }

    if (force)
    {
        // if not enough free cache remove buckets from cache in queue order till there is space
        while (cacheused + requestsize > totalcache)
        {
            bucketsincache.front()->uncache();
            cacheing = true;
        }
    }
    else if ((cacheused + requestsize > totalcache))
    {   
        return false;
    }

   // cout << "cacheing" << endl;

    bucketsincache.push_back(pbucket);
    cacheused = cacheused + requestsize;
   // cout << cacheused << endl;
   // cout << bucketsincache.size() << endl;
    return true;


    // remove some buckets from cache to make space
    // if this is the buckets first request for cache give it a filename
    //
}

// this should only be called if your really really sure that you have already freed the ram
// WARNING!!!: as this is dealing with keeping a count of ram it is imperitive that
// any object calling releasecache gets its sums right, if your request and release don't
// math up all hell will break loose
// NOTE: this has been done so that the buckets decide there ram size not the minder, this allows
// different types of buckets to be used or buckets containing different amounts of data without
// the minder knowing how to calculate there ram usage

void cacheminder::releasecache(int releasesize, pointbucket *pbucket)
{
   
   // boost::recursive_mutex::scoped_lock mylock(quemutex);
    deque<pointbucket*>::iterator ity;
    for (ity = bucketsincache.begin(); ity < bucketsincache.end(); ity++)
    {
        if (*ity == pbucket)
        {
            bucketsincache.erase(ity);
            cacheused = cacheused - releasesize;
            return;
        }
    }
    throw ramallocationexception("cache minder asked to release cache that is not being used");
    // search for the bucket and then remove it from the queue then update the cache counter
}



cacheminder::~cacheminder()
{
    thread1.interrupt();
    thread1.join();
    delete cachetodo;
}
