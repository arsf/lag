#include "quadtreestructs.h"
#include "cacheminder.h"
#include <fstream>
#include <c++/3.4.6/iosfwd>
#include <limits.h>
#include <cstdio>

using namespace std;

// self explanitory :P

pointbucket::pointbucket(int cap, double minx, double miny, double maxx, double maxy, cacheminder *MCP)
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
    sprintf(serialfile, "/tmp/test/%f_%f-%f_%f", minx, miny, maxx, maxy);
}

// self explanitory :P

pointbucket::~pointbucket()
{
    if(serialized)
    {
        if(remove(serialfile) != 0)
        {
            throw serialfile;
            throw "failed to delete serial file";
        }
    }

    
    if (incache)
    {
        MCP->releasecache(cap, this);
        delete b;
    }
}

void pointbucket::uncache()
{
    boost::recursive_mutex::scoped_lock mylock(cachemutex);
    // check serial version already exists and if not create it
    if (serialized == false || numberofcachedpoints != numberofpoints)
    {
        // generate file name and save
        b->length=numberofpoints;
        std::ofstream ofs(serialfile, ios::out | ios::binary | ios::trunc);

        boost::archive::binary_oarchive binaryouta(ofs);
        binaryouta << b;
        ofs.close();
        serialized = true;
    }
    //clean up bucket
    delete b;
    b = NULL;
    MCP->releasecache(cap, this);
    incache = false;

}

bool pointbucket::cache(bool force)
{
    boost::recursive_mutex::scoped_lock mylock(cachemutex);
    if (incache)
    {
        return true;
    }
    if (serialized == true)
    {
        
        if (MCP->requestcache(cap, this, force) == false)
        {
            return false;
        }
        b = new SerializableInnerBucket;
        // load the serial version from the filename assigned into a new bucket instance
        std::ifstream ifs(serialfile, ios::out | ios::binary);
        
        boost::archive::binary_iarchive binaryina(ifs);
        binaryina >> b;
        ifs.close();
        incache = true;
        numberofcachedpoints = numberofpoints;
        return true;
    }
    else
    {
        
        if (MCP->requestcache(cap, this, force) == false)
        {
            return false;
        }
        b = new SerializableInnerBucket(cap);
        incache = true;
        return true;
    }

}

