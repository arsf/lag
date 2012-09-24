/*
 * File:   Quadtree.cpp
 * Authors: chrfi, jaho, Berin Smaldon
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

#include <stdlib.h>
#include <sstream>
#include <cmath>
#include <iostream>
#include <cstdio>

#ifdef __WIN32
#else
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#endif

#include "time.h"
#include "Quadtree.h"
#include "CollisionDetection.h"
#include "QuadtreeExceptions.h"

using namespace std;

#ifndef __WIN32
#define DEFAULT_TEMP_DIRECTORY "/tmp"
#define FILE_SEPERATOR_STRING "/"
#else
#define DEFAULT_TEMP_DIRECTORY "C:\\Temp"
#define FILE_SEPERATOR_STRING "\\"
#endif

Quadtree::Quadtree(Boundary *b, int cap, int cacheSize, int depth,
		int resolutionBase, int numberOfResolutionLevels,
		string cacheFolder)
{
	emptyTreeInit(b->minX, b->minY, b->maxX, b->maxY, cap, cacheSize, depth,
			resolutionBase, numberOfResolutionLevels, NULL, cacheFolder);
}

Quadtree::Quadtree(double minX, double minY, double maxX, double maxY, int cap,
		int cacheSize, int depth, int resolutionBase,
		int numberOfResolutionLevels, string cacheFolder)
{
	emptyTreeInit(minX, minY, maxX, maxY, cap, cacheSize, depth, resolutionBase,
			numberOfResolutionLevels, NULL, cacheFolder);

}

Quadtree::Quadtree(double minX, double minY, double maxX, double maxY, int cap,
		int cacheSize, int depth, int resolutionBase,
		int numberOfResolutionLevels)
{
	emptyTreeInit(minX, minY, maxX, maxY, cap, cacheSize, depth, resolutionBase,
			numberOfResolutionLevels, NULL, string(DEFAULT_TEMP_DIRECTORY));
}

void Quadtree::initiliseValues(int capacity, int cacheSize, int depth,
		int resolutionBase, int numberOfResolutionLevels,
		ostringstream *errorStream, string cacheFolder)
{
	if (resolutionBase < 1)
	{
		throw "invalid resolution base";
	}
	if (numberOfResolutionLevels < 1)
	{
		throw "invalid number of resolution levels";
	}
	if (errorStream == NULL)
	{
		// if no stringstream is given it defaults to the error stream
		this->errorStream_ = &cerr;
	}
	else
	{
		this->errorStream_ = errorStream;
	}

	capacity_ = capacity;
	this->resolutionBase_ = resolutionBase;
	this->numberOfResolutionLevels_ = numberOfResolutionLevels;
	root_ = NULL;
	guessBucket_ = NULL;
   
	// assuming the OS provides a suitable unique path
#ifndef __WIN32
   char instance_template [] = "/quadtree_XXXXXX";
   mktemp(instance_template);
#else
   char instance_template [] = "\\quadtree_XXXXXX";
   _mktemp(instance_template);
#endif

   instanceDirectory_ = cacheFolder + instance_template;
#ifndef __WIN32
   if (mkdir(instanceDirectory_.c_str(), S_IRWXU))
#else
   if (CreateDirectory(instanceDirectory_.c_str(), NULL) == 0)
#endif
   {
      throw QuadtreeIOException();
   }

	MCP_ = new CacheMinder(cacheSize);
	flightLineNumber_ = 0;
	prebuildDepth_ = depth;
	numberOfPoints_ = 0;
}

void Quadtree::emptyTreeInit(double minX, double minY, double maxX, double maxY,
		int cap, int cacheSize, int depth, int resolutionBase,
		int numberOfResolutionLevels, ostringstream *errorStream, string cacheFolder)
{
	initiliseValues(cap, cacheSize, depth, resolutionBase,
			numberOfResolutionLevels, errorStream, cacheFolder);

	root_ = new QuadtreeNode(minX, minY, maxX, maxY, capacity_, MCP_,
			instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);

	root_->increaseDepth(depth);
}

QuadtreeNode* Quadtree::expandBoundary(QuadtreeNode* oldNode,
		Boundary* newpointsBoundary)
{
	Boundary* b = oldNode->getBoundary();

	// if the node already covers the area just return the node
	if (b->maxX >= newpointsBoundary->maxX && b->maxY >= newpointsBoundary->maxY
			&& b->minX <= newpointsBoundary->minX
			&& b->minY <= newpointsBoundary->minY)
	{
		delete b;
		return oldNode;
	}

	double newbx1 = b->minX;
	double newby1 = b->minY;
	double newbx2 = b->maxX;
	double newby2 = b->maxY;

	// find the Boundary that encompasses the old node and the new Boundary
	if (newpointsBoundary->minX < newbx1)
	{
		newbx1 = newpointsBoundary->minX;
	}
	if (newpointsBoundary->minY < newby1)
	{
		newby1 = newpointsBoundary->minY;
	}
	if (newpointsBoundary->maxX > newbx2)
	{
		newbx2 = newpointsBoundary->maxX;
	}
	if (newpointsBoundary->maxY > newby2)
	{
		newby2 = newpointsBoundary->maxY;
	}

	// work out the center Point of the new Boundary
	double cx = newbx1 + ((newbx2 - newbx1) / 2);
	double cy = newby1 + ((newby2 - newby1) / 2);

	// find the distance from each of the 4 corners of the nodes Boundary
	// to the new center Point (work out the general location of the node
	// within the new Boundary (top left, top right etc))
	double topleftdistance = sqrt(
			pow(abs(cx - b->minX), 2) + pow((abs(cy - b->maxY)), 2));
	double toprightdistance = sqrt(
			pow(abs(cx - b->maxX), 2) + pow((abs(cy - b->maxY)), 2));
	double bottomleftdistance = sqrt(
			pow(abs(cx - b->minX), 2) + pow((abs(cy - b->minY)), 2));
	double bottomrightdistance = sqrt(
			pow(abs(cx - b->maxX), 2) + pow((abs(cy - b->minY)), 2));

	// if the old node is in the bottom right
	if (topleftdistance <= toprightdistance
			&& topleftdistance <= bottomleftdistance
			&& topleftdistance <= bottomrightdistance)
	{
		// create nodes that divide up the new Boundary with the dividing lines
		// passing through the top left corner of the old node
		QuadtreeNode* tl = new QuadtreeNode(newbx1, b->maxY, b->minX, newby2,
				capacity_, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
		QuadtreeNode* tr = new QuadtreeNode(b->minX, b->maxY, newbx2, newby2,
				capacity_, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
		QuadtreeNode* bl = new QuadtreeNode(newbx1, newby1, b->minX, b->maxY,
				capacity_, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);

		Boundary* subBoundary = new Boundary;
		subBoundary->minX = b->minX;
		subBoundary->minY = newby1;
		subBoundary->maxX = newbx2;
		subBoundary->maxY = b->maxY;

		// the old node then needs to be expanded into its new quarter (this
		// is to deal with instances where the old node only touches one side
		// of the new Boundary and therefore only fills half its new quarter.
		QuadtreeNode* br = expandBoundary(oldNode, subBoundary);
		delete subBoundary;
		delete b;
		// create a new node above the old containing the 3 new child nodes
		// and the expaned old node
		return new QuadtreeNode(newbx1, newby1, newbx2, newby2, capacity_, tl,
				tr, bl, br, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
	}

	// if the old node is in the bottom left
	if (toprightdistance <= topleftdistance
			&& toprightdistance <= bottomleftdistance
			&& toprightdistance <= bottomrightdistance)
	{
		// create nodes that divide up the new Boundary with the dividing
		// lines passing through the top right corner of the old node
		QuadtreeNode* tl = new QuadtreeNode(newbx1, b->maxY, b->maxX, newby2,
				capacity_, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
		QuadtreeNode* tr = new QuadtreeNode(b->maxX, b->maxY, newbx2, newby2,
				capacity_, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);

		Boundary* subBoundary = new Boundary;
		subBoundary->minX = newbx1;
		subBoundary->minY = newby1;
		subBoundary->maxX = b->maxX;
		subBoundary->maxY = b->maxY;

		// the old node then needs to be expanded into its new quarter
		QuadtreeNode* bl = expandBoundary(oldNode, subBoundary);
		delete subBoundary;
		QuadtreeNode* br = new QuadtreeNode(b->maxX, newby1, newbx2, b->maxY,
				capacity_, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
		delete b;
		return new QuadtreeNode(newbx1, newby1, newbx2, newby2, capacity_, tl,
				tr, bl, br, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
	}

	// if the old node is in the top right
	if (bottomleftdistance <= topleftdistance
			&& bottomleftdistance <= toprightdistance
			&& bottomleftdistance <= bottomrightdistance)
	{
		// create nodes that divide up the new Boundary with the dividing lines
		// passing through the bottom left corner of the old node
		QuadtreeNode* tl = new QuadtreeNode(newbx1, b->minY, b->minX, newby2,
				capacity_, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);

		Boundary* subBoundary = new Boundary;
		subBoundary->minX = b->minX;
		subBoundary->minY = b->minY;
		subBoundary->maxX = newbx2;
		subBoundary->maxY = newby2;

		// the old node then needs to be expanded into its new quarter
		QuadtreeNode* tr = expandBoundary(oldNode, subBoundary);
		delete subBoundary;
		QuadtreeNode* bl = new QuadtreeNode(newbx1, newby1, b->minX, b->minY,
				capacity_, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
		QuadtreeNode* br = new QuadtreeNode(b->minX, newby1, newbx2, b->minY,
				capacity_, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
		delete b;
		return new QuadtreeNode(newbx1, newby1, newbx2, newby2, capacity_, tl,
				tr, bl, br, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
	}

	// if the old node is in the top left
	if (bottomrightdistance <= topleftdistance
			&& bottomrightdistance <= toprightdistance
			&& bottomrightdistance <= bottomleftdistance)
	{
		Boundary* subBoundary = new Boundary;
		subBoundary->minX = newbx1;
		subBoundary->minY = b->minY;
		subBoundary->maxX = b->maxX;
		subBoundary->maxY = newby2;

		// the old node then needs to be expanded into its new quarter
		QuadtreeNode* tl = expandBoundary(oldNode, subBoundary);
		delete subBoundary;

		// create nodes that divide up the new Boundary with the dividing lines
		// passing through the bottom right corner of the old node
		QuadtreeNode* tr = new QuadtreeNode(b->maxX, b->minY, newbx2, newby2,
				capacity_, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
		QuadtreeNode* bl = new QuadtreeNode(newbx1, newby1, b->maxX, b->minY,
				capacity_, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
		QuadtreeNode* br = new QuadtreeNode(b->maxX, newby1, newbx2, b->minY,
				capacity_, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
		delete b;
		return new QuadtreeNode(newbx1, newby1, newbx2, newby2, capacity_, tl,
				tr, bl, br, MCP_, instanceDirectory_, resolutionBase_,
				numberOfResolutionLevels_);
	}

	delete b;

	return NULL;
}

void Quadtree::insert_points(LidarPoint const* points_array, int points_number)
{
	for (int i = 0; i < points_number; ++i)
	{
		insert(points_array[i]);
	}
}

Quadtree::~Quadtree()
{
	delete root_;
	delete MCP_;

	//Clean up after PointBuckets
	PointBucket::clean_up();

	// Remove temporary files
   removeInstanceDir(instanceDirectory_);
}

bool Quadtree::removeInstanceDir(string target)
{
#ifndef __WIN32
   // try to treat target as directory first, if that fails: check why
   if (rmdir(target.c_str()))
   {
      // failed, check why
      if (errno == ENOTEMPTY)
      {
         // directory contains stuff, enter and delete it
         DIR* openD;
         struct dirent* entry;
         string s;

         openD = opendir(target.c_str());

         if (openD == NULL)
            return true;

         entry = readdir(openD);
         while (entry)
         {
            s = target + FILE_SEPERATOR_STRING + entry->d_name;

            if (entry->d_name[0] != '.')
            {
               if (removeInstanceDir(s.c_str()))
                  return true;
            }

            entry = readdir(openD);
         }

         // then delete directory
         return ( rmdir(target.c_str()) == -1 );
      }

      else if (errno == ENOTDIR)
      {
         // is file, unlink
         return ( unlink(target.c_str()) == -1 );
      }
   }

   // succeeded, return false
   return false;
#else

   return true; // not implemented
#endif
}

// this is for debugging only usefull for tiny trees (<50)
void Quadtree::print()
{
	root_->print();
}

bool Quadtree::isEmpty()
{
	return root_->isEmpty();
}

bool Quadtree::insert(const LidarPoint& newPoint)
{
	// check the Point falls within the global Boundary of the tree
	if (!root_->checkBoundary(newPoint))
	{
		// abort
		return false;
	}

	// this counter simple keeps track of the total points inserted
	// WARNING : debug code, dosen't take account of deletions
	// static int counter;
	// counter++;
	++numberOfPoints_;

	// the guess bucket records the last bucket that a Point
	// was sucsessfully inserted into. because the lidar records read
	// top to bottom; left to right to left neighbouring points generally
	// fall into the same bucket. checking this bucket first saves time.
	// this Pointer keeps track of the location when a full search is
	// needed
	QuadtreeNode *current;

	// if there is no guessbucket this must be the first run
	if (guessBucket_ == NULL)
	{
		current = root_;
	}
	else
	{
		// first try the guess bucket, if this works we can just return
		if (guessBucket_->insert(newPoint))
		{
			return true;
		}
		else
		{
			// otherwise start at the root of the tree
			current = root_;
		}
	}

	// untill a leaf is reached keep picking the child node of the
	// current node that the new Point falls into
	while (!current->isLeaf())
	{
		current = current->pickChild(newPoint);
	}

	// once the correct leaf is reached attempt to insert
	// NOTE: if the Point does not fit the wrong node has
	// been picked and an error is thrown, this is very bad and means
	// there is a bug in the insert method
	if (!current->insert(newPoint))
	{
		throw OutOfBoundsException(
				"could not insert Point into leaf that was chosen, insert broken");
	}

	guessBucket_ = current;
	return true;
}

int heightsort(const void * a, const void * b)
{
	double temp = double(((LidarPoint*) a)->getZ())
			- double(((LidarPoint*) b)->getZ());
	if (temp < 0)
	{
		return -1;
	}
	if (temp > 0)
	{
		return 1;
	}
	return 0;
}

// ditto

int timesort(const void * a, const void * b)
{
	double temp = double(((LidarPoint*) a)->getTime())
			- double(((LidarPoint*) b)->getTime());
	if (temp < 0)
	{
		return -1;
	}
	if (temp > 0)
	{
		return 1;
	}
	return 0;
}

// this function sorts the points within the buckets
// the functionallity of this is contained within the nodes
// which recursivly applie it to all nodes

void Quadtree::sort(char v)
{
	if (v == 'H')
	{
		root_->sort(heightsort);
	}

	if (v == 'T')
	{
		root_->sort(timesort);
	}
}

vector<PointBucket*>* Quadtree::advSubset(vector<double> horizontalCornerValues,
		vector<double> verticalCornerValues, int size)
{

	// work out from the 2 points and the forumula of the line they describe
	// the four Point of
	// the subset rectangle
	vector<PointBucket*> *buckets = new vector<PointBucket*>;

	// begin the recursive subsetting of the root node
	if (root_)
		root_->advSubset(horizontalCornerValues, verticalCornerValues, size,buckets);
	return buckets;
}

vector<PointBucket*>* Quadtree::advSubset(double* horizontalCornerValues,
		double* verticalCornerValues, int size)
{
	vector<PointBucket*> *buckets = new vector<PointBucket*>;

	vector<double> hcv(horizontalCornerValues, horizontalCornerValues + size);
	vector<double> vcv(verticalCornerValues, verticalCornerValues + size);

	// begin the recursive subsetting of the root node
	if (root_)
		root_->advSubset(hcv, vcv, size,
				buckets);
	return buckets;
}

vector<PointBucket*>* Quadtree::getAllBuckets()
{
	Boundary* b = this->getBoundary();
	vector<PointBucket*> *buckets = new vector<PointBucket*>;

	vector<double> Xs(4);
	vector<double> Ys(4);
	Xs[0] = Xs[1] = b->minX - 5;
	Xs[2] = Xs[3] = b->maxX + 5;
	Ys[0] = Ys[3] = b->minY - 5;
	Ys[1] = Ys[2] = b->maxY + 5;

	if (root_)
		root_->advSubset(Xs, Ys, 4, buckets);

	delete b;
	return buckets;
}

Boundary* Quadtree::getBoundary()
{
	if (root_ == NULL)
	{
		throw NullPointerException("attempted to get Boundary from NULL root");
	}
	return root_->getBoundary();
}

void Quadtree::adjustBoundary(Boundary boundary)
{
	// resize the quadtree to accomadate new points
	root_ = expandBoundary(root_, &boundary);
}

string Quadtree::getFileName(uint8_t flightLineNumber)
{
	flighthash::iterator ity = flightTable_.find(flightLineNumber);
	if (ity != flightTable_.end())
	{
		return ity->second.c_str();
	}
	else
	{
		throw OutOfBoundsException("flightline number does not exist");
	}
}

void Quadtree::addFlightline(std::string filename)
{
	flightTable_.insert(make_pair(flightLineNumber_, filename));
	flightLineNumber_++;
}

void Quadtree::increaseDepth(int i)
{
	root_->increaseDepth(i);
}

void Quadtree::increaseToMinimumDepth(int i)
{
	root_->increaseToMinimumDepth(i);
}

int Quadtree::getNumberOfPoints()
{
	return numberOfPoints_;
}

// This function is here purely to provide a C-compatible function for autoconf to detect
extern "C"
{
void liblidarquadtree_is_present(void)
{
	return;
}
;
}
