#include "quadtreestructs.h"

// self explanitory :P
pointbucket::pointbucket(int cap, double minx, double miny, double maxx, double maxy)
{
   numberofpoints = 0;
   points = new point[cap];
   this->minx = minx;
   this->miny = miny;
   this->maxx = maxx;
   this->maxy = maxy;
}

// self explanitory :P
pointbucket::~pointbucket()
{
   delete[] points;
}