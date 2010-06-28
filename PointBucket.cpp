#include "PointBucket.h"
#include "quadtreestructs.h"
#include "CacheMinder.h"


#include <cstdio>
#include "boost/filesystem.hpp"
#include <math.h>



using namespace std;

unsigned char *PointBucket::workingMemory = NULL;
unsigned char *PointBucket::compressedData = NULL;

PointBucket::PointBucket(int capacity, double minX, double minY, double maxX, double maxY, CacheMinder *MCP, string instancedirectory, int resolutionBase, int numberOfResolutionLevels)
{
   resolutionBase_ = resolutionBase;
   numberOfResolutionLevels_ = numberOfResolutionLevels; 
   capacity_ = capacity;
   minX_ = minX;
   minY_ = minY;
   maxX_ = maxX;
   maxY_ = maxY;
   MCP_ = MCP;
   instanceDirectory_ = instancedirectory;

   serialized_ = new bool[numberOfResolutionLevels_];
   incache_ = new bool[numberOfResolutionLevels_];
   updated_ = new bool[numberOfResolutionLevels_];
   compressedDataSize_ = new lzo_uint[numberOfResolutionLevels_];
   points_ = new Point*[numberOfResolutionLevels_];
   pointArraySize_ = new int[numberOfResolutionLevels_];
   numberOfPoints_ = new int[numberOfResolutionLevels_];
   numberOfSerializedPoints_ = new int[numberOfResolutionLevels_];

   
   string s = boost::lexical_cast<string > (this);

   // append the PointBuckets 'this' pointer value to the filename with each pair of digits as a sub folder of the previous
   for (int k = s.size(); k > 2; k = k - 2)
   {
      instancedirectory.append("/");
      instancedirectory.append(s, k - 2, 2);
      boost::filesystem::create_directory(instancedirectory);
   }

   // append the boundarys to the filename
   filePath_ = new string[numberOfResolutionLevels];

   for (int k = 0; k < numberOfResolutionLevels; k++)
   {
      pointArraySize_[k] = capacity_ / (pow(resolutionBase, k));
      numberOfPoints_[k] = 0;
      numberOfSerializedPoints_[k] = 0;
      serialized_[k] = false;
      incache_[k] = false;
      updated_[k] = false;
      compressedDataSize_[k] = 0;
      points_[k] = NULL;
      filePath_[k] = instancedirectory + "/" + boost::lexical_cast<string > (minX_).substr(0, 20) + "-" + boost::lexical_cast<string > (minY_).substr(0, 20) + "_" + boost::lexical_cast<string > (maxY_).substr(0, 20) + "-" + boost::lexical_cast<string > (maxX_).substr(0, 20) + "_1_" + boost::lexical_cast<string > (pow(resolutionBase_, k));
   }

   if (workingMemory == NULL)
   {
      workingMemory = (unsigned char*) malloc(LZO1B_MEM_COMPRESS);
   }

   if (compressedData == NULL)
   {
      // 1.08 increase is worst case scenario for lzo "compression"
      compressedData = (unsigned char*) malloc(ceil(sizeof (Point) * capacity_ * 1.08));
   }

}


PointBucket::~PointBucket()
{
   int cachetotal = 0;
   // when a point bucket is deleted the corrisponding serial file in secondary memory is also deleted
   for (int k = 0; k < numberOfResolutionLevels_; k++)
   {
      if (serialized_[k])
      {
         if (std::remove(filePath_[k].c_str()) != 0)
         {
            throw FileException("failed to delete serial file");
         }
      }

      // if the bucket is allocated memory this is freed
      if (incache_[k])
      {
         delete[] points_[k];
         cachetotal += pointArraySize_[k];
      }
   }
   if (cachetotal != 0)
   {
      MCP_->releaseCache(cachetotal, this);
   }
}


// the getpoint method adds a layer between outside classes and the SerializableInnerBucket. this prevents
// outside classes from accessing the SerializableInnerBucket without the PointBuckets knowledge. This
// is important as the SerializableInnerBucket may not be cached. by providing this method all access to
// SerializableInnerBucket prompts the PointBucket to check if its cached and cache if neccessary.
void PointBucket::uncache()
{
   for (int k = (numberOfResolutionLevels_ - 1); k>-1; k--)
   {
      if (incache_[k])
      {
         // check serial version already exists and if not create it, also if serial version is out of date overwrite it
         if (serialized_[k] == false || numberOfSerializedPoints_[k] != numberOfPoints_[k] || updated_[k] == true)
         {
            //code to save points array
            FILE * file;
            file = fopen(filePath_[k].c_str(), "wb");
            if (file == NULL)
            {
               throw FileException("couldn't open cache file to write");
            }
            lzo_init();
            lzo1b_1_compress((const unsigned char*) points_[k], sizeof (Point) * numberOfPoints_[k], compressedData, &compressedDataSize_[k], workingMemory);
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

         // check wether there are any buckets left in cache and remove the piontbucket if there arn't
         bool bucketsincache = false;
         for (int x = 0; x < numberOfResolutionLevels_; x++)
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

         // if some ram has been freed then break (so only uncache the first sub bucket found)
         break;
      }
   }
}


// the cache method requests some space in main memory and then loads the SerializableInnerBucket into it.
// this is only done if the SerializableInnerBucket is not already in cache.
// the parameter "force" defines wether the another bucket can be forced out of cache to accomodate this one
// if space cannot be found false is returned
bool PointBucket::cache(bool force, int resolution)
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
      points_[resolution] = new Point[pointArraySize_[resolution]];

      FILE *pFile;
      pFile = fopen(filePath_[resolution].c_str(), "rb");
      if (pFile == NULL)
      {
         throw FileException("couldn't open cache file to read");
      }
      fread(compressedData, sizeof (char), compressedDataSize_[resolution], pFile);

      lzo_uint wasteoftime;
      lzo_init();
      lzo1b_decompress(compressedData, compressedDataSize_[resolution], (unsigned char *) points_[resolution], &wasteoftime, NULL);
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
      points_[resolution] = new Point[pointArraySize_[resolution]];
      incache_[resolution] = true;
      return true;
   }

}


void PointBucket::setPoint(Point& newPoint)
{
   //for each sub bucket
   for (int k = 0; k < numberOfResolutionLevels_; k++)
   {
      // check if the point falls into this subbucket (always falls into the big one)
      if (!((numberOfPoints_[0] + 1) % int(pow(resolutionBase_, k)) == 0))
      {
         continue;
      }
      if (!incache_[k])
      {
         cache(true, k);
      }

      // update the meta data about all the points held
      if (numberOfPoints_[0] == 0)
      {
         maxIntensity_ = newPoint.intensity;
         minIntensity_ = newPoint.intensity;
         maxZ_ = newPoint.z;
         minZ_ = newPoint.z;
      }
      if (newPoint.intensity > maxIntensity_)
      {
         maxIntensity_ = newPoint.intensity;
      }
      if (newPoint.intensity < minIntensity_)
      {
         minIntensity_ = newPoint.intensity;
      }
      if (newPoint.z > maxZ_)
      {
         maxZ_ = newPoint.z;
      }
      if (newPoint.z < minZ_)
      {
         minZ_ = newPoint.z;
      }

      points_[k][numberOfPoints_[k]] = newPoint;
      numberOfPoints_[k]++;
   }
}


void PointBucket::setClassification(int i, uint8_t classification)
{
   for (int k = 0; k < numberOfResolutionLevels_; k++)
   {
      // if the point falls into this resolution subset
      if (i % int(pow(resolutionBase_, k)) == 0)
      {
         if (incache_[k])
         {
            points_[k][i].classification = classification;
         }
         else
         {
            cache(true, k);
            points_[k][i].classification = classification;
         }
         updated_[k] = true;
      }
   }
}


