



#include "QuadtreeNode.h"
#include "quadtreestructs.h"
#include "quadtreeexceptions.h"
#include "CollisionDetection.h"

using namespace std;
// basic constructor which initilizes the boundary and capacity from paramters and the other meta data to defaults

QuadtreeNode::QuadtreeNode(double minX, double minY, double maxX, double maxY, int capacity, CacheMinder *MCP, string instanceDirectory, int resolutionBase, int numberOfResolutionLevels)
{
   this->resolutionBase_ = resolutionBase;
   this->numberOfResolutionLevels_ = numberOfResolutionLevels;
   this->minX_ = minX;
   this->minY_ = minY;
   this->maxX_ = maxX;
   this->maxY_ = maxY;
   this->capacity_ = capacity;
   bucket_ = NULL;
   this->MCP_ = MCP;
   this->instanceDirectory_ = instanceDirectory;
   leaf_ = true;
   a_ = b_ = c_ = d_ = NULL;
   numberOfPoints_=0;
}


// constructor which allows both the boundarys, capacity and the child nodes of the
// quadtree to be defined
QuadtreeNode::QuadtreeNode(double minX, double minY, double maxX, double maxY, int capacity, QuadtreeNode* a, QuadtreeNode* b, QuadtreeNode* c, QuadtreeNode* d, CacheMinder *MCP, string instanceDirectory, int resolutionbase, int numberOfResolutionLevels)
{
   this->resolutionBase_ = resolutionbase;
   this->numberOfResolutionLevels_ = numberOfResolutionLevels;
   this->minX_ = minX;
   this->minY_ = minY;
   this->maxX_ = maxX;
   this->maxY_ = maxY;
   this->capacity_ = capacity;
   bucket_ = NULL;
   this->MCP_ = MCP;
   this->instanceDirectory_ = instanceDirectory;
   // these checks ensure the new quadtree is legal (that all child nodes
   //  boundarys fall within the parent node).
   // NOTE: WARNING: this checking neglects to check if the child nodes
   // corretly divide the parent node into 4 equal squares

   if (a->minX_ < minX_ || a->minY_ < minY_ || a->maxX_ > maxX_ || a->maxY_ > maxY_)
      throw OutOfBoundsException("node argument a does not fit within paraent node");

   if (b->minX_ < minX_ || b->minY_ < minY_ || b->maxX_ > maxX_ || b->maxY_ > maxY_)
      throw OutOfBoundsException("node argument b does not fit within paraent node");

   if (c->minX_ < minX_ || c->minY_ < minY_ || c->maxX_ > maxX_ || c->maxY_ > maxY_)
      throw OutOfBoundsException("node argument c does not fit within paraent node");

   if (d->minX_ < minX_ || d->minY_ < minY_ || d->maxX_ > maxX_ || d->maxY_ > maxY_)
      throw OutOfBoundsException("node argument d does not fit within paraent node");

   leaf_ = false;
   this->a_ = a;
   this->b_ = b;
   this->c_ = c;
   this->d_ = d;
   numberOfPoints_ = 0;
}


// deconstructor for quad tree, this recursivly calls the deconstructor in the
// nodes below it
QuadtreeNode::~QuadtreeNode()
{
   delete a_;
   delete b_;
   delete c_;
   delete d_;
   if (bucket_ != NULL)
   {
      delete bucket_;
   }
}


// this method prints out a crude representation of the quadtree
// NOTE: this method is purely for debugging purposes and will be very difficult
// to interpret when the tree contains more than 50 or so points so use with caution
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
            cout << "(" << bucket_->getPoint(k, 0).x << " , " << bucket_->getPoint(k, 0).y << ")";
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



bool QuadtreeNode::isLeaf()
{
   return leaf_;
}


// this simply checks if the passed point is within the node boundarys
bool QuadtreeNode::checkBoundary(Point newPoint)
{
   if (newPoint.x < minX_ || newPoint.x > maxX_ || newPoint.y < minY_ || newPoint.y > maxY_)
   {
      return false;
   }
   return true;
}


// return the boundary of this node in a boundary struct
Boundary* QuadtreeNode::getBoundary()
{
   Boundary *temp = new Boundary;
   temp->minX = minX_;
   temp->minY = minY_;
   temp->maxX = maxX_;
   temp->maxY = maxY_;
   return temp;
}


// this method takes a point and returns the child node that the point falls
// into, if none an exception is thrown
QuadtreeNode* QuadtreeNode::pickChild(Point newPoint)
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

   if (a_ == NULL)
      a_ = new QuadtreeNode(minX_, minY_ + ((maxY_ - minY_) / 2.0), minX_ + ((maxX_ - minX_) / 2.0), maxY_, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
   if (b_ == NULL)
      b_ = new QuadtreeNode(minX_ + ((maxX_ - minX_) / 2.0), minY_ + ((maxY_ - minY_) / 2.0), maxX_, maxY_, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
   if (c_ == NULL)
      c_ = new QuadtreeNode(minX_, minY_, minX_ + ((maxX_ - minX_) / 2.0), minY_ + ((maxY_ - minY_) / 2.0), capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
   if (d_ == NULL)
      d_ = new QuadtreeNode(minX_ + ((maxX_ - minX_) / 2.0), minY_, maxX_, minY_ + ((maxY_ - minY_) / 2.0), capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);

   for (int k = 0; k < numberOfPoints_; k++)
   {
      Point workingPoint = bucket_->getPoint(k, 0);
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
         throw OutOfBoundsException("failed to insert old point into any of the four child nodes, big problem");
      }

   }
   // clean up node and turn into non leaf
   delete bucket_;
   bucket_ = NULL;
   leaf_ = false;
}


// this method inserts a point into the node. 
// NOTE: this method does not contain functionality for finding the correct node to insert into
// save for when the correct node overflows and 4 children need to be created and reporting
// wrong nodes.
bool QuadtreeNode::insert(Point newPoint)
{
   // if the point dosen't belong in this subset of the tree return false
   if (newPoint.x < minX_ || newPoint.x > maxX_ || newPoint.y < minY_ || newPoint.y > maxY_)
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
         throw OutOfBoundsException("failed to insert new point into any of the four child nodes, big problem");
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
            bucket_ = new PointBucket(capacity_, minX_, minY_, maxX_, maxY_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
         }
         bucket_->setPoint(newPoint);         
         numberOfPoints_++;
         return true;
      }
   }
}


// recursivly sort each node
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
   // different functions the attribute by which the points are sorted is controlled
   for(int k=0; k<numberOfResolutionLevels_; k++)
   {
         qsort(bucket_->points_[k], bucket_->numberOfPoints_[k], sizeof (Point), comparator);
   }
}


// if this is a leaf and the bucket is NULL then it is empty,
// sort of, WARNING: this dosen't take account of the damn constructor setting
// childs with no thought for the consiquences
bool QuadtreeNode::isEmpty()
{
   if (leaf_ && bucket_ == NULL)
   {
      return true;
   }
   return false;
}





// this method takes 4 points that describe a rectangle of any orientation and 
// fills the passed vector with buckets that collide with this rectangle
void QuadtreeNode::advSubset(double *horizontalCornerValues, double *verticalCornerValues, int size, vector<PointBucket*> *buckets)
{

   //  NOTE : there is no check for axis orientated lines, this is handled by the quadtree

  if(aoRectangleNaoRectangle(minX_, minY_, maxX_, maxY_, horizontalCornerValues, verticalCornerValues, size))
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
