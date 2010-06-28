/* 
 * File:   cacheminder.cpp
 * Author: chrfi
 * 
 * Created on January 26, 2010, 3:57 PM
 */

#include "CacheMinder.h"
#include "quadtreeexceptions.h"

#include <deque>


CacheMinder::CacheMinder(int cacheSize)
{
   totalCache_ = cacheSize;
   cacheUsed_ = 0;
   cacheing_ = false;
}


// WANRING!!!: it is imperitive that the requestsize is correct and is always
// identical to the relasesize given to the releasecache method below
// if they don't match up things go downhill fast, see releasecache for justification
void CacheMinder::clearCache()
{
   while (!bucketsInCache_.empty())
   {
      bucketsInCache_.front()->uncache();
   }
}


bool CacheMinder::updateCache(int requestSize, PointBucket *pointBucket, bool force)
{
   //boost::recursive_mutex::scoped_lock mylock(quemutex);
   if (requestSize > totalCache_)
   {
      throw RamAllocationException("request has been made to cache minder asking for more ram in a single block than available in total");
   }

   deque<PointBucket *>::iterator ity;
   for (ity = bucketsInCache_.begin(); ity < bucketsInCache_.end(); ity++)
   {
      if (*ity == pointBucket)
      {
         bucketsInCache_.erase(ity);
         break;
      }
   }

   if (force)
   {
      // if not enough free cache remove buckets from cache in queue order till there is space
      while (cacheUsed_ + requestSize > totalCache_)
      {
         bucketsInCache_.front()->uncache();
         cacheing_ = true;
      }
   }
   else if ((cacheUsed_ + requestSize > totalCache_))
   {
      return false;
   }

   bucketsInCache_.push_back(pointBucket);
   cacheUsed_ = cacheUsed_ + requestSize;
   return true;
}


// this should only be called if your really really sure that you have already freed the ram
// WARNING!!!: as this is dealing with keeping a count of ram it is imperitive that
// any object calling releasecache gets its sums right, if your cumulative cacheUpdates and release don't
// match up all hell will break loose
// NOTE: this has been done so that the buckets decide there ram size not the minder, this allows
// different types of buckets to be used or buckets containing different amounts of data without
// the minder knowing how to calculate there ram usage
void CacheMinder::releaseCache(int releaseSize, PointBucket *bucketToRemove)
{

   // boost::recursive_mutex::scoped_lock mylock(quemutex);
   deque<PointBucket*>::iterator ity;
   for (ity = bucketsInCache_.begin(); ity < bucketsInCache_.end(); ity++)
   {
      if (*ity == bucketToRemove)
      {
         bucketsInCache_.erase(ity);
         cacheUsed_ = cacheUsed_ - releaseSize;
         return;
      }
   }
   throw RamAllocationException("cache minder asked to release cache that is not being used");
   // search for the bucket and then remove it from the queue then update the cache counter
}


CacheMinder::~CacheMinder() {
 }
