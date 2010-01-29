/* 
 * File:   cacheminder.h
 * Author: chrfi
 *
 * Created on January 26, 2010, 3:57 PM
 */

#include <queue>
#include "quadtreestructs.h"
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

private:
    deque<pointbucket*> bucketsincache;
    int cacheused;
    int totalcache;
};

#endif	/* _CACHEMINDER_H */

