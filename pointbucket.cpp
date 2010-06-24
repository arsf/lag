
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

pointbucket::pointbucket(int cap, double minX, double minY, double maxX, double maxY, cacheminder *MCP, string instancedirectory, int resolutionbase, int numresolutionlevels)
{
   splitvalue = resolutionbase;
   numberofsplitlevels = numresolutionlevels;
   numberofpoints = new int[numberofsplitlevels];
   numberofserializedpoints = new int[numberofsplitlevels];
   this->cap = cap;

   pointarraysize = new int[numberofsplitlevels];
   
   this->minX = minX;
   this->minY = minY;
   this->maxX = maxX;
   this->maxY = maxY;
   this->MCP = MCP;
   serialized = new bool[numberofsplitlevels];
   incache = new bool[numberofsplitlevels];
   updated = new bool[numberofsplitlevels];
   compresseddatasize = new lzo_uint[numberofsplitlevels];
   points = new point*[numberofsplitlevels];



   this->instancedirectory = instancedirectory;
   string s = boost::lexical_cast<string > (this);
   
   // append the pointbuckets 'this' pointer value to the filename with each pair of digits as a sub folder of the previous
   for (int k = s.size(); k > 2; k = k - 2)
   {
      instancedirectory.append("/");
      instancedirectory.append(s, k - 2, 2);


      boost::filesystem::create_directory(instancedirectory);
   }
   // append the boundarys to the filename
   filepath = new string[numberofsplitlevels];

   for(int k=0; k<numberofsplitlevels; k++)
   {
      pointarraysize[k] = cap/(pow(splitvalue, k));
      numberofpoints[k] = 0;
      numberofserializedpoints[k] = 0;
      serialized[k] = false;
      incache[k] = false;
      updated[k] = false;
      compresseddatasize[k] = 0;
      points[k] = NULL;
      filepath[k] = instancedirectory + "/" + boost::lexical_cast<string >(minX).substr(0,20) + "-" + boost::lexical_cast<string >(minY).substr(0,20) + "_" + boost::lexical_cast<string >(maxY).substr(0,20) + "-" + boost::lexical_cast<string >(maxX).substr(0,20) + "_1_" + boost::lexical_cast<string >(pow(splitvalue, k));
   }

   
   

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
   int cachetotal = 0;
   // when a point bucket is deleted the corrisponding serial file in secondary memory is also deleted
   for (int k = 0; k < numberofsplitlevels; k++)
   {
      if (serialized[k])
      {

         if (std::remove(filepath[k].c_str()) != 0)
         {
            throw fileexception("failed to delete serial file");
         }
      }


      // if the bucket is allocated memory this is freed
      if (incache[k])
      {
         delete[] points[k];
         cachetotal += pointarraysize[k];
      }
      
   }
   if (cachetotal != 0)
   {
      MCP->releasecache(cachetotal, this);
   }
}


// the getpoint method adds a layer between outside classes and the SerializableInnerBucket. this prevents
// outside classes from accessing the SerializableInnerBucket without the pointbuckets knowledge. This
// is important as the SerializableInnerBucket may not be cached. by providing this method all access to
// SerializableInnerBucket prompts the pointbucket to check if its cached and cache if neccessary.

void pointbucket::uncache()
{

   for(int k=(numberofsplitlevels-1); k>-1; k--)
   {
      if(incache[k])
      {

         // check serial version already exists and if not create it, also if serial version is out of date overwrite it
         if (serialized[k] == false || numberofserializedpoints[k] != numberofpoints[k] || updated[k] == true)
         {
            //code to save points array
            FILE * pFile;
            pFile = fopen(filepath[k].c_str(), "wb");
            if (pFile == NULL)
            {
               cout << strerror(errno) << endl;
               cout << filepath[k].c_str() << endl;
               throw "couldn't open cache fileto write";
            }
            lzo_init();
            lzo1b_1_compress((const unsigned char*)points[k], sizeof(point)*numberofpoints[k], compresseddata, &compresseddatasize[k], workingmemory);
            o_counter += compresseddatasize[k];
            fwrite(compresseddata, 1, compresseddatasize[k], pFile);
            fclose(pFile);
            numberofserializedpoints[k] = numberofpoints[k];
            serialized[k] = true;

         }
         //clean up bucket

         
         delete[] points[k];
         points[k] = NULL;
         incache[k] = false;
         // free memory only after removal is complete
         MCP->updatecache(-pointarraysize[k], this, true);

         // check wether there are any buckets left in cache and remove the piontbucket if there arn't
         bool bucketsincache = false;
         for (int x=0; x<numberofsplitlevels; x++)
         {
            if(incache[x])
            {
               bucketsincache = true;
            }
         }
         if(!bucketsincache)
         {
            MCP->releasecache(0, this);
         }
         
         

         
         // if some ram has been freed then break (so only uncache the first sub bucket found)
         break;
      }
   }
   // if this point is reached then all subbuckets are uncached and this pointbucket
   // is registered as using zero ram, all that remains is to tell cacheminder to remove it
   
}


// the cache method requests some space in main memory and then loads the SerializableInnerBucket into it.
// this is only done if the SerializableInnerBucket is not already in cache.
// the parameter "force" defines wether the another bucket can be forced out of cache to accomodate this one
// if space cannot be found false is returned

bool pointbucket::cache(bool force, int resolution)
{

   assert(points[resolution] == NULL);
   if (resolution > numberofsplitlevels)
   {
      throw "resolution index out of bounds";
   }
   // if already cached just return
   if (incache[resolution])
   {
      return true;
   }
   if (serialized[resolution] == true)
   {
      // aquire memory before using it to ensure memory limit is respected
      if (MCP->updatecache(pointarraysize[resolution], this, force) == false)
      {
         return false;
      }
      points[resolution] = new point[pointarraysize[resolution]];
              
      
      FILE *pFile;
      pFile = fopen(filepath[resolution].c_str(), "rb");
      if (pFile == NULL)
      {
         throw "couldn't open cache file to read";
      }
      fread(compresseddata, sizeof(char), compresseddatasize[resolution], pFile);
     
      i_counter += compresseddatasize[resolution];
      lzo_uint wasteoftime;
      lzo_init();
      lzo1b_decompress(compresseddata, compresseddatasize[resolution], (unsigned char *)points[resolution], &wasteoftime, NULL);
      numberofpoints[resolution] = numberofserializedpoints[resolution];
      incache[resolution] = true;
      fclose(pFile);
      return true;
   }
   else
   {
      // aquire memory before using it to ensure memory limit is respected
      if (MCP->updatecache(pointarraysize[resolution], this, force) == false)
      {
         return false;
      }
      points[resolution] = new point[pointarraysize[resolution]];
      incache[resolution] = true;
      return true;
   }

}



void pointbucket::setpoint(point& newP)
{
   //for each sub bucket
   for(int k=0; k<numberofsplitlevels; k++)
   {
      // check if the point falls into this subbucket (always falls into the big one)
      if (! ((numberofpoints[0]+1) % int(pow(splitvalue, k)) == 0))
      {
         continue; 
      }
      if (!incache[k])
      {
         cache(true, k);
      }
  
      // update the meta data about all the points held
      if (numberofpoints[0] == 0)
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
     /* if (numberofpoints == pointarraysize)
      {
         pointarraysize+=increaseamount;
         if (!MCP->requestcache(increaseamount, this, true))
         {
            throw ramallocationexception("failed to acquire extra ram to allow more points to be inserted");
         }

         point *temp = new point[pointarraysize];
         copy(points, points+numberofpoints, temp);
         delete[] points;
         points = temp;
         points[numberofpoints] = newP;

         numberofpoints++;
      }*/
      points[k][numberofpoints[k]] = newP;
      numberofpoints[k]++;
   }
}


void pointbucket::setclassification(int i, uint8_t classification)
{
   for (int k = 0; k < numberofsplitlevels; k++)
   {
      if (i % int(pow(splitvalue, k)) == 0)
      {
         if (incache[k])
         {
            points[k][i].classification = classification;
         }
         else
         {
            cache(true, k);
            points[k][i].classification = classification;
         }
         updated[k] = true;
      }
   }
}


