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
#include "lidarpointloader.h"
#include <sstream>
#include <string>
#include <tr1/unordered_map> 

using namespace std;

typedef tr1::unordered_map<uint8_t, string> flighthash;

class quadtree{
    quadtreenode *root;   
    int capacity;
    int flightlinenum;
    ostream *errorstream;
    quadtreenode* guessbucket;
    flighthash flighttable;
public:
   
   // constructor that builds a new tree from a loader object
   quadtree(lidarpointloader *l, int cap, int nth, ostringstream *s = NULL);
   
   // constructor that builds a new tree from a specified area of a file
   quadtree(lidarpointloader *l,int cap, int nth, double minX, double minY, double maxX, double maxY, ostringstream *s = NULL);
   
   // constructor that builds a new tree with user defined dimensions
   quadtree(double minX, double minY, double maxX, double maxY, int cap, ostringstream *s = NULL);
   ~quadtree();
   
   // inserts a new point into the quad tree, throws an exception if the point dosen't fall within the quadtree boundary
   bool insert(point newP);
   
   // debugging method, prints out the quadtree, only usefull in tiny quadtrees <50 points
   void print();
   
   // returns true if the quadtree has no points
   bool isEmpty();
   
   // loads points from a Las file using the loader object given, nth defines how many points to
   // ignore per point, ie nth = 9 means every 10th point is loaded, 0 means every point is loaded
   void load(lidarpointloader *l, int nth);
   
   // same as above but only loads points within the area given
   void load(lidarpointloader *l, int nth, double minX, double minY, double maxX, double maxY);
   
   // for a given area this method returns a pointer to a vector which contains
   // pointers to all the buckets that could contain points falling within this area
   // NOTE : this area must be defined by the lower left and up right points of a rectangle
   vector<pointbucket*>* subset(double minX, double minY, double maxX, double maxY);
   
   // not implemented yet <----------- WARNING
   point* search(int x,int y,int z);
   
   // this method sorts the points within each bucket (this means that they are not
   // sorted globely) the parameter defines which point attribute to sort by (accending)
   void sort(char v);
   
   // for a given area this method returns a pointer to a vector which contains
   // pointers to all the buckets that could contain points falling within this area
   // NOTE : this method takes two points which forms a line and uses the width value
   // to work out the actual area defined by the rectangle. if it finds the rectangle is axis aligned
   // it deferes to the subset method which is faster in that case.
   vector<pointbucket*>* advsubset(double x1, double y1, double x2, double y2, double width);
   
   // returns a boundary object representing the area the root node covers
   boundary* getboundary();
   
   //this method takes a flightlinenum and returns the associated filename 
   // (the numbers are stored with each point because it takes less memory)
   string getfilename(uint8_t flightlinenum);
   
private:
   // this method expands the quadtree whose root is oldnode to contain the boundary passed
   // as nb, to do this is creates a new root node above oldnode and makes the oldnode its child.
   // NOTE : the method reassigns oldnode to point at the new root node providing a handle on the
   // new top layer of the tree.
   quadtreenode* expandboundary(quadtreenode* oldnode, boundary* nb);
   
};



#endif	/* _QUADTREE_H */

