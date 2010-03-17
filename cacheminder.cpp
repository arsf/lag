/* 
 * File:   cacheminder.cpp
 * Author: chrfi
 * 
 * Created on January 26, 2010, 3:57 PM
 */

#include "cacheminder.h"
#include <iostream>
#include <deque>
#include "boost/date_time/time_duration.hpp"
#include "quadtreeexceptions.h"

cacheminder::cacheminder(int cachesize)
{
    totalcache = cachesize;
    cacheused = 0;
    cachetodo = new deque<pointbucket *>;
    cacheing = false;
}

// WANRING!!!: it is imperitive that the requestsize is correct and is always
// identical to the relasesize given to the releasecache method below
// if there don't match up things go downhill fast, see releasecache for justification

void cacheminder::clearcache()
{
    while (!bucketsincache.empty())
    {
        bucketsincache.front()->uncache();
    }
}

bool cacheminder::requestcache(int requestsize, pointbucket *pbucket, bool force)
{
    boost::recursive_mutex::scoped_lock mylock(quemutex);
    if (requestsize > totalcache)
    {
        throw ramallocationexception("request has been made to cache minder asking for more ram in a single block than available in total");
    }

    deque<pointbucket *>::iterator ity;
    for(ity=bucketsincache.begin(); ity<bucketsincache.end(); ity++)
    {
        if (*ity == pbucket)
        {
            bucketsincache.erase(ity);
            break;
        }
    }

    if (force)
    {
        // if not enough free cache remove buckets from cache in queue order till there is space
        while (cacheused + requestsize > totalcache)
        {
            bucketsincache.front()->uncache();
            cacheing = true;
        }
    }
    else if ((cacheused + requestsize > totalcache))
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
    boost::recursive_mutex::scoped_lock mylock(quemutex);
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
    throw ramallocationexception("cache minder asked to release cache that is not being used");
    // search for the bucket and then remove it from the queue then update the cache counter
}

void cacheminder::cachelist(vector<pointbucket *> *bucketlist)
{
    {
        boost::recursive_mutex::scoped_lock mylock(quemutex);

        // if the cache limit has not been reached yet there is no need to clear the cache or cache new buckets in
        if(!cacheing)
        {
            return;
        }

        vector<pointbucket *> tobereleased;
        cout << "start" << endl;
        
        for (unsigned int k = 0; k < bucketsincache.size(); k++)
        {
            bool found = false;
            for (unsigned int x = 0; x < bucketlist->size(); x++)
            {
                if (bucketsincache.at(k) == bucketlist->at(x))
                {
                    found = true;
                    break;
                }
            }
            if (found == false)
            {
                tobereleased.push_back(bucketsincache[k]);
            }
        }
        
        cout << "middle" << endl;
        for (unsigned int k = 0; k < tobereleased.size(); k++)
        {
            tobereleased[k]->uncache();
        }
        cout << "stop" << endl;
        
    }
    setcachetodo(bucketlist);

    // clear the cache

    // load as much of the list as possible into the chache
}

cacheminder::~cacheminder()
{
    thread1.interrupt();
    thread1.join();
    delete cachetodo;
}

void cacheminder::setcachetodo(vector<pointbucket *> *tasks)
{
    boost::recursive_mutex::scoped_lock mylock(todomutex);
    delete cachetodo;
    cachetodo = new deque<pointbucket *>();
    for (unsigned int k = 0; k < tasks->size(); k++)
    {
        cachetodo->push_back(tasks->at(k));
    }
    startcachethread();
}

void cacheminder::pushcachetodo(vector<pointbucket *> *tasks)
{
    boost::recursive_mutex::scoped_lock mylock(todomutex);
    for (unsigned int k = 0; k < tasks->size(); k++)
    {
        cachetodo->push_back(tasks->at(k));
    }
    startcachethread();
}

void cacheminder::popcachetodo()
{
    boost::recursive_mutex::scoped_lock mylock(todomutex);
    cachetodo->pop_back();
}

void cacheminder::deletecachetodo(vector<pointbucket *> *tasks)
{
    boost::recursive_mutex::scoped_lock mylock(todomutex);
    for (unsigned int k = 0; k < tasks->size(); k++)
    {
        for (unsigned int x = 0; x < cachetodo->size(); x++)
        {
            if (tasks->at(k) == cachetodo->at(x))
            {
                cachetodo->erase(cachetodo->begin() + x);
                break;
            }
        }
    }
}

void cacheminder::clearcachetodo()
{
    boost::recursive_mutex::scoped_lock mylock(todomutex);
    cachetodo->clear();
    cout << "thread over" << endl;
}

void cacheminder::startcachethread()
{
    boost::posix_time::time_duration td(0, 0, 0, 0);
    if (thread1.timed_join(td))
    {
        if (cachetodo->size() != 0)
        {
            thread1 = boost::thread(&cacheminder::cachethread, this);
            cout << "NEW THREAD" << endl;
        }
    }
}

void cacheminder::cachethread()
{
    while (true)
    {
        thread1.yield();
        boost::this_thread::interruption_point();
        boost::recursive_mutex::scoped_lock mylock(todomutex);
        if (cachetodo->size() == 0)
        {
            cout << "cache thread: que empty" << endl;
            break;
        }
        if (cachetodo->front()->incache)
        {
            cachetodo->pop_front();
            continue;
        }
        if (cachetodo->front()->cache(false) == false)
        {
            cout << "no free memory" << endl;
            break;
        }
        cout << "cache thread :" << cachetodo->front() << endl;
        cachetodo->pop_front();
    }
    clearcachetodo();
}

void cacheminder::stopcachethread()
{
    thread1.interrupt();
    thread1.join();
}
