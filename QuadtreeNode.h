/* 
 * File:   quadtreenode.h
 * Author: Christopher Stanley Finerty (chrfi)
 * doxygen taged
 */

#ifndef _QUADTREENODE_H
#define	_QUADTREENODE_H

#include "quadtreestructs.h"
#include "PointBucket.h"

#include <vector>
#include <string>

using namespace std;


/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * this class represents a single node (leaf or nonleaf) within a quadtree.
 *
 * each node is defined by its boundary which places it within the quadtree and indicate which points fall within it.
 * when a node excedes its capacity it creates four new child nodes each a quater the size which are leaves.
 * in this way the number of nodes expands untill all the being inserted are held in non overflowing nodes.
 */
class QuadtreeNode
{


public:
private:
    friend class Quadtree;

    QuadtreeNode *a_, *b_, *c_, *d_;
    double minX_, minY_, maxX_, maxY_;
    int capacity_;
    int numberOfPoints_;
    PointBucket *bucket_;
    bool leaf_;
    CacheMinder *MCP_;
    string instanceDirectory_;
    int resolutionBase_;
    int numberOfResolutionLevels_;


    /**
     * constructor allowing the boundary of the new node to be defined and sets other attributes to default values
     *
     * @note this constructor should be used with extreme caution. creation of nodes is normally handled by the quadtree
     * and any difference in the way the boundarys are calculated could lead to points falling between gaps or nodes overlaping.
     *
     * @param minX X value of the lower left corner of the boundary
     * @param minY Y value of the lower left corner of the boundary
     * @param maxX X value of the upper right corner of the boundary
     * @param maxY Y value of the upper right corner of the boundary
     * @param capacity the capacity of the node before it overflows and splits
     * @param MCD the cacheminder for this quadtree instance
     * @param instanceDirectory string containing a path to a directory where temporary files will be saved
     * @param resolutionBase is passed on to pointbuckets, see \link PointBucket \endlink detailed description for explanation
     * @param numberOfResolutionLevels is passed on to pointbuckets, see \link PointBucket \endlink detailed description for explanation
     */
    QuadtreeNode(double minX, double minY, double maxX, double maxY, int capacity, CacheMinder *MCD, string instanceDirectory, int resolutionBase, int numberOfResolutionLevels);


    /**
     * constructor allowing the boundary of the new node to be defined and its children specified
     *
     * @note this constructor should be used with extreme caution. creation of nodes is normally handled by the quadtree
     * and any difference in the way the boundarys are calculated could lead to points falling between gaps or nodes overlaping.
     *
     * @param minX X value of the lower left corner of the boundary
     * @param minY Y value of the lower left corner of the boundary
     * @param maxX X value of the upper right corner of the boundary
     * @param maxY Y value of the upper right corner of the boundary
     * @param capacity the capacity of the node before it overflows and splits
     * @param a child node (by convention top left)
     * @param b child node (by convention top right)
     * @param d child node (by convention bottom left)
     * @param c child node (by convention bottom right)
     * @param MCD the cacheminder for this quadtree instance
     * @param instanceDirectory string containing a path to a directory where temporary files will be saved
     * @param resolutionBase is passed on to pointbuckets, see \link PointBucket \endlink detailed description for explanation
     * @param numberOfResolutionLevels is passed on to pointbuckets, see \link PointBucket \endlink detailed description for explanation
     */
    QuadtreeNode(double minX, double minY, double maxX, double maxY, int capacity, QuadtreeNode* a, QuadtreeNode* b, QuadtreeNode* c, QuadtreeNode* d, CacheMinder *MCD, string instanceDirectory, int resolutionBase, int numberOfResolutionLevels);
    ~QuadtreeNode();
    

    /**
     * a method to insert a new point into the node
     *
     * @note if the insertion of the point causes the node to overflow then it creates
     * four child nodes and inserts the point into one of thease (along with copying
     * all the points it already contains into the corrisponding children)
     *
     * @param newPoint the point to be inserted
     *
     * @return false=the point falls within this nodes boundary but the node is not a leaf, true=sucssesfull insertion
     */
    bool insert(Point newPoint);

    /**
     *  a method to print a debug view of the node to cout
     */
    void print();

    /**
     *  a method to check if the node is a leaf
     * 
     * @return true=leaf
     */
    bool isLeaf();


    /**
     *  this method checks if a given point is within the boundary of the node
     *
     * @param newPoint the point to check
     *
     * @return true=within boundary
     */
    bool checkBoundary(Point newPoint);

    /**
     * a method to get the boundary of the node in a boundary struct
     *
     * @return the boundary of the node
     */
    Boundary* getBoundary();

    /**
     *  this method returns the child into which the point passed fits
     *
     * @param newPoint the point to check against
     *
     * @return a pointer to the child of this node that the point falls within, if this node is a leaf then NULL is returned
     */
    QuadtreeNode* pickChild(Point newPoint);



    /**
     *  a method that sorts the bucket in the node and recurivly in all child nodes
     * using the point comparison function passed
     *
     * @param comparator a comparison function for two points that returns an int
     */
    void sort(int ( * comparator) (const void *, const void *));

    /**
     *  checks if the node is empty
     *
     * @return true=empty
     */
    bool isEmpty();

    /**
     * a method to add pointbuckets from nodes that fall within a given boundary to a vector. this method
     * is recursive so if the node is not a leaf it simply calls this method in its child nodes
     * this means that every node below the node you first call it on is checked.
     *
     * @note this method creates the search area by drawing a line from point 1 to 2 to 3 to 4.
     * and searches the area within these lines these lines MUST describe a convex polygon
     *
     * @param horizontalCornerValues an array of doubles each of which is the x componant of a point of the polygon (in sequence)
     * @param verticalCornerValues an array of doubles each of which is the y componant of a point of the polygon (in sequence)
     * @param size the number of corners that make up the polygon (the length of the HorizontalCornerValues and VerticalCornerValues arrays)
     * @param buckets the pointer to a vector to which any correct nodes are added
     */
    void advSubset(double *horizontalCornerValues, double *verticalCornerValues, int size, vector<PointBucket*> *buckets);

    /**
     * a method that creates 4 child nodes each a quater of the size of this node
     * and copies the points into the child node they fall within
     */
    void splitNode();

    /**
     * a method which increases the depth of all leaf nodes below it by i (splits them then checks)
     */
    void increaseDepth(int i);

    /**
     * a method which increases the depth of all leaf nodes below it to a minimum of i
     */
    void increaseToMinimumDepth(int i);

};



#endif	/* _QUADTREENODE_H */

