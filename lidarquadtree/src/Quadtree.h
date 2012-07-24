/* 
 * File:   Quadtree.h
 * Authors: chrfi, jaho
 *
 * Created on October 8, 2009, 10:43 AM
 *
 * ----------------------------------------------------------------
 *
 * This file is part of lidarquadtree, a library providing a data
 * structure for storing and indexing LiDAR points.
 *
 * Copyright (C) 2008-2012 Plymouth Marin Laboratory
 *    arsfinternal@pml.ac.uk
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * The GNU General Public License is contained in the file COPYING. *
 */

#ifndef _QUADTREE_H
#define	_QUADTREE_H

#include <vector>
#include <sstream>
#include <string>
#include <tr1/unordered_map>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include "QuadtreeNode.h"
#include "QuadtreeExceptions.h"
#include "CacheMinder.h"

using namespace std;
namespace fs = boost::filesystem;

// Keep strings for now - may be fixed downstream per OS
//typedef tr1::unordered_map<uint8_t, fs::path> flighthash;
typedef tr1::unordered_map<uint8_t, fs::path> flighthash;


/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * this class represents a quadtree. all interaction with the quadtree 
 * by users (classes or physical users) is performed through this class. 
 * it holds metadata about the qaudtree along with a pointer to a single
 * quadtreenode which is the root node. it provides functions to build 
 * the tree, load points and the access those points.
 *
 * @note the quadtree generates and uses a unique folder in the /tmp 
 * area of the machine it is run on to hold temporary data during 
 * exicution, if the program containing the quadtree ends unexpectantly
 * preventing the deconstructor from being called these temporary files 
 * will not be cleaned up.
 */

class Quadtree
{
public:
	Quadtree()
	{}

    /**
     * a constructor that builds a new quadtree with user defined dimensions
     *
     * @param boundary a boundary struct specifiying the dimensions desired
     * @param capacity the capacity of each point bucket (the point at which 
     * a bucket will overflow and split into 4 smaller buckets
     * @param cacheSize the number of points to hold in ram at any one time
     * @param depth the number of pregenerated levels of the quadtree to 
     * create during construction
     * @param resolutionBase is passed on to pointbuckets, see \link 
     * PointBucket \endlink detailed description for explanation
     * @param numberOfResolutionLevels is passed on to pointbuckets, 
     * see \link PointBucket \endlink detailed description for explanation
     * @param errorStream string stream to which error messages will be appended
     */
    Quadtree(Boundary *boundary, int capacity, int cacheSize, int depth, 
             int resolutionBase, int numberOfResolutionLevels, 
             fs::path cacheFolder);

    /**
     * a constructor that builds a new quadtree with user defined dimensions
     *
     * @param minX X value of the lower left corner of the tree
     * @param minY Y value of the lower left corner of the tree
     * @param maxX X value of the upper right corner of the tree
     * @param maxY Y value of the upper right corner of the tree
     * @param capacity capacity of each point bucket (the point at which a 
     * bucket will overflow and split into 4 smaller buckets
     * @param cacheSize the number of points to hold in ram at any one time
     * @param depth the number of pregenerated levels of the quadtree to create 
     * during construction
     * @param resolutionBase is passed on to pointbuckets, see \link 
     * PointBucket \endlink detailed description for explanation
     * @param numberOfResolutionLevels is passed on to pointbuckets, see 
     * \link PointBucket \endlink detailed description for explanation
     * @param errorStream string stream to which error messages will be appended
     */
    Quadtree(double minX, double minY, double maxX, double maxY, 
             int capacity, int cacheSize, int depth, int resolutionBase, 
             int numberOfResolutionLevels, fs::path cacheFolder);

    ~Quadtree();

    /**
     *  method to insert a new point into the quad tree, throws an exception 
     *  if the point dosen't fall within the quadtree boundary
     *
     * @param newPoint the new point to be inserted
     */
    bool insert(const LidarPoint& newPoint);

    /**
     * Method to insert a number of points at once into the quadtree.
     *
     * @param points_array - an array of LidarPoint objects.
     * @param points_number - number of points in the array.
     */
     void insert_points(LidarPoint const* points_array, int points_number);

    /**
     * debugging method, prints out the quadtree, only usefull in tiny 
     * quadtrees less than 50 points
     */
    void print();

    /**
     *  method to check wether the quadtree contains any points
     *
     * @return true if the quadtree contains no points
     */
    bool isEmpty();

    /**
     * a method to sort the points into accending order within each bucket 
     * (this means that they are not
     * sorted globely)
     *
     * @param c defines the type of sort required "H" indicates sort by 
     * height (Z) "T" indicates sort by time
     */
    void sort(char c);

    /**
     * a method for forming a subset of the quadtree based on an area given. 
     * The fence for the area is provided as a convex polgon.
     *
     *
     * @note this returns point buckets which overlap the area of intrest 
     * not points that fall within which means that the pointbuckets may 
     * contain points that do not fall within the area.
     * @param horizontalCornerValues an array of doubles each of which 
     * is the x component of a point of the polygon (in sequence)
     * @param verticalCornerValues an array of doubles each of which is 
     * the y component of a point of the polygon (in sequence)
     * @param size the number of corners that make up the polygon (the 
     * length of the HorizontalCornerValues and VerticalCornerValues arrays)
     *
     * @return a pointer to a vector, which contains pointers to pointbuckets. 
     * this vector must be deleted by the caller of this method
     */
    vector<PointBucket*>* advSubset(vector<double> horizontalCornerValues,
                                    vector<double> verticalCornerValues, int size);

    // For backwards compatibility
    vector<PointBucket*>* advSubset(double* horizontalCornerValues,
                                        double* verticalCornerValues, int size);

    // For convenience - returns a vector with all pointbuckets
    vector<PointBucket*>* getAllBuckets();

    /**
     * a method to get the area the root node covers
     *
     * @return a boundary object defining the area covered by the quadtrees 
     * root node (the area of the entire quadtree)
     */
    Boundary* getBoundary();

    /**
     * a method to find the filename associated with a flight line number
     * (the numbers are stored with each point because it takes less memory 
     * than the file string)
     *
     * @param flightLineNumber the number of the flight line
     * @return a string containing the path that was used in the 
     * lidarpointloader for this flightline
     */
    string getFileName(uint8_t flightLineNumber);

    /**
     * Method to add a flightline to the flight table.
     *
     * @param filename - name of the file loaded.
     */
    void addFlightline(std::string filename);

    /**
     * a method which increases the depth of all areas of the quadtree by 
     * the given amount by spliting nodes
     *
     * @param i the number of additional levels to add to the quadtree
     */
    void increaseDepth(int i);

    /**
     * a method to increase all areas of the quadtree to a minimum depth 
     * by splitting nodes,
     * there is no effect on areas allready deeper than this
     *
     * @param i the new minimum depth of the quadtree
     */
    void increaseToMinimumDepth(int i);

    void adjustBoundary(Boundary boundary);

    int getNumberOfPoints();

    int get_capacity()
    {
    	return this->capacity_;
    }

    int getFlightlinesNumber()
    {
    	return this->flightLineNumber_;
    }

protected:

    QuadtreeNode *root_;
    QuadtreeNode *guessBucket_;
    int capacity_;
    int flightLineNumber_;
    ostream *errorStream_;
    // hashtable to hold flight file name/flight number pairs
    flighthash flightTable_;
    int prebuildDepth_;
    //string instanceDirectory_;
    fs::path instanceDirectory_;
    int resolutionBase_;
    int numberOfResolutionLevels_;
    CacheMinder *MCP_;
    int numberOfPoints_;
    void initiliseValues(int cap, int cacheSize, int depth, 
                         int resolutionBase, int numberOfResolutionLevels, 
                         ostringstream *errorStream, fs::path cacheFolder);
    void emptyTreeInit(double minX, double minY, double maxX, double maxY, 
                       int cap, int cacheSize, int depth, int resolutionBase, 
                       int numberOfResolutionLevels, 
                       ostringstream *errorStream, fs::path cacheFolder);

    /**
     * a method thats expands the quadtree to encompass new points
     *
     * @note : the returned pointer refers to the new root of the tree 
     * using the pointer
     * from oldnode will lead to one of the new roots child nodes
     *
     * @param oldNode the root node of the current quadtree
     * @param newPointsBoundary the boundary of the new points
     *
     * @return pointer to the new root of the tree
     */
    QuadtreeNode* expandBoundary(QuadtreeNode* oldNode, 
                                 Boundary* newPointsBoundary);

};



#endif	/* _QUADTREE_H */

