/* 
 * File:   cacheminder.cpp
 * Author: chrfi
 * 
 * Created on January 26, 2010, 3:57 PM
 */

#include "cacheminder.h"
#include <iostream>

cacheminder::cacheminder(int cachesize)
{
    totalcache = cachesize;
    cacheused = 0;
}

// WANRING!!!: it is imperitive that the requestsize is correct and is always
// identical to the relasesize given to the releasecache method below
// if there don't match up things go downhill fast, see releasecache for justification

bool cacheminder::requestcache(int requestsize, pointbucket *pbucket, bool force)
{
    if (requestsize > totalcache)
    {
        throw "request has been made to cache minder asking for more ram in a single block than available in total";
    }



    if (force)
    {
        // if not enough free cache remove buckets from cache in queue order till there is space
        while (cacheused + requestsize > totalcache)
        {
            bucketsincache.front()->uncache();
        }
    }
    else if((cacheused + requestsize > totalcache))
    {
        return false;
    }

    bucketsincache.push_back(pbucket);
    cacheused = cacheused + requestsize;
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
    throw "cache minder asked to release cache that is not being used";
    // search for the bucket and then remove it from the queue then update the cache counter
}





void cacheminder::cachelist(vector<pointbucket *> *bucketlist)
{
    vector<pointbucket *> tobereleased;
    for(int k=0; k<bucketsincache.size(); k++)
    {
        bool found = false;
        for(int x=0; x<bucketlist->size(); x++)
        {
            if (bucketsincache.at(k) == bucketlist->at(x))
            {
                found = true;
            }
        }
        if (found == false)
        {
            tobereleased.push_back(bucketsincache[k]);
        }
    }

    for (int k=0; k<tobereleased.size(); k++)
    {
        tobereleased[k]->uncache();
    }

    for (int k=0; k<bucketlist->size(); k++)
    {
        if(bucketlist->at(k)->incache == false)
        {
            if(bucketlist->at(k)->cache(false))
            {
                break;
            }
        }
    }
    
    // clear the cache

    // load as much of the list as possible into the chache
}


cacheminder::~cacheminder() { }

