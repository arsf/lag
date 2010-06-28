/* 
 * File:   quadtree.h
 * Author: chrfi
 *
 * Created on October 8, 2009, 10:43 AM
 */


#ifndef _QUADTREE_H
#define	_QUADTREE_H


#include "QuadtreeNode.h"
#include "quadtreeexceptions.h"
#include "LidarPointLoader.h"
#include "LidarPointSaver.h"

#include <vector>
#include <sstream>
#include <string>

#include <tr1/unordered_map>


using namespace std;

typedef tr1::unordered_map<uint8_t, string> flighthash;

/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * this class represents a quadtree. all interaction with the quadtree by users (classes or physical users)
 * is performed through this class. it holds metadata about the qaudtree along with a pointer to a single
 * quadtreenode which is the root node. it provides functions to build the tree, load points and the access those points.
 *
 * @note the quadtree generates and uses a unique folder in the /tmp area of the machine it is run on
 * to hold temporary data during exicution, if the program containing the quadtree ends unexpectantly
 * preventing the deconstructor from being called these temporary files will not be cleaned up.
 */
class Quadtree
{
public:
    /**
     * a constructor that builds a new quad tree from a given lidarpointloader object
     *
     * @param loader lidarpointloader object for the file to be loaded
     * @param capacity capacity of each point bucket (the point at which a bucket will overflow and split into 4 smaller buckets
     * @param nth the number of points to skip between each loaded point
     * @param cacheSize the number of points to hold in ram at any one time
     * @param depth the number of pregenerated levels of the quadtree to create during construction
     * @param resolutionBase see \link PointBucket \endlink detailed description for explanation
     * @param numberOfResolutionLevels see \link PointBucket \endlink detailed description for explanation
     * @param errorStream string stream to which error messages will be appended
     */
    Quadtree(LidarPointLoader *loader, int capacity, int nth, int cacheSize, int depth, int resolutionBase, int numberOfResolutionLevels, ostringstream *errorStream = NULL);

    /**
     * a constructor that builds a new quadtree from a given lidarpointloader object using a convex polygon defined by a series of points
     *
     * @param loader lidarpointloader object for the file to be loaded
     * @param capacity capacity of each point bucket (the point at which a bucket will overflow and split into 4 smaller buckets
     * @param nth the number of points to skip between each loaded point
     * @param horizontalCornerValues an array of doubles each of which is the x componant of a point of the polygon (in sequence)
     * @param verticalCornerValues an array of doubles each of which is the y componant of a point of the polygon (in sequence)
     * @param size the number of corners that make up the polygon (the length of the HorizontalCornerValues and VerticalCornerValues arrays)
     * @param cacheSize the number of points to hold in ram at any one time
     * @param depth the number of pregenerated levels of the quadtree to create during construction
     * @param resolutionBase is passed on to pointbuckets, see \link PointBucket \endlink detailed description for explanation
     * @param numberOfResolutionLevels is passed on to pointbuckets, see \link PointBucket \endlink detailed description for explanation
     * @param errorStream string stream to which error messages will be appended
     */
    Quadtree(LidarPointLoader *loader, int capacity, int nth, double *horizontalCornerValues, double *verticalCornerValues, int size, int cacheSize, int depth, int resolutionBase, int numberOfResolutionLevels, ostringstream *errorStream = NULL);


    /**
     * a constructor that builds a new quadtree with user defined dimensions
     *
     * @param boundary a boundary struct specifiying the dimensions desired
     * @param capacity the capacity of each point bucket (the point at which a bucket will overflow and split into 4 smaller buckets
     * @param cacheSize the number of points to hold in ram at any one time
     * @param depth the number of pregenerated levels of the quadtree to create during construction
     * @param resolutionBase is passed on to pointbuckets, see \link PointBucket \endlink detailed description for explanation
     * @param numberOfResolutionLevels is passed on to pointbuckets, see \link PointBucket \endlink detailed description for explanation
     * @param errorStream string stream to which error messages will be appended
     */
    Quadtree(Boundary boundary, int capacity, int cacheSize, int depth, int resolutionBase, int numberOfResolutionLevels, ostringstream *errorStream = NULL);

    /**
     * a constructor that builds a new quadtree with user defined dimensions
     *
     * @param minX X value of the lower left corner of the tree
     * @param minY Y value of the lower left corner of the tree
     * @param maxX X value of the upper right corner of the tree
     * @param maxY Y value of the upper right corner of the tree
     * @param capacity capacity of each point bucket (the point at which a bucket will overflow and split into 4 smaller buckets
     * @param cacheSize the number of points to hold in ram at any one time
     * @param depth the number of pregenerated levels of the quadtree to create during construction
     * @param resolutionBase is passed on to pointbuckets, see \link PointBucket \endlink detailed description for explanation
     * @param numberOfResolutionLevels is passed on to pointbuckets, see \link PointBucket \endlink detailed description for explanation
     * @param errorStream string stream to which error messages will be appended
     */
    Quadtree(double minX, double minY, double maxX, double maxY, int capacity, int cacheSize, int depth, int resolutionBase, int numberOfResolutionLevels, ostringstream *errorStream = NULL);
    ~Quadtree();

    /**
     *  method to insert a new point into the quad tree, throws an exception if the point dosen't fall within the quadtree boundary
     *
     * @param newPoint the new point to be inserted
     */
    bool insert(Point newPoint);

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
     * @param preloadMinimumDepth specifies a depth to which all areas of the quadtree will will be at least as deep (most inportantly the new area covering the points about to be loaded)
     */
    void load(LidarPointLoader *loader, int nth, int preloadMinimumDepth);

    /**
     * a method to load points within a fence defined by a convex polygon from a las file using the loader object given
     *
     * @param loader lidarpointloader object to be used
     * @param nth the number of points to skipp between each point to be loaded
     * @param preloadMinimumDepth specifies a depth to which all areas of the quadtree will will be at least as deep (most inportantly the new area covering the points about to be loaded)
     * @param horizontalCornerValues an array of doubles each of which is the x componant of a point of the polygon (in sequence)
     * @param verticalCornerValues an array of doubles each of which is the y componant of a point of the polygon (in sequence)
     * @param size the number of corners that make up the polygon (the length of the HorizontalCornerValues and VerticalCornerValues arrays)
     */
    void load(LidarPointLoader *loader, int nth, int preloadMinimumDepth, double *horizontalCornerValues, double *verticalCornerValues, int size);

    /**
     * a method to sort the points into accending order within each bucket (this means that they are not
     * sorted globely)
     *
     * @param c defines the type of sort required "H" indicates sort by height (Z) "T" indicates sort by time
     */
    void sort(char c);

    /**
     * a method for forming a subset of the quadtree based on an area given. The fence for the area
     * is provided as a convex polgon.
     *
     *
     * @note this returns point buckets which overlap the area of intrest not points that fall within
     * which means that the pointbuckets may contain points that do not fall within the area.
     * @param horizontalCornerValues an array of doubles each of which is the x componant of a point of the polygon (in sequence)
     * @param verticalCornerValues an array of doubles each of which is the y componant of a point of the polygon (in sequence)
     * @param size the number of corners that make up the polygon (the length of the HorizontalCornerValues and VerticalCornerValues arrays)
     *
     * @return a pointer to a vector, which contains pointers to pointbuckets. this vector must be deleted by the caller of this method
     */
    vector<PointBucket*>* advSubset(double *horizontalCornerValues, double *verticalCornerValues, int size);

    /**
     * a method to get the area the root node covers
     *
     * @return a boundary object defining the area covered by the quadtrees root node (the area of the entire quadtree)
     */
    Boundary* getBoundary();

    /**
     * a method to find the filename associated with a flight line number
     * (the numbers are stored with each point because it takes less memory than the file string)
     *
     * @param flightLineNumber the number of the flight line
     * @return a string containing the path that was used in the lidarpointloader for this flightline
     */
    string getFileName(uint8_t flightLineNumber);

    /**
     * a method to save points into a file, the method of saving and file format are determined
     * in the lidarpointsaver passed not in this method.
     *
     * @param flightLineNumber the flight line number of the flight line that needs to be saved
     * @param saver a lidarpointsaverobject which provides all the functionality to save points into files
     */
    void saveFlightLine(uint8_t flightLineNumber, LidarPointSaver *saver);

    /**
     * a method which increases the depth of all areas of the quadtree by the given amount by spliting nodes
     *
     * @param i the number of additional levels to add to the quadtree
     */
    void increaseDepth(int i);

    /**
     * a method to increase all areas of the quadtree to a minimum depth by splitting nodes,
     * there is no effect on areas allready deeper than this
     *
     * @param i the new minimum depth of the quadtree
     */
    void increaseToMinimumDepth(int i);


private:

    QuadtreeNode *root_;
    QuadtreeNode* guessBucket_;
    int capacity_;
    int flightLineNumber_;
    ostream *errorStream_;
    // hashtable to hold flight file name/flight number pairs
    flighthash flightTable_;
    int prebuildDepth_;
    string instanceDirectory_;
    int resolutionBase_;
    int numberOfResolutionLevels_;
    CacheMinder *MCP_;
    void initiliseValues(int cap, int cacheSize, int depth, int resolutionBase, int numberOfResolutionLevels, ostringstream *errorStream);


    /**
     * a method thats expands the quadtree to encompass new points
     *
     * @note : the returned pointer refers to the new root of the tree using the pointer
     * from oldnode will lead to one of the new roots child nodes
     *
     * @param oldNode the root node of the current quadtree
     * @param newPointsBoundary the boundary of the new points
     *
     * @return pointer to the new root of the tree
     */
    QuadtreeNode* expandBoundary(QuadtreeNode* oldNode, Boundary* newPointsBoundary);

};



#endif	/* _QUADTREE_H */

