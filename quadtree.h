/* 
 * File:   quadtree.h
 * Author: chrfi
 *
 * Created on October 8, 2009, 10:43 AM
 */




#ifndef _QUADTREE_H
#define	_QUADTREE_H


#include "quadtreenode.h"
#include <vector>
#include "lidarpointloader.h"
#include "lidarpointsaver.h"
#include <sstream>
#include <string>
#include <tr1/unordered_map>
#include "quadtreeexceptions.h"

using namespace std;

typedef tr1::unordered_map<uint8_t, string> flighthash;

/**
 * this class represents a quadtree. all interaction with the quadtree by users (classes or physical users)
 * is performed through this class. it holds metadata about the qaudtree along with a pointer to a single
 * quadtreenode which is the root node. it provides functions to build the tree, load points and the access those points.
 *
 * @note the quadtree generates and uses a unique folder in the /tmp area of the machine it is run on
 * to hold temporary data during exicution, if the program containing the quadtree end unexpectantly
 * preventing the deconstructor from being called these temporary files will not be cleaned up.
 */
class quadtree{
    quadtreenode *root;   
    int capacity;
    int flightlinenum;
    ostream *errorstream;
    quadtreenode* guessbucket;
    // hashtable to hold flight file name/flight number pairs
    flighthash flighttable;
    cacheminder *MCP;
    string instancedirectory;
public:
   
   /**
     * a constructor that builds a new quad tree from a given lidarpointloader object
     *
     * @param loader lidarpointloader object for the file to be loaded
     * @param cap capacity of each point bucket (the point at which a bucket will overflow and split into 4 smaller buckets
     * @param nth the number of points to skip between each loaded point
     * @param cachesize the number of points to hold in ram at any one time
     * @param errorstream string stream to which error messages will be appended
     */
   quadtree(lidarpointloader *loader, int cap, int nth, int cachesize, ostringstream *errorstream = NULL);
   
   /**
     * a constructor that builds a new quadtree from a given lidarpointloader object using a fence defined by two points and a width (allowing any orientation
     *
     * @param loader lidarpointloader object for the file to be loaded
     * @param cap capacity of each point bucket (the point at which a bucket will overflow and split into 4 smaller buckets
     * @param nth the number of points to skip between each loaded point
     * @param x1 the X position of the first point
     * @param y1 the Y position of the first point
     * @param x2 the X position of the second point
     * @param y2 the Y position of the second point
     * @param width the width of the rectangle (width/2 will be added to each side of the line)
     * @param cachesize the number of points to hold in ram at any one time
     * @param errorstream string stream to which error messages will be appended
     */
   quadtree(lidarpointloader *loader,int cap, int nth, double x1, double y1, double x2, double y2, double width, int cachesize, ostringstream *errorstream = NULL);



   /**
    * a constructor that builds a new quadtree with user defined dimensions
    *
    * @param minX X value of the lower left corner of the tree
    * @param minY Y value of the lower left corner of the tree
    * @param maxX X value of the upper right corner of the tree
    * @param maxY Y value of the upper right corner of the tree
    * @param cap capacity of each point bucket (the point at which a bucket will overflow and split into 4 smaller buckets
    * @param cachesize the number of points to hold in ram at any one time
    * @param errorstream string stream to which error messages will be appended
    */
   quadtree(double minX, double minY, double maxX, double maxY, int cap, int cachesize, ostringstream *errorstream = NULL);
   ~quadtree();
   

   /**
    *  method to insert a new point into the quad tree, throws an exception if the point dosen't fall within the quadtree boundary
    * 
    * @param newP the new point to be inserted
    */
   bool insert(point newP);
   
   /**
    * debugging method, prints out the quadtree, only usefull in tiny quadtrees less than 50 points
    */
   void print();
   
   /**
    *  method to check wether the quadtree contains any points
    *
    * @return true if the quadtree contains no points
    */
   bool isEmpty();
   
   /**
    * method to load points from a Las file using the loader object given
    *
    * @param loader lidarpointloader object to be used
    * @param nth the number of points to skipp between each point to be loaded
    */
   void load(lidarpointloader *loader, int nth);
   
   /**
    * a method to load points from a Las file using the loader object given
    *
    * @param loader lidarpointloader object to be used
    * @param nth the number of points to skipp between each point to be loaded
    * @param minX X value of the lower left corner of the fence
    * @param minY Y value of the lower left corner of the fence
    * @param maxX X value of the upper right corner of the fence
    * @param maxY Y value of the upper right corner of the fence
    */
   void load(lidarpointloader *loader, int nth, double minX, double minY, double maxX, double maxY);

   void load(lidarpointloader *loader, int nth, double x1, double y1, double x2, double y2, double width);
   /**
    * a method for forming a subset of the quadtree based on an area given. This subset is a collection of buckets
    * that are in some area within the subset, this means that the subset contains points that
    * are not within the boundary given.
    *
    * @param minX X value of the lower left corner of the subset
    * @param minY Y value of the lower left corner of the subset
    * @param maxX X value of the upper right corner of the subset
    * @param maxY Y value of the upper right corner of the subset
    *
    * @return a pointer to a vector, which contains pointers to buckets. this must be deleted by the caller of this method
    */
   vector<pointbucket*>* subset(double minX, double minY, double maxX, double maxY);

   /*
    * placeholder
    */
   vector<pointbucket*>* uncachedsubset(double minX, double minY, double maxX, double maxY);
   
   /*
    * placeholder
    */
   point* search(int x,int y,int z);
   
   /**
    * a method to sort the points into accending order within each bucket (this means that they are not
    * sorted globely)
    *
    * @param v defines the type of sort required "H" indicates sort by height (Z)
    */
   void sort(char v);
   
   /**
    * a method for forming a subset of the quadtree based on an area given. This subset is a collection of buckets
    * that are in some area within the subset, this means that the subset contains points that
    * are not within the boundary given.
    *
    * @note this method takes two points which form a line and uses the width value
    * to work out the actual area defined by the rectangle centered on that line. if it finds the rectangle is axis aligned
    * it deferes to the subset method which is faster in that case.
    *
    * @param x1 the X position of the first point
    * @param y1 the Y position of the first point
    * @param x2 the X position of the second point
    * @param y2 the Y position of the second point
    * @param width the width of the rectangle (width/2 will be added to each side of the line)
    *
    * @return a pointer to a vector, which contains pointers to buckets. this must be deleted by the caller of this method
    */
   vector<pointbucket*>* advsubset(double x1, double y1, double x2, double y2, double width);
   
   /**
    * a method to get the area the root node covers
    *
    * @return a boundary object defining the area covered by the quadtrees root node (the area of the entire quadtree)
    */
   boundary* getboundary();
   
   /**
    * a method to find the filename associated with a flight line number
    * (the numbers are stored with each point because it takes less memory than the file string)
    *
    * @param flightlinenum the number of the flight line
    *
    * @return a string containg the path that was used in the lidarpointloader for this flightline
    */
   string getfilename(uint8_t flightlinenum);

   void saveflightline(uint8_t flightlinenum, lidarpointsaver *saver);
   
private:
   /**
     * a method thats expands the quadtree to encompass new points
     *
     * @note : the returned pointer refers to the new root of the tree using the pointer
     * from oldnode will lead to one of the new roots child nodes
     *
     * @param oldnode the root node of the current quadtree
     * @param nb the boundary of the new points
     *
     * @return pointer to the new root of the tree
     */
   quadtreenode* expandboundary(quadtreenode* oldnode, boundary* nb);
   
};



#endif	/* _QUADTREE_H */

