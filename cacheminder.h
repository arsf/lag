/* 
 * File:   cacheminder.h
 * Author: chrfi
 *
 * Created on January 26, 2010, 3:57 PM
 */

#include <queue>
#include "quadtreestructs.h"
#include "boost/thread.hpp"

using namespace std;

#ifndef _CACHEMINDER_H
#define	_CACHEMINDER_H



class cacheminder {
public:
    cacheminder(int cachesize);
    ~cacheminder();
    bool requestcache(int requestsize, pointbucket *pbucket, bool force);
    void releasecache(int release, pointbucket *pbucket);
    void cachelist(vector<pointbucket*> *bucketlist);
    void setcachetodo(vector<pointbucket *> *tasks);
    void pushcachetodo(vector<pointbucket *> *tasks);
    void popcachetodo();
    void deletecachetodo(vector<pointbucket *> *tasks);
    void clearcachetodo();
    void cachethread();
    void startcachethread();
    void stopcachethread();


private:
    deque<pointbucket*> bucketsincache;
    int cacheused;
    int totalcache;
    deque<pointbucket *> *cachetodo;
    boost::recursive_mutex quemutex;
    boost::recursive_mutex todomutex;
    boost::thread thread1;
};

#endif	/* _CACHEMINDER_H */

