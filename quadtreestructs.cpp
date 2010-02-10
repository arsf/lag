#include "quadtreestructs.h"
#include "cacheminder.h"
#include <fstream>
#include <c++/3.4.6/iosfwd>
#include <limits.h>
#include <cstdio>
#include "boost/filesystem.hpp"
#include "quadtreeexceptions.h"

using namespace std;

// self explanitory :P

pointbucket::pointbucket(int cap, double minx, double miny, double maxx, double maxy, cacheminder *MCP, string instancedirectory)
{
    numberofpoints = 0;
    numberofcachedpoints = 0;
    this->cap = cap;

    this->minx = minx;
    this->miny = miny;
    this->maxx = maxx;
    this->maxy = maxy;
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
    filepath.append("/"+boost::lexical_cast<string>(minx)+"-"+boost::lexical_cast<string>(miny)+"_"+boost::lexical_cast<string>(maxy)+"-"+boost::lexical_cast<string>(maxx));

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
        delete b;
    }
}


// the getpoint method adds a layer between outside classes and the SerializableInnerBucket. this prevents
    // outside classes from accessing the SerializableInnerBucket without the pointbuckets knowledge. This
    // is important as the SerializableInnerBucket may not be cached. by providing this method all access to
    // SerializableInnerBucket prompts the pointbucket to check if its cached and cache if neccessary.
void pointbucket::uncache()
{
    boost::recursive_mutex::scoped_lock mylock(cachemutex);
    // check serial version already exists and if not create it, also if serial version is out of date overwrite it
    if (serialized == false || numberofcachedpoints != numberofpoints)
    {

        b->length=numberofpoints;
        std::ofstream ofs(filepath.c_str(), ios::out | ios::binary | ios::trunc);

        boost::archive::binary_oarchive binaryouta(ofs);
        binaryouta << b;
        ofs.close();
        serialized = true;
    }
    //clean up bucket
    delete b;
    b = NULL;
    // free memory only after removal is complete
    MCP->releasecache(cap, this);
    incache = false;

}


// the cache method requests some space in main memory and then loads the SerializableInnerBucket into it.
    // this is only done if the SerializableInnerBucket is not already in cache.
    // the parameter "force" defines wether the another bucket can be forced out of cache to accomodate this one
    // if space cannot be found false is returned
bool pointbucket::cache(bool force)
{
    boost::recursive_mutex::scoped_lock mylock(cachemutex);
    // if already cached just return
    if (incache)
    {
        return true;
    }
    if (serialized == true)
    {
        // aquire memory before using it to ensure memory limit is respected
        if (MCP->requestcache(cap, this, force) == false)
        {
            return false;
        }
        b = new SerializableInnerBucket;
        // load the serial version from the filename assigned into a new bucket instance

        std::ifstream ifs(filepath.c_str(), ios::out | ios::binary);
        
        boost::archive::binary_iarchive binaryina(ifs);
        binaryina >> b;
        ifs.close();
        incache = true;
        numberofcachedpoints = numberofpoints;
        return true;
    }
    else
    {
        // aquire memory before using it to ensure memory limit is respected
        if (MCP->requestcache(cap, this, force) == false)
        {
            return false;
        }
        b = new SerializableInnerBucket(cap);
        incache = true;
        return true;
    }

}

