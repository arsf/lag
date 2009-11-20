/* 
 * File:   quadtree.h
 * Author: chrfi
 *
 * Created on October 8, 2009, 10:43 AM
 */




#ifndef _QUADTREE_H
#define	_QUADTREE_H

#include <string>
#include "quadloader.h"
#include "quadtreenode.h"
#include <vector>
using namespace std;

class quadtree{
    quadtreenode *root;   
    quadloader *loader;
    int capacity;
    
public:
   // constructor that builds a new tree from a Las file
   quadtree(const char *filename, int cap, int nth);
   // constructor that builds a new tree with user defined dimensions
   quadtree(double sx, double sy, double bx, double by, int cap);
   ~quadtree();
   // inserts a new point into the quad tree, throws an exception if the point dosen't fall within the quadtree boundary
   void insert(point newP);
   // debugging method, prints out the quadtree, only usefull in tiny quadtrees <50 points
   void print();
   // returns true if the quadtree has no points
   bool isEmpty();
   // loads points from a Las file from the filename given, nth defines how many points to
   // ignore per point, ie nth = 9 means every 10th point is loaded, 0 means every point is loaded
   void load(const char *filename, int nth);
   // for a given area this method returns a pointer to a vector which contains
   // pointers to all the buckets that could contain points falling within this area
   vector<pointbucket*>* subset(double minX, double minY, double maxX, double maxY);
   // not implemented yet <----------- WARNING
   point* search(int x,int y,int z);
   // this method sorts the points within each bucket (this means that they are not
   // sorted globely) the parameter defines which point attribute to sort by (accending)
   void sort(char v);
   vector<pointbucket*>* advsubset(double x1, double y1, double x2, double y2, double width);
   boundary* getboundary();
   quadtreenode* expandboundary(quadtreenode* oldnode, boundary* nb);
};



#endif	/* _QUADTREE_H */

