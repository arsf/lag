/*
 * File:   QuadtreeNode.cpp
 * Authors: Christopher Stanley Finerty (chrfi)
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

#include "QuadtreeNode.h"
#include "QuadtreeExceptions.h"
#include "CollisionDetection.h"

#include <iostream>

using namespace std;
namespace fs = boost::filesystem;


QuadtreeNode::QuadtreeNode(double minX, double minY, double maxX, double maxY,
                     int capacity, CacheMinder *MCP,
                     fs::path instanceDirectory, int resolutionBase,
                     int numberOfResolutionLevels)
:
		a_					(NULL),
		b_					(NULL),
		c_					(NULL),
		d_					(NULL),
		minX_				(minX),
		minY_				(minY),
		maxX_				(maxX),
		maxY_				(maxY),
		capacity_			(capacity),
		numberOfPoints_		(0),
		bucket_				(NULL),
		leaf_				(true),
		MCP_				(MCP),
		instanceDirectory_	(instanceDirectory),
		resolutionBase_		(resolutionBase),
		numberOfResolutionLevels_ (numberOfResolutionLevels)
{}


QuadtreeNode::QuadtreeNode(double minX, double minY, double maxX, double maxY,
                           int capacity, QuadtreeNode* a, QuadtreeNode* b,
                           QuadtreeNode* c, QuadtreeNode* d, CacheMinder *MCP,
                           fs::path instanceDirectory, int resolutionbase,
                           int numberOfResolutionLevels)
:
		a_					(a),
		b_					(b),
		c_					(c),
		d_					(d),
		minX_				(minX),
		minY_				(minY),
		maxX_				(maxX),
		maxY_				(maxY),
		capacity_			(capacity),
		numberOfPoints_		(0),
		bucket_				(NULL),
		leaf_				(false),
		MCP_				(MCP),
		instanceDirectory_	(instanceDirectory),
		resolutionBase_		(resolutionbase),
		numberOfResolutionLevels_ (numberOfResolutionLevels)
{
}


QuadtreeNode::~QuadtreeNode()
{
	if (leaf_)
		delete bucket_;
	else
	{
		delete a_;
		delete b_;
		delete c_;
		delete d_;
	}
}

void QuadtreeNode::print()
{
   if (leaf_ == true)
   {
      if (numberOfPoints_ == 0)
      {
         cout << "(empty)";
      }
      else
      {
         for (int k = 0; k < numberOfPoints_; k++)
         {
            cout << "(" << bucket_->getPoint(k, 0).getX() << " , " << 
                           bucket_->getPoint(k, 0).getY() << ")";
         }
      }
      cout << endl << endl;
   }
   else
   {
      a_->print();
      b_->print();
      c_->print();
      d_->print();
   }
}


bool QuadtreeNode::checkBoundary(LidarPoint newPoint)
{
   if (newPoint.getX() < minX_ || newPoint.getX() > maxX_ || 
       newPoint.getY() < minY_ || newPoint.getY() > maxY_)
   {
      return false;
   }
   return true;
}


Boundary* QuadtreeNode::getBoundary()
{
   Boundary *temp = new Boundary;
   temp->minX = minX_;
   temp->minY = minY_;
   temp->maxX = maxX_;
   temp->maxY = maxY_;
   return temp;
}


QuadtreeNode* QuadtreeNode::pickChild(LidarPoint newPoint)
{
   if (leaf_)
   {
      return NULL;
   }
   if (a_->checkBoundary(newPoint))
   {
      return a_;
   }
   else if (b_->checkBoundary(newPoint))
   {
      return b_;
   }
   else if (c_->checkBoundary(newPoint))
   {
      return c_;
   }
   else if (d_->checkBoundary(newPoint))
   {
      return d_;
   }
   else
   {
	   throw OutOfBoundsException("failed to fit into any of the four child nodes, big problem");
   }
}


void QuadtreeNode::increaseDepth(int i)
{
   // recursively split leaf nodes and decrease i until i == 0;
   if(i <= 0) return;

   if(leaf_)
   {
      splitNode();
      a_->increaseDepth(i-1);
      b_->increaseDepth(i-1);
      c_->increaseDepth(i-1);
      d_->increaseDepth(i-1);
   }
   else
   {
      a_->increaseDepth(i);
      b_->increaseDepth(i);
      c_->increaseDepth(i);
      d_->increaseDepth(i);
   }
}


void QuadtreeNode::increaseToMinimumDepth(int i)
{
   // recursively split leaf nodes until i = 0
   // also decrement i on none leaf nodes
   if(i <= 0) return;
   if(leaf_)
   {
      splitNode();
   }
   a_->increaseToMinimumDepth(i-1);
   b_->increaseToMinimumDepth(i-1);
   c_->increaseToMinimumDepth(i-1);
   d_->increaseToMinimumDepth(i-1);
}


void QuadtreeNode::splitNode()
{
   // if any child nodes are null create them
   if (a_ == NULL)
      a_ = new QuadtreeNode(minX_, minY_ + ((maxY_ - minY_) / 2.0), 
                            minX_ + ((maxX_ - minX_) / 2.0), 
                            maxY_, capacity_, MCP_, instanceDirectory_, 
                            resolutionBase_, numberOfResolutionLevels_);
   if (b_ == NULL)
      b_ = new QuadtreeNode(minX_ + ((maxX_ - minX_) / 2.0), 
                            minY_ + ((maxY_ - minY_) / 2.0), 
                            maxX_, maxY_, capacity_, MCP_, 
                            instanceDirectory_, resolutionBase_, 
                            numberOfResolutionLevels_);
   if (c_ == NULL)
      c_ = new QuadtreeNode(minX_, minY_, minX_ + ((maxX_ - minX_) / 2.0), 
                            minY_ + ((maxY_ - minY_) / 2.0), capacity_, MCP_, 
                            instanceDirectory_, resolutionBase_, 
                            numberOfResolutionLevels_);
   if (d_ == NULL)
      d_ = new QuadtreeNode(minX_ + ((maxX_ - minX_) / 2.0), minY_, 
                            maxX_, minY_ + ((maxY_ - minY_) / 2.0), capacity_, 
                            MCP_, instanceDirectory_, resolutionBase_, 
                            numberOfResolutionLevels_);


   for (int k = 0; k < numberOfPoints_; ++k)
   {
      LidarPoint workingPoint = bucket_->getPoint(k, 0);
      // attept to insert each point in turn into the child nodes
      if (a_->insert(workingPoint))
      {
      }
      else if (b_->insert(workingPoint))
      {
      }
      else if (c_->insert(workingPoint))
      {
      }
      else if (d_->insert(workingPoint))
      {
      }
      else
      {
         throw OutOfBoundsException("Failed to insert old point into any of the four child nodes, big problem.");
      }

   }
   // clean up node and turn into non leaf
   delete bucket_;
   bucket_ = NULL;
   leaf_ = false;
}


bool QuadtreeNode::insert(LidarPoint newPoint)
{
	// if the point dosen't belong in this subset of the tree return false
   if (newPoint.getX() < minX_ || newPoint.getX() > maxX_ || 
       newPoint.getY() < minY_ || newPoint.getY() > maxY_)
   {
      return false;
   }
   else
   {
      // if the node has overflowed and is a leaf
      if ((numberOfPoints_ + 1) > capacity_ && leaf_ == true)
      {
         splitNode();

         // insert the new point that caused the overflow
         if (a_->insert(newPoint))
         {
            return true;
         }
         if (b_->insert(newPoint))
         {
            return true;
         }
         if (c_->insert(newPoint))
         {
            return true;
         }
         if (d_->insert(newPoint))
         {
            return true;
         }

         throw OutOfBoundsException("failed to insert new point into any \
                                     of the four child nodes, big problem");
      }

      // if the node falls within the boundary but this node not a leaf
      if (leaf_ == false)
      {
         return false;
      }
      // if the node falls within the boundary and will not cause an overflow
      else
      {
         // insert new point
         if (bucket_ == NULL)
         {
            bucket_ = new PointBucket(capacity_, minX_, minY_, maxX_, maxY_, 
                                      MCP_, instanceDirectory_, resolutionBase_, 
                                      numberOfResolutionLevels_);
         }
         bucket_->setPoint(newPoint);         
         numberOfPoints_++;
         return true;
      }
   }
}


void QuadtreeNode::sort(int ( * comparator) (const void *, const void *))
{
   if (!leaf_)
   {
      a_->sort(comparator);
      b_->sort(comparator);
      c_->sort(comparator);
      d_->sort(comparator);
   }

   if (bucket_ == NULL)
   {
      return;
   }
   // the qsort function takes a function to compare elements, by passing
   // different functions the attribute by which the points are sorted is 
   // controlled
   for(int k=0; k<numberOfResolutionLevels_; k++)
   {
         qsort(bucket_->points_[k], bucket_->numberOfPoints_[k], 
               sizeof (LidarPoint), comparator);
   }
}


bool QuadtreeNode::isEmpty()
{
   // WARNING: this dosen't take account of the constructor setting
   // childs with no thought for the consequences
   if (leaf_ && bucket_ == NULL)
   {
      return true;
   }
   return false;
}


void QuadtreeNode::advSubset(vector<double> horizontalCornerValues, vector<double> verticalCornerValues,
                             int size, vector<PointBucket*> *buckets)
{
   if (this == NULL)
	   return;

   // convert boundary to polygon
   vector<double> Xs(4);
   vector<double> Ys(4);
   Xs[0] = Xs[1] = minX_;
   Xs[2] = Xs[3] = maxX_;
   Ys[0] = Ys[3] = minY_;
   Ys[1] = Ys[2] = maxY_;

  if(axisSeperationTest(Xs, Ys, 4, horizontalCornerValues, verticalCornerValues, size))
   {
      if(!leaf_)
      {
         a_->advSubset(horizontalCornerValues, verticalCornerValues, size, buckets);
         b_->advSubset(horizontalCornerValues, verticalCornerValues, size, buckets);
         c_->advSubset(horizontalCornerValues, verticalCornerValues, size, buckets);
         d_->advSubset(horizontalCornerValues, verticalCornerValues, size, buckets);
      }
      else
      {
         if(bucket_ != NULL)
         {
            buckets->push_back(bucket_);
         }
      }
   }
}
