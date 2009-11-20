#include "quadtreestructs.h"

// self explanitory :P
pointbucket::pointbucket(int cap)
{
   numberofpoints = 0;
   points = new point[cap];
}

// self explanitory :P
pointbucket::~pointbucket()
{
   delete[] points;
}