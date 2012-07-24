/* 
 * File:   CacheMinder.cpp
 * Author: chrfi, jaho
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

#include "CacheMinder.h"
#include "QuadtreeExceptions.h"

#include <deque>


CacheMinder::CacheMinder(int cacheSize)
:
	cacheUsed_	(0),
	totalCache_	(cacheSize),
	cacheing_	(false)
{
}

void CacheMinder::clearCache()
{
   while (!bucketsInCache_.empty())
   {
      bucketsInCache_.front()->uncache();
   }
}

bool CacheMinder::updateCache(int requestSize, PointBucket *pointBucket, bool force)
{

   if (requestSize > totalCache_)
   {
      throw RamAllocationException("request has been made to cache minder asking \
                                    for more ram in a single block than available \
                                    in total");
   }

   // if we are reducing the cache a bucket is using we don't want to change its
   // position in the que
   if(requestSize >= 0)
   {
      //remove the pointbucket from the cache que if it is present
      deque<PointBucket *>::iterator ity;
      for (ity = bucketsInCache_.begin(); ity < bucketsInCache_.end(); ++ity)
      {
         if (*ity == pointBucket)
         {
            bucketsInCache_.erase(ity);
            break;
         }
      }
   }

   if (force)
   {
      // if not enough free cache remove buckets from cache in queue order till there 
      // is space
      while (cacheUsed_ + requestSize > totalCache_)
      {
         // if we are reducing the cache a bucket is using we don't want to change
         // its position in the que
         bucketsInCache_.front()->uncache();
         cacheing_ = true;
      }
   }
   else if ((cacheUsed_ + requestSize > totalCache_))
   {
      return false;
   }

   // add the pointbucket back into the cache que but at the back (this means
   // that it will be the last to uncache now)
   // if we are reducing the cache a bucket is using we don't want to change
   // its position in the que
   if (requestSize >= 0)
   {
      bucketsInCache_.push_back(pointBucket);
   }
   cacheUsed_ = cacheUsed_ + requestSize;
   return true;
}

void CacheMinder::releaseCache(int releaseSize, PointBucket *bucketToRemove)
{
	// remove the pointbucket from the cache que and update available cache
   deque<PointBucket*>::iterator ity;
   for (ity = bucketsInCache_.begin(); ity <= bucketsInCache_.end(); ++ity)
   {
      if (*ity == bucketToRemove)
      {
         bucketsInCache_.erase(ity);
         cacheUsed_ = cacheUsed_ - releaseSize;
         return;
      }
   }
   throw RamAllocationException("cache minder asked to release cache that is \
                                 not being used");
}

CacheMinder::~CacheMinder()
{
}
