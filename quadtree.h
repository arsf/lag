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
 * to hold temporary data during exicution, if the program containing the quadtree ends unexpectantly
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
    int prebuilddepth;
    string instancedirectory;
    int resolutionbase;
    int numresolutionlevels;

    void initilisevalues(int cap, int cachesize, int depth, int resolutionbase, int numresolutionlevels, ostringstream *errorstream);
public:
   cacheminder *MCP;

    
   /**
     * a constructor that builds a new quad tree from a given lidarpointloader object
     *
     * @param loader lidarpointloader object for the file to be loaded
     * @param cap capacity of each point bucket (the point at which a bucket will overflow and split into 4 smaller buckets
     * @param nth the number of points to skip between each loaded point
     * @param cachesize the number of points to hold in ram at any one time
     * @param depth the number of pregenerated levels of the quadtree to create during construction
     * @param resolutionbase see \link pointbucket \endlink detailed description for explanation
     * @param numresolutionlevels see \link pointbucket \endlink detailed description for explanation
     * @param errorstream string stream to which error messages will be appended
     */
   quadtree(lidarpointloader *loader, int cap, int nth, int cachesize, int depth, int resolutionbase, int numresolutionlevels, ostringstream *errorstream = NULL);
   
   /**
     * a constructor that builds a new quadtree from a given lidarpointloader object using a convex polygon defined by a series of points
     *
     * @param loader lidarpointloader object for the file to be loaded
     * @param cap capacity of each point bucket (the point at which a bucket will overflow and split into 4 smaller buckets
     * @param nth the number of points to skip between each loaded point
     * @param Xs pointer to an array of the x values for the polgon point series
     * @param Ys pointer to an array of the y values for the polgon point series
     * @param size the number of points in in the polgon therefore the length of Xs Ys arrays
     * @param cachesize the number of points to hold in ram at any one time
    * @param depth the number of pregenerated levels of the quadtree to create during construction
     * @param resolutionbase is passed on to pointbuckets, see \link pointbucket \endlink detailed description for explanation
     * @param numresolutionlevels is passed on to pointbuckets, see \link pointbucket \endlink detailed description for explanation
     * @param errorstream string stream to which error messages will be appended
     */
   quadtree(lidarpointloader *loader, int cap, int nth, double *Xs, double *Ys, int size, int cachesize, int depth, int resolutionbase, int numresolutionlevels, ostringstream *errorstream = NULL);


   /**
    * a constructor that builds a new quadtree with user defined dimensions
    *
    * @param b a boundary struct specifiying the dimensions desired
    * @param cap capacity of each point bucket (the point at which a bucket will overflow and split into 4 smaller buckets
    * @param cachesize the number of points to hold in ram at any one time
    * @param depth the number of pregenerated levels of the quadtree to create during construction
     * @param resolutionbase is passed on to pointbuckets, see \link pointbucket \endlink detailed description for explanation
     * @param numresolutionlevels is passed on to pointbuckets, see \link pointbucket \endlink detailed description for explanation
    * @param errorstream string stream to which error messages will be appended
    */
   quadtree(boundary b, int cap, int cachesize, int depth, int resolutionbase, int numresolutionlevels, ostringstream *errorstream = NULL);

   /**
    * a constructor that builds a new quadtree with user defined dimensions
    *
    * @param minX X value of the lower left corner of the tree
    * @param minY Y value of the lower left corner of the tree
    * @param maxX X value of the upper right corner of the tree
    * @param maxY Y value of the upper right corner of the tree
    * @param cap capacity of each point bucket (the point at which a bucket will overflow and split into 4 smaller buckets
    * @param cachesize the number of points to hold in ram at any one time
    * @param depth the number of pregenerated levels of the quadtree to create during construction
     * @param resolutionbase is passed on to pointbuckets, see \link pointbucket \endlink detailed description for explanation
     * @param numresolutionlevels is passed on to pointbuckets, see \link pointbucket \endlink detailed description for explanation
    * @param errorstream string stream to which error messages will be appended
    */
   quadtree(double minX, double minY, double maxX, double maxY, int cap, int cachesize, int depth, int resolutionbase, int numresolutionlevels, ostringstream *errorstream = NULL);
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
    * @param preloadminimumdepth specifies a depth to which all areas of the quadtree will will be at least as deep (most inportantly the new area covering the points about to be loaded)
    */
   void load(lidarpointloader *loader, int nth, int preloadminimumdepth);
   

  /**
   * a method to load points within a fence defined by a convex polygon from a las file using the loader object given
   *
   * @param loader lidarpointloader object to be used
   * @param nth the number of points to skipp between each point to be loaded
    * @param preloadminimumdepth specifies a depth to which all areas of the quadtree will will be at least as deep (most inportantly the new area covering the points about to be loaded)
   * @param Xs pointer to an array of the x values for the polgon point series
   * @param Ys pointer to an array of the y values for the polgon point series
   * @param size the number of points in in the polgon therefore the length of Xs Ys arrays
   */
   void load(lidarpointloader *loader, int nth, int preloadminimumdepth, double *Xs, double *Ys, int size);


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

      
   /**
    * a method to sort the points into accending order within each bucket (this means that they are not
    * sorted globely)
    *
    * @param v defines the type of sort required "H" indicates sort by height (Z) "T" indicates sort by time
    */
   void sort(char v);
   
   /**
    * a method for forming a subset of the quadtree based on an area given. The fence for the area
    * is provided as a convex polgon.
    *
    *
    * @note this returns point buckets which overlap the area of intrest not points that fall within
    * which means that the pointbuckets may contain points that do not fall within the area.
    * @param Xs a pointer to an array of x values each corrisponding to a corner on the polygon
    * @param Ys a pointer to an array of y values each corrisponding to a corner on the polygon
    * @param size the number of corners on the polygon (the size of the Xs and Ys arrays)
    *
    * @return a pointer to a vector, which contains pointers to pointbuckets. this vector must be deleted by the caller of this method
    */
   vector<pointbucket*>* advsubset(double *Xs, double *Ys, int size);
   
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
    * @return a string containing the path that was used in the lidarpointloader for this flightline
    */
   string getfilename(uint8_t flightlinenum);

   /**
    * a method to save points into a file, the method of saving and file format are determined
    * in the lidarpointsaver passed not in this method.
    *
    * @param flightlinenum the flight line number of the flight line that needs to be saved
    * @param saver a lidarpointsaverobject which provides all the functionality to save points into files
    */
   void saveflightline(uint8_t flightlinenum, lidarpointsaver *saver);

   /**
    * a method which increases the depth of all areas of the quadtree by the given amount by spliting nodes
    *
    * @param i the number of additional levels to add to the quadtree
    */
   void increasedepth(int i);

   /**
    * a method to increase all areas of the quadtree to a minimum depth by splitting nodes,
    * there is no effect on areas allready deeper than this
    *
    * @param i the new minimum depth of the quadtree
    */
   void increase_to_minimum_depth(int i);
   
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

