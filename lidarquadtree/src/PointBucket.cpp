/*
 * File:   pointbucket.cpp
 * Authors: chrfi, jaho
 *
 * Created on February 11, 2010, 12:08 PM
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

#include "PointBucket.h"
#include "CacheMinder.h"
#include "QuadtreeExceptions.h"

#include <cstdio>
#include <float.h>
#include <limits.h>
#include <math.h>

#ifdef __WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

using namespace std;

// static shared working memory
// these are reused alot as each use writes over the previous values
unsigned char *PointBucket::workingMemory = NULL;
unsigned char *PointBucket::compressedData = NULL;

PointBucket::PointBucket(int capacity, double minX, double minY, double maxX, 
                         double maxY, CacheMinder *MCP,
                         string instancedirectory,
                         int resolutionBase, int numberOfResolutionLevels)
:
        minIntensity_		(USHRT_MAX),
        maxIntensity_		(0),
        minZ_				(DBL_MAX),
        maxZ_				(DBL_MIN),
        minX_				(minX),
        maxX_				(maxX),
        minY_				(minY),
        maxY_				(maxY),
        MCP_				(MCP),
        resolutionBase_		(resolutionBase),
        numberOfResolutionLevels_ (numberOfResolutionLevels),
        capacity_			(capacity),
        instanceDirectory_	(instancedirectory)



{
   // construct a string indicating the address of this
   string s;
   char pointer_string[13];
   int e;

   snprintf(pointer_string, 13, "%012x", (unsigned long)this);
   s = string(pointer_string);

   // all attributes which have a value for each resolution level
   serialized_ = new bool[numberOfResolutionLevels_];
   incache_ = new bool[numberOfResolutionLevels_];
   updated_ = new bool[numberOfResolutionLevels_];
   compressedDataSize_ = new lzo_uint[numberOfResolutionLevels_];
   points_ = new LidarPoint*[numberOfResolutionLevels_];
   pointArraySize_ = new int[numberOfResolutionLevels_];
   numberOfPoints_ = new int[numberOfResolutionLevels_];
   pointInterval_ = new int[numberOfResolutionLevels_];
   numberOfSerializedPoints_ = new int[numberOfResolutionLevels_];

   // append the PointBuckets 'this' pointer value to the filename with 
   // each pair of digits as a sub folder of the previous
   for (int k = s.size(); k > 2; k = k - 2)
   {
#ifndef __WIN32
      instancedirectory += "/";
#else
      instancedirectory += "\\";
#endif

      instancedirectory += s.substr(k - 2, 2);

#ifndef __WIN32
      e = mkdir(instancedirectory.c_str(), S_IRWXU);
#else
      if (CreateDirectory(instancedirectory.c_str(), NULL))
         e = 0;
      else
         e = -1;
#endif

      if (e)
         throw QuadtreeIOException();
   }

   // append the boundaries to the filename
   filePath_ = new string[numberOfResolutionLevels];

   // setup each of the resolution levels
   char resolution_string [] = // [85]
      "XXXXXXXXXXXXXXXXXXXX-XXXXXXXXXXXXXXXXXXXX_XXXXXXXXXXXXXXXXXXXX-XXXXXXXXXXXXXXXXXXXX_1_";
   char buffer[16];

   snprintf(&resolution_string[ 0], 20, "%.40g", minX_);
   snprintf(&resolution_string[21], 20, "%.40g", minY_);
   snprintf(&resolution_string[42], 20, "%.40g", maxY_);
   snprintf(&resolution_string[63], 20, "%.40g", maxX_);
   for (int k = 0; k < numberOfResolutionLevels; ++k)
   {
	   pointInterval_[k] = int(pow(resolutionBase_, k));
	   pointArraySize_[k] = capacity_ / pointInterval_[k];
	   numberOfPoints_[k] = 0;
	   numberOfSerializedPoints_[k] = 0;
	   serialized_[k] = false;
	   incache_[k] = false;
	   updated_[k] = false;
	   compressedDataSize_[k] = 0;
	   points_[k] = NULL;

      snprintf(buffer, 16, "%d", pointInterval_[k]);
	   filePath_[k] = instancedirectory +
         resolution_string + buffer;
   }

   if (workingMemory == NULL)
   {
      workingMemory = (unsigned char*) malloc(LZO1B_MEM_COMPRESS);
   }

   if (compressedData == NULL)
   {
      // 1.08 increase is worst case scenario for lzo "compression"
      compressedData = (unsigned char*) malloc(ceil(sizeof(LidarPoint) * capacity_ * 1.08));
   }
}


PointBucket::~PointBucket()
{
   int cachetotal = 0;

   // when a point bucket is deleted the corresponding serial file i+
   // secondary memory is also deleted
   for (int k = 0; k < numberOfResolutionLevels_; ++k)
   {
      if (serialized_[k])
      {
    	  remove(filePath_[k].c_str());
      }

      // if the bucket is allocated memory this is freed
      if (incache_[k])
      {
         delete[] points_[k];
         cachetotal += pointArraySize_[k];
      }
   }

   delete[] pointInterval_;
   delete[] filePath_;
   delete[] serialized_;
   delete[] incache_;
   delete[] updated_;
   delete[] compressedDataSize_;
   delete[] points_;
   delete[] pointArraySize_;
   delete[] numberOfPoints_;
   delete[] numberOfSerializedPoints_;

   if (cachetotal != 0)
   {
      MCP_->releaseCache(cachetotal, this);
   }
}

void PointBucket::uncache()
{
   for (int k = (numberOfResolutionLevels_ - 1); k>-1; --k)
   {
      if (incache_[k])
      {
         // check serial version already exists and if not create it, 
         // also if serial version is out of date overwrite it
         if (serialized_[k] == false || numberOfSerializedPoints_[k] != numberOfPoints_[k] ||
             updated_[k] == true)
         {
            //code to save points array
            FILE * file;
            file = fopen(filePath_[k].c_str(), "wb");
            if (file == NULL)
            {
               throw FileException("couldn't open cache file to write");
            }
            lzo_init();
            // compress the data using the working memory and dump the 
            // compressed data into the allocated area
            lzo1b_2_compress((const unsigned char*) points_[k], 
                             sizeof (LidarPoint) * numberOfPoints_[k], 
                             compressedData, 
                             &compressedDataSize_[k], 
                             workingMemory);
            //write the compressed data to file
            fwrite(compressedData, 1, compressedDataSize_[k], file);
            fclose(file);
            numberOfSerializedPoints_[k] = numberOfPoints_[k];
            serialized_[k] = true;

         }

         //clean up bucket
         delete[] points_[k];
         points_[k] = NULL;
         incache_[k] = false;
         // free memory only after removal is complete
         MCP_->updateCache(-pointArraySize_[k], this, true);
         // check whether there are any buckets left in cache and remove
         // the pointbucket if there arn't note: this is neccesary 
         // because updateing the cache to zero dosen't remove the bucket from
         // the cache que.
         bool bucketsincache = false;
         for (int x = 0; x < numberOfResolutionLevels_; ++x)
         {
            if (incache_[x])
            {
               bucketsincache = true;
            }
         }
         if (!bucketsincache)
         {
            MCP_->releaseCache(0, this);
         }

         // if some ram has been freed then break (so only uncache the first sub 
         // bucket found)
         break;
      }
   }
}

// the cache method requests some space in main memory and then loads the 
// SerializableInnerBucket into it.
// this is only done if the SerializableInnerBucket is not already in cache.
// the parameter "force" defines whether the another bucket can be forced out
// of cache to accomodate this one
// if space cannot be found false is returned
bool  PointBucket::cache(bool force, int resolution)
{
   assert(points_[resolution] == NULL);
   if (resolution > numberOfResolutionLevels_)
   {
      throw OutOfBoundsException("resolution index out of bounds");
   }

   // if already cached just return
   if (incache_[resolution])
   {
      return true;
   }

   if (serialized_[resolution] == true)
   {
      // aquire memory before using it to ensure memory limit is respected
      if (MCP_->updateCache(pointArraySize_[resolution], this, force) == false)
      {
         return false;
      }
        
      points_[resolution] = new LidarPoint[pointArraySize_[resolution]];

      // read the file in
      FILE *pFile;
      pFile = fopen(filePath_[resolution].c_str(), "rb");
      if (pFile == NULL)
      {
         throw FileException("couldn't open cache file to read");
      }
      fread(compressedData, sizeof (char), compressedDataSize_[resolution], pFile);

      lzo_uint wasteoftime;
      lzo_init();
      // decompress the loaded binary into the points_ array (because the order 
      // dosen't change and its loss less compression this means the array is 
      // now identical to before it was saved to HDD
      lzo1b_decompress(compressedData, compressedDataSize_[resolution], 
                       (unsigned char *) points_[resolution], 
                       &wasteoftime, NULL);
      numberOfPoints_[resolution] = numberOfSerializedPoints_[resolution];
      incache_[resolution] = true;
      fclose(pFile);
      return true;
   }
   else
   {
      // aquire memory before using it to ensure memory limit is respected
      if (MCP_->updateCache(pointArraySize_[resolution], this, force) == false)
      {
         return false;
      }

      points_[resolution] = new LidarPoint[pointArraySize_[resolution]];
      incache_[resolution] = true;
      return true;
   }
}

LidarPoint& PointBucket::getPoint(int i, int resolution)
{
    if (incache_[resolution])
    {
        return points_[resolution][i];
    }
    else
    {
        cache(true, resolution);
        return points_[resolution][i];
    }
}

void PointBucket::setPoint(LidarPoint& newPoint)
{
	if (!incache_[0])
		cache(true, 0);

	// Update max/min intensity/Z values for the bucket.
	if (newPoint.getIntensity() > maxIntensity_)
		maxIntensity_ = newPoint.getIntensity();
	else if (newPoint.getIntensity() < minIntensity_)
		minIntensity_ = newPoint.getIntensity();

	if (newPoint.getZ() > maxZ_)
		maxZ_ = newPoint.getZ();
	else if (newPoint.getZ() < minZ_)
		minZ_ = newPoint.getZ();

	// Insert into the main bucket then check if it falls into the subbuckets
	points_[0][numberOfPoints_[0]] = newPoint;
	numberOfPoints_[0]++;

	//for each sub bucket
	for (int k = 1; k < numberOfResolutionLevels_; ++k)
	{
		if ((numberOfPoints_[0] % pointInterval_[k] == 0))
		{
			if (!incache_[k])
				cache(true, k);

			points_[k][numberOfPoints_[k]] = newPoint;
			numberOfPoints_[k]++;
		}
	}
}

void PointBucket::setClassification(int i, uint8_t classification)
{
   // Cache fulll resolution
   if (!incache_[0])
   {
      cache(true, 0);
   }

   points_[0][i].setClassification(classification);

   for(int k=1; k<numberOfResolutionLevels_;++k)
   {
      if(!incache_[k])
      {
         cache(true, k);
      }
      if (i % pointInterval_[k] == 0)
         points_[k][i / pointInterval_[k]].setClassification(classification);
   }
           
   for (int k=0; k<numberOfResolutionLevels_; ++k)
      updated_[k] = true;
}

int PointBucket::get_cache_used()
{
    	return MCP_->get_cacheused();
}



