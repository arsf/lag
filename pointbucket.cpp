
#include "quadtreestructs.h"
#include "cacheminder.h"
#include <fstream>
#include <c++/3.4.6/iosfwd>
#include <limits.h>
#include <cstdio>
#include "boost/filesystem.hpp"


using namespace std;

// self explanitory :P





pointbucket::pointbucket(int cap, double minX, double minY, double maxX, double maxY, cacheminder *MCP, string instancedirectory)
{
    numberofpoints = 0;
    numberofserializedpoints = 0;
    this->cap = cap;
    innerbucketsize = 25000;
    this->minX = minX;
    this->minY = minY;
    this->maxX = maxX;
    this->maxY = maxY;
    this->MCP = MCP;
    serialized = false;
    incache = false;
    this->instancedirectory = instancedirectory;
    string s = boost::lexical_cast<string>(this);
    filepath = instancedirectory;
    for(int k=s.size(); k>3; k=k-2)
    {
        filepath.append("/");
        filepath.append(s, k-2, 2);


        boost::filesystem::create_directory(filepath);
    }
    filepath.append("/"+boost::lexical_cast<string>(minX)+"-"+boost::lexical_cast<string>(minY)+"_"+boost::lexical_cast<string>(maxY)+"-"+boost::lexical_cast<string>(maxX));
    innerbucket = NULL;
}




pointbucket::~pointbucket()
{
    // when a point bucket is deleted the corrisponding serial file in secondary memory is also deleted
    if(serialized)
    {

        if(std::remove(filepath.c_str()) != 0)
        {
            throw fileexception("failed to delete serial file");
        }
    }


    if (incache)
    {
        MCP->releasecache(cap, this);
        delete innerbucket;
    }
}


// the getpoint method adds a layer between outside classes and the SerializableInnerBucket. this prevents
    // outside classes from accessing the SerializableInnerBucket without the pointbuckets knowledge. This
    // is important as the SerializableInnerBucket may not be cached. by providing this method all access to
    // SerializableInnerBucket prompts the pointbucket to check if its cached and cache if neccessary.
void pointbucket::uncache()
{
   // cout << "uncaching    actual size " << innerbucket->size << "  size used " << innerbucketsize << endl;
   // boost::recursive_mutex::scoped_lock mylock(cachemutex);
    // check serial version already exists and if not create it, also if serial version is out of date overwrite it
    if (serialized == false || numberofserializedpoints != numberofpoints)
    {

        innerbucket->numpoints=numberofpoints;
        std::ofstream ofs(filepath.c_str(), ios::out | ios::binary | ios::trunc);

        boost::archive::binary_oarchive binaryouta(ofs);
        binaryouta << innerbucket;
        ofs.close();
        serialized = true;
        innerbucketsize = innerbucket->size;
    }
    //clean up bucket
    delete innerbucket;
    innerbucket = NULL;
    // free memory only after removal is complete
    MCP->releasecache(innerbucketsize, this);
    incache = false;

}


// the cache method requests some space in main memory and then loads the SerializableInnerBucket into it.
    // this is only done if the SerializableInnerBucket is not already in cache.
    // the parameter "force" defines wether the another bucket can be forced out of cache to accomodate this one
    // if space cannot be found false is returned
bool pointbucket::cache(bool force)
{
    assert(innerbucket == NULL);
    //boost::recursive_mutex::scoped_lock mylock(cachemutex);
    // if already cached just return
    if (incache)
    {
        return true;
    }
    if (serialized == true)
    {
        // aquire memory before using it to ensure memory limit is respected
        if (MCP->requestcache(innerbucketsize, this, force) == false)
        {
            return false;
        }
        innerbucket = new SerializableInnerBucket();
        // load the serial version from the filename assigned into a new bucket instance


        

        std::ifstream ifs(filepath.c_str(), ios::out | ios::binary);

        boost::archive::binary_iarchive binaryina(ifs);
        binaryina >> innerbucket;
        ifs.close();
        incache = true;
       // cout << "cacheing     actual size " << innerbucket->size << "  size used " << innerbucketsize << endl;
        numberofserializedpoints = numberofpoints;
        return true;
    }
    else
    {
        // aquire memory before using it to ensure memory limit is respected
        if (MCP->requestcache(innerbucketsize, this, force) == false)
        {
            return false;
        }
        innerbucket = new SerializableInnerBucket(innerbucketsize, 25000);
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

        if (innerbucket->size == innerbucket->numpoints)
        {
            if(!increasecache(true, innerbucket->increase))
            {
                throw ramallocationexception("failed to acquire extra ram to allow more points to be inserted");
            }
            innerbucket->setpoint(newP);
            innerbucketsize = innerbucket->size;
            numberofpoints++;

        }


        innerbucket->setpoint(newP);
        numberofpoints++;
        return;
    }

/*
void pointbucket::generateraster(double width)
{
   double ratio = (maxX-minX)/(maxY-minY);
   double height = width/ratio;
   point *raster = new point[width][height];

   for(int x=0; x<width; x++)
   {
      for(int y=0; y<height; y++)
      {
         raster[x][y].z = minZ-1;
      }
   }

   for(int k=0; k<numberofpoints; k++)
   {
      point current = getpoint(k);
      int x=abs((current.x % width)*10);
      int y=abs((current.y % height)*10);

      if(current.z > raster[x][y])
      {
         raster[x][y] = current;
      }
   }

   for(int x=0; x<width; x++)
   {
      for(int y=0; y<height; y++)
      {
         raster[x][y].z = minZ-1;
      }
   }

}*/