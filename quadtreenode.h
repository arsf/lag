/* 
 * File:   quadtreenode.h
 * Author: chrfi
 *
 * Created on October 12, 2009, 3:27 PM
 */

#ifndef _QUADTREENODE_H
#define	_QUADTREENODE_H
#include "quadtreestructs.h"
#include <vector>
#include <string>
using namespace std;

class quadtreenode{
   quadtreenode *a, *b, *c, *d;
   double minX, minY, maxX, maxY;
   int capacity;
   int numofpoints;
   pointbucket *bucket;
   bool leaf;
   cacheminder *MCP;

   void addsubset(double x1, double y1, double x2, double y2, double x3, double y3 ,double x4, double y4, std::vector<pointbucket*> *buckets);
public:
    // basic constructor allowing boundary and capacity to be specified
    quadtreenode(double minX, double minY, double maxX, double maxY, int cap, cacheminder *MCD);
    // in addition to the basic constructor this one allows each of the child nodes to be specified
    quadtreenode(double minX, double minY, double maxX, double maxY, int cap, quadtreenode* a, quadtreenode* b, quadtreenode* c, quadtreenode* d, cacheminder *MCD);
    ~quadtreenode();
    
    // this method inserts a new point into the node
    bool insert(point newP);
    
    // this method prints a debug view of the tree
    void print();
    
    // this method checks if the node is a leaf
    bool isLeaf();
    
    // this method checks if a given point is within the boundary of the node
    bool checkbound(point newP);
    
    // this method returns the boundary of the node in a boundary struct
    boundary* getbound();
    
    // this method returns the child into which the point passed fits
    quadtreenode* pickchild(point newP);
    
    // this method adds the pointbucket to the passed vector if the node falls
    // within the boundary given and is recursivly called on all child nodes
    void subset(double minX, double minY, double maxX, double maxY, std::vector<pointbucket*> *buckets);
    
    // this method sorts the bucket in the node and recurivly in all child nodes
    // using the point comparison function passed
    void sort(int ( * comparator ) ( const void *, const void * ));
    
    // checks if the node is empty
    bool isEmpty(); 
    
    // this method adds the pointbucket to the passed vector if the node falls
    // within the boundary given and is recursivly called on all child nodes
    void advsubset(double x1, double y1, double x2, double y2, double x3, double y3 ,double x4, double y4, std::vector<pointbucket*> *buckets);
};


#endif	/* _QUADTREENODE_H */

