/* 
 * File:   quadtreenode.h
 * Author: chrfi
 *
 * Created on October 12, 2009, 3:27 PM
 */

#ifndef _QUADTREENODE_H
#define	_QUADTREENODE_H
#include "quadtreestructs.h"
#include "pointbucket.h"
#include <vector>
#include <string>
using namespace std;


/**
 * this class represents a single node (leaf or nonleaf) within a quadtree.
 *
 * each node is defined by its boundary which places it within the quadtree and indicate which points fall within it.
 * when a node excedes its capacity it creates four new child nodes each a quater the size which are leaves.
 * in this way the number of nodes expands untill all the being inserted are held in non overflowing nodes.
 */
class quadtreenode
{


public:
    static int counter;
    static int overflowcounter;
private:
    friend class quadtree;

    quadtreenode *a, *b, *c, *d;
    double minX, minY, maxX, maxY;
    int capacity;
    int numofpoints;
    pointbucket *bucket;
    bool leaf;
    cacheminder *MCP;
    string instancedirectory;
    int resolutionbase;
    int numresolutionlevels;
    int subset1skip;
    int subset2skip;


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
     * @param cap capacity of the node before it overflows and splits
     * @param MCD the cacheminder for this quadtree instance
     * @param instancedirectory string containing a path to a directory where temporary files will be saved
     * @param resolutionbase is passed on to pointbuckets, see \link pointbucket \endlink detailed description for explanation
     * @param numresolutionlevels is passed on to pointbuckets, see \link pointbucket \endlink detailed description for explanation
     */
    quadtreenode(double minX, double minY, double maxX, double maxY, int cap, cacheminder *MCD, string instancedirectory, int resolutionbase, int numresolutionlevels);


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
     * @param cap capacity of the node before it overflows and splits
     * @param a child node (by convention top left)
     * @param b child node (by convention top right)
     * @param d child node (by convention bottom left)
     * @param c child node (by convention bottom right)
     * @param MCD the cacheminder for this quadtree instance
     * @param instancedirectory string containing a path to a directory where temporary files will be saved
     * @param resolutionbase is passed on to pointbuckets, see \link pointbucket \endlink detailed description for explanation
     * @param numresolutionlevels is passed on to pointbuckets, see \link pointbucket \endlink detailed description for explanation
     */
    quadtreenode(double minX, double minY, double maxX, double maxY, int cap, quadtreenode* a, quadtreenode* b, quadtreenode* c, quadtreenode* d, cacheminder *MCD, string instancedirectory, int resolutionbase, int numresolutionlevels);
    ~quadtreenode();
    

    /**
     * a method to insert a new point into the node
     *
     * @note if the insertion of the point causes the node to overflow then it creates
     * four child nodes and inserts the point into one of thease (along with copying
     * all the points it already contains into the corrisponding children)
     *
     * @param newP the point to be inserted
     *
     * @return false=the point falls within this nodes boundary but the node is not a leaf, true=sucssesfull insertion
     */
    bool insert(point newP);

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
     * @param newP the point to check
     *
     * @return true=within boundary
     */
    bool checkbound(point newP);

    /**
     * a method to get the boundary of the node in a boundary struct
     *
     * @return the boundary of the node
     */
    boundary* getbound();

    /**
     *  this method returns the child into which the point passed fits
     *
     * @param newP the point to check against
     *
     * @return a pointer to the child of this node that the point falls within, if this node is a leaf then NULL is returned
     */
    quadtreenode* pickchild(point newP);



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
     * @param Xs pointer to an array of doubles each of which is the x componant of polygon corner
     * @param Ys pointer to an array of doubles each of which is the y componant of polygon corner
     * @param size the number of corners on the polygon (the size of the Xs and Ys arrays)
     * @param buckets the pointer to a vector to which any correct nodes are added
     */
    void advsubset(double *Xs, double *Ys, int size, vector<pointbucket*> *buckets);

    /**
     * a method that creates 4 child nodes each a quater of the size of this node
     * and copies the points into the child node they fall within
     */
    void splitnode();

    /**
     * a method which increases the depth of all leaf nodes below it by i (splits them then checks)
     */
    void increasedepth(int i);

    /**
     * a method which increases the depth of all leaf nodes below it to a minimum of i
     */
    void increase_to_minimum_depth(int i);

};



#endif	/* _QUADTREENODE_H */

