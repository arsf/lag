
#include "quadtreestructs.h"
#include "cacheminder.h"


#include <cstdio>
#include "boost/filesystem.hpp"
#include <math.h>



using namespace std;

unsigned char *pointbucket::workingmemory = NULL;
unsigned char *pointbucket::compresseddata = NULL;
long pointbucket::o_counter = 0;
long pointbucket::i_counter = 0;

pointbucket::pointbucket(int cap, double minX, double minY, double maxX, double maxY, cacheminder *MCP, string instancedirectory)
{
   numberofpoints = 0;
   numberofserializedpoints = 0;
   this->cap = cap;

   pointarraysize = cap/8;
   increaseamount = pointarraysize;
   this->minX = minX;
   this->minY = minY;
   this->maxX = maxX;
   this->maxY = maxY;
   this->MCP = MCP;
   serialized = false;
   incache = false;
   this->instancedirectory = instancedirectory;
   string s = boost::lexical_cast<string > (this);
   filepath = instancedirectory;
   // append the pointbuckets 'this' pointer value to the filename with each pair of digits as a sub folder of the previous
   for (int k = s.size(); k > 2; k = k - 2)
   {
      filepath.append("/");
      filepath.append(s, k - 2, 2);


      boost::filesystem::create_directory(filepath);
   }
   // append the boundarys to the filename
   filepath.append("/" + boost::lexical_cast<string > (minX) + "-" + boost::lexical_cast<string > (minY) + "_" + boost::lexical_cast<string > (maxY) + "-" + boost::lexical_cast<string > (maxX));

   
   updated = false;

   
   points = NULL;

   if(workingmemory == NULL)
   {
      workingmemory = (unsigned char*)malloc(LZO1B_MEM_COMPRESS);
      cout << "uh oh 1" << endl;
   }

   if(compresseddata == NULL)
   {
      // 1.08 increase is worst case scenario for lzo "compression"
      compresseddata = (unsigned char*)malloc(ceil(sizeof(point)*cap*1.08));
      cout << "uh oh 2" << endl;
   }

}

pointbucket::~pointbucket()
{
   // when a point bucket is deleted the corrisponding serial file in secondary memory is also deleted
   if (serialized)
   {

      if (std::remove(filepath.c_str()) != 0)
      {
         throw fileexception("failed to delete serial file");
      }
   }

   // if the bucket is allocated memory this is freed
   if (incache)
   {
      MCP->releasecache(pointarraysize, this);
      delete[] points;
   }
}


// the getpoint method adds a layer between outside classes and the SerializableInnerBucket. this prevents
// outside classes from accessing the SerializableInnerBucket without the pointbuckets knowledge. This
// is important as the SerializableInnerBucket may not be cached. by providing this method all access to
// SerializableInnerBucket prompts the pointbucket to check if its cached and cache if neccessary.

void pointbucket::uncache()
{
   // boost::recursive_mutex::scoped_lock mylock(cachemutex);
   // check serial version already exists and if not create it, also if serial version is out of date overwrite it
   if (serialized == false || numberofserializedpoints != numberofpoints || updated == true)
   {
      //code to save points array
      FILE * pFile;
      pFile = fopen(filepath.c_str(), "wb");
      if (pFile == NULL)
      {
         throw "couldn't open cache file to write";
      }
      lzo_init();  
      lzo1b_1_compress((const unsigned char*)points, sizeof(point)*numberofpoints, compresseddata, &compresseddatasize, workingmemory);
      o_counter += compresseddatasize;
      fwrite(compresseddata, 1, compresseddatasize, pFile);
      fclose(pFile);
      numberofserializedpoints = numberofpoints;
      serialized = true;
      
   }
   //clean up bucket
   

   delete[] points;
   points = NULL;
   // free memory only after removal is complete
   MCP->releasecache(pointarraysize, this);
   incache = false;

}


// the cache method requests some space in main memory and then loads the SerializableInnerBucket into it.
// this is only done if the SerializableInnerBucket is not already in cache.
// the parameter "force" defines wether the another bucket can be forced out of cache to accomodate this one
// if space cannot be found false is returned

bool pointbucket::cache(bool force)
{
   assert(points == NULL);
   //boost::recursive_mutex::scoped_lock mylock(cachemutex);
   // if already cached just return
   if (incache)
   {
      return true;
   }
   if (serialized == true)
   {
      // aquire memory before using it to ensure memory limit is respected
      if (MCP->requestcache(pointarraysize, this, force) == false)
      {
         return false;
      }

      points = new point[pointarraysize];
              
      
      FILE *pFile;
      pFile = fopen(filepath.c_str(), "rb");
      if (pFile == NULL)
      {
         throw "couldn't open cache file to read";
      }
      fread(compresseddata, sizeof(char), compresseddatasize, pFile);
      i_counter += compresseddatasize;
      lzo_uint wasteoftime;
      lzo_init();
      lzo1b_decompress(compresseddata, compresseddatasize, (unsigned char *)points, &wasteoftime, NULL);

      numberofpoints = numberofserializedpoints;
      incache = true;
      return true;
   }
   else
   {
      // aquire memory before using it to ensure memory limit is respected
      if (MCP->requestcache(pointarraysize, this, force) == false)
      {
         return false;
      }
      points = new point[pointarraysize];
      incache = true;
      return true;
   }

}

bool pointbucket::increasecache(bool force, int i)
{
   // boost::recursive_mutex::scoped_lock mylock(cachemutex);

   if (!incache)
   {
      return false;
   }
   return MCP->requestcache(i, this, force);
}

void pointbucket::setpoint(point& newP)
{
   if (!incache)
   {
      cache(true);
   }

   // update the meta data about all the points held
   if (numberofpoints == 0)
   {
      maxintensity = newP.intensity;
      minintensity = newP.intensity;
      maxZ = newP.z;
      minZ = newP.z;
   }
   if (newP.intensity > maxintensity)
   {
      maxintensity = newP.intensity;
   }
   if (newP.intensity < minintensity)
   {
      minintensity = newP.intensity;
   }
   if (newP.z > maxZ)
   {
      maxZ = newP.z;
   }
   if (newP.z < minZ)
   {
      minZ = newP.z;
   }

   // if the ram limit has been reached but not the overall points per bucket limit,
   // request an increase in ram to accomodate new points
   if (numberofpoints == pointarraysize)
   {
      pointarraysize+=increaseamount;
      if (!increasecache(true, increaseamount))
      {
         throw ramallocationexception("failed to acquire extra ram to allow more points to be inserted");
      }

      point *temp = new point[pointarraysize];
      copy(points, points+numberofpoints, temp);
      delete[] points;
      points = temp;
      points[numberofpoints] = newP;

      numberofpoints++;
   }
   points[numberofpoints] = newP;
   numberofpoints++;
   return;
}

