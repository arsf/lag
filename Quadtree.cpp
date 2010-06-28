
#include "Quadtree.h"
#include "CacheMinder.h"
#include "CollisionDetection.h"

#include <stdlib.h>
#include <sstream>
#include "time.h"

#include "boost/filesystem.hpp"

using namespace std;

void Quadtree::initiliseValues(int capacity, int cacheSize, int depth, int resolutionBase, int numberOfResolutionLevels, ostringstream *errorStream)
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
   instanceDirectory_ = "/tmp/lag_";
   instanceDirectory_.append(boost::lexical_cast<string > (time(NULL)));
   instanceDirectory_.append(boost::lexical_cast<string > (getpid()));
   instanceDirectory_.append(boost::lexical_cast<string > (this));
   boost::filesystem::create_directory(instanceDirectory_);
   MCP_ = new CacheMinder(cacheSize);
   flightLineNumber_ = 0;
   prebuildDepth_ = depth;
}

// this constructor creates a quadtree using the parameters given. it then loads
// into the quadtree the lidarPointloader that was passed

Quadtree::Quadtree(LidarPointLoader *loader, int cap, int nth, int cachesize, int depth, int resolutionBase, int numberOfResolutionLevels, ostringstream *errorStream)
{
   initiliseValues(cap, cachesize, depth, resolutionBase, numberOfResolutionLevels, errorStream);
   // get the Boundary of the file points
   Boundary *b = loader->getBoundary();
   // use Boundary to create new tree that incompasses all points
   root_ = new QuadtreeNode(b->minX, b->minY, b->maxX, b->maxY, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
   root_->increaseDepth(depth);
   load(loader, nth, 0);
}



// this constructor creates a quadtree using a loader object for a given area of interest

Quadtree::Quadtree(LidarPointLoader *loader, int cap, int nth, double *horizontalCornerValues, double *verticalCornerValues, int size, int cacheSize, int depth, int resolutionBase, int numberOfResolutionLevels, ostringstream *errorStream)
{
   initiliseValues(cap, cacheSize, depth, resolutionBase, numberOfResolutionLevels, errorStream);


   // find the simple bounding box of the new fence
   double maxX, maxY, minX, minY;
   maxX = horizontalCornerValues[0];
   minX = horizontalCornerValues[0];
   maxY = verticalCornerValues[0];
   minY = verticalCornerValues[0];
   for (int k = 1; k < size; k++)
   {
      if (horizontalCornerValues[k] > maxX)
      {
         maxX = horizontalCornerValues[k];
      }
      if (horizontalCornerValues[k] < minX)
      {
         minX = horizontalCornerValues[k];
      }
      if (verticalCornerValues[k] > maxY)
      {
         maxY = verticalCornerValues[k];
      }
      if (verticalCornerValues[k] < minY)
      {
         minY = verticalCornerValues[k];
      }
   }
   root_ = new QuadtreeNode(minX, minY, maxX, maxY, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
   root_->increaseDepth(depth);
   // use area of intrest load
   load(loader, nth, 0, horizontalCornerValues, verticalCornerValues, size);

}

Quadtree::Quadtree(Boundary b, int cap, int cacheSize, int depth, int resolutionBase, int numberOfResolutionLevels, ostringstream *errorStream)
{
   Quadtree(b.minX, b.minY, b.maxX, b.maxY, cap, cacheSize, depth, resolutionBase, numberOfResolutionLevels, errorStream);
}


// this constructor creates an empty quadtree to the input specifications
// NOTE: this could still have data loaded into if using load but
// the points may not fail within the boundry

Quadtree::Quadtree(double minX, double minY, double maxX, double maxY, int cap, int cacheSize, int depth, int resolutionBase, int numberOfResolutionLevels, ostringstream *errorStream)
{
   initiliseValues(cap, cacheSize, depth, resolutionBase, numberOfResolutionLevels, errorStream);
   root_ = new QuadtreeNode(minX, minY, maxX, maxY, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
   root_->increaseDepth(depth);
}


// this method expands a quadtree to encompass a new Boundary

QuadtreeNode* Quadtree::expandBoundary(QuadtreeNode* oldNode, Boundary* newpointsBoundary)
{



   Boundary* b = oldNode->getBoundary();

   // if the node already covers the area just return the node
   if (b->maxX == newpointsBoundary->maxX && b->maxY == newpointsBoundary->maxY && b->minX == newpointsBoundary->minX && b->minY == newpointsBoundary->minY)
   {
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

   // find the distance from each of the 4 corners of the nodes Boundary to the new center Point
   // (work out the general location of the node within the new Boundary (top left, top right etc))
   double topleftdistance = sqrt(pow(abs(cx - b->minX), 2) + pow((abs(cy - b->maxY)), 2));
   double toprightdistance = sqrt(pow(abs(cx - b->maxX), 2) + pow((abs(cy - b->maxY)), 2));
   double bottomleftdistance = sqrt(pow(abs(cx - b->minX), 2) + pow((abs(cy - b->minY)), 2));
   double bottomrightdistance = sqrt(pow(abs(cx - b->maxX), 2) + pow((abs(cy - b->minY)), 2));

   // if the old node is in the bottom right
   if (topleftdistance <= toprightdistance && topleftdistance <= bottomleftdistance && topleftdistance <= bottomrightdistance)
   {
      // create nodes that divide up the new Boundary with the dividing lines passing through
      // the top left corner of the old node
      QuadtreeNode* tl = new QuadtreeNode(newbx1, b->maxY, b->minX, newby2, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
      QuadtreeNode* tr = new QuadtreeNode(b->minX, b->maxY, newbx2, newby2, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
      QuadtreeNode* bl = new QuadtreeNode(newbx1, newby1, b->minX, b->maxY, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);

      Boundary* subBoundary = new Boundary;
      subBoundary->minX = b->minX;
      subBoundary->minY = newby1;
      subBoundary->maxX = newbx2;
      subBoundary->maxY = b->maxY;

      // the old node then needs to be expanded into its new quarter (this is to deal with
      // instances where the old node only touches one side of the new Boundary and
      // therefore only fills half its new quarter.
      QuadtreeNode* br = expandBoundary(oldNode, subBoundary);
      delete subBoundary;
      delete b;
      // create a new node above the old containing the 3 new child nodes and the expaned old node
      return new QuadtreeNode(newbx1, newby1, newbx2, newby2, capacity_, tl, tr, bl, br, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
   }

   // if the old node is in the bottom left
   if (toprightdistance <= topleftdistance && toprightdistance <= bottomleftdistance && toprightdistance <= bottomrightdistance)
   {
      // create nodes that divide up the new Boundary with the dividing lines passing through
      // the top right corner of the old node
      QuadtreeNode* tl = new QuadtreeNode(newbx1, b->maxY, b->maxX, newby2, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
      QuadtreeNode* tr = new QuadtreeNode(b->maxX, b->maxY, newbx2, newby2, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);

      Boundary* subBoundary = new Boundary;
      subBoundary->minX = newbx1;
      subBoundary->minY = newby1;
      subBoundary->maxX = b->maxX;
      subBoundary->maxY = b->maxY;

      // the old node then needs to be expanded into its new quarter
      QuadtreeNode* bl = expandBoundary(oldNode, subBoundary);
      delete subBoundary;
      QuadtreeNode* br = new QuadtreeNode(b->maxX, newby1, newbx2, b->maxY, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
      delete b;
      return new QuadtreeNode(newbx1, newby1, newbx2, newby2, capacity_, tl, tr, bl, br, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
   }

   // if the old node is in the top right
   if (bottomleftdistance <= topleftdistance && bottomleftdistance <= toprightdistance && bottomleftdistance <= bottomrightdistance)
   {
      // create nodes that divide up the new Boundary with the dividing lines passing through
      // the bottom left corner of the old node
      QuadtreeNode* tl = new QuadtreeNode(newbx1, b->minY, b->minX, newby2, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);

      Boundary* subBoundary = new Boundary;
      subBoundary->minX = b->minX;
      subBoundary->minY = b->minY;
      subBoundary->maxX = newbx2;
      subBoundary->maxY = newby2;

      // the old node then needs to be expanded into its new quarter
      QuadtreeNode* tr = expandBoundary(oldNode, subBoundary);
      delete subBoundary;
      QuadtreeNode* bl = new QuadtreeNode(newbx1, newby1, b->minX, b->minY, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
      QuadtreeNode* br = new QuadtreeNode(b->minX, newby1, newbx2, b->minY, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
      delete b;
      return new QuadtreeNode(newbx1, newby1, newbx2, newby2, capacity_, tl, tr, bl, br, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
   }

   // if the old node is in the top left
   if (bottomrightdistance <= topleftdistance && bottomrightdistance <= toprightdistance && bottomrightdistance <= bottomleftdistance)
   {
      Boundary* subBoundary = new Boundary;
      subBoundary->minX = newbx1;
      subBoundary->minY = b->minY;
      subBoundary->maxX = b->maxX;
      subBoundary->maxY = newby2;

      // the old node then needs to be expanded into its new quarter
      QuadtreeNode* tl = expandBoundary(oldNode, subBoundary);
      delete subBoundary;

      // create nodes that divide up the new Boundary with the dividing lines passing through
      // the bottom right corner of the old node
      QuadtreeNode* tr = new QuadtreeNode(b->maxX, b->minY, newbx2, newby2, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
      QuadtreeNode* bl = new QuadtreeNode(newbx1, newby1, b->maxX, b->minY, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
      QuadtreeNode* br = new QuadtreeNode(b->maxX, newby1, newbx2, b->minY, capacity_, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
      delete b;
      return new QuadtreeNode(newbx1, newby1, newbx2, newby2, capacity_, tl, tr, bl, br, MCP_, instanceDirectory_, resolutionBase_, numberOfResolutionLevels_);
   }

   return NULL;
}





// load a new flight line into the quad tree, nth is the nth points to load

void Quadtree::load(LidarPointLoader *loader, int nth, int preloadDepth)
{
   // size of each block of points loaded
   int arraysize = 5000000;
   // add the flightline name flightline num pair to the table
   string tempstring(loader->getFileName());
   flightTable_.insert(make_pair(flightLineNumber_, tempstring));

   int outofboundscounter = 0;
   // get new flight Boundary
   Boundary *nb = loader->getBoundary();
   //int hackcounter = 0;


   Point *temp = new Point[arraysize];

   // resize the array to accomadate new points
   root_ = expandBoundary(root_, nb);
   root_->increaseToMinimumDepth(preloadDepth);

   delete nb;
   int Pointcounter;
   ostream &outs = *(errorStream_);
   Boundary *tempBoundary = root_->getBoundary();
   // while there are new points, pull a new block of points from the loader
   // and push them into the tree
   do
   {


      Pointcounter = loader->load(arraysize, nth, temp, flightLineNumber_);
      for (int k = 0; k < Pointcounter; k++)
      {
         // try and insert each Point
         if (!insert(temp[k]))
         {
            // this block of code appends various information and error messages
            // regarding out of bounds points to the specified error stream
            outofboundscounter++;
            outs << outofboundscounter << ": Point out of bounds, diff: ";
            if (temp[k].x < tempBoundary->minX)
            {
               outs << "x:" << abs(temp[k].x - tempBoundary->minX) << " below minimum ";
            }
            else
               if (temp[k].x > tempBoundary->maxX)
            {
               outs << "x:" << abs(temp[k].x - tempBoundary->maxX) << " above maximum ";
            }

            if (temp[k].y < tempBoundary->minY)
            {
               outs << "y:" << abs(temp[k].y - tempBoundary->minY) << " below minimum ";
            }
            else
               if (temp[k].y > tempBoundary->maxY)
            {
               outs << "y:" << abs(temp[k].y - tempBoundary->maxY) << " above maximum ";
            }
            outs << endl;
         }
      }

   } while (Pointcounter == arraysize);
   flightLineNumber_++;
   delete[] temp;
   delete tempBoundary;

}




// this method loads points from a flightline that fall within an area of intrest

void Quadtree::load(LidarPointLoader *loader, int nth, int preloadDepth, double *horizontalCornerValues, double *verticalCornerValues, int size)
{
   // add the flightline name flightline num pair to the table
   string tempstring(loader->getFileName());
   flightTable_.insert(make_pair(flightLineNumber_, tempstring));

   // get new flight Boundary
   Boundary *nb = new Boundary();
   int outofboundscounter = 0;

   Boundary *fb = loader->getBoundary();


   if (!aoRectangleNaoRectangle(fb->minX, fb->minY, fb->maxX, fb->maxY, horizontalCornerValues, verticalCornerValues, size))
   {
      throw OutOfBoundsException("area of interest falls outside new file");
   }

   delete fb;

   // find the simple bounding box of the new fence
   double largestX, largestY, smallestX, smallestY;
   largestX = horizontalCornerValues[0];
   smallestX = horizontalCornerValues[0];
   largestY = verticalCornerValues[0];
   smallestY = verticalCornerValues[0];
   for (int k = 1; k < size; k++)
   {
      if (horizontalCornerValues[k] > largestX)
      {
         largestX = horizontalCornerValues[k];
      }
      if (horizontalCornerValues[k] < smallestX)
      {
         smallestX = horizontalCornerValues[k];
      }
      if (verticalCornerValues[k] > largestY)
      {
         largestY = verticalCornerValues[k];
      }
      if (verticalCornerValues[k] < smallestY)
      {
         smallestY = verticalCornerValues[k];
      }
   }

   nb->minX = smallestX;
   nb->maxX = largestX;
   nb->minY = smallestY;
   nb->maxY = largestY;

   int arraySize = 5000000;
   Point *temp = new Point[arraySize];
   // expand Boundary to cover new points
   root_ = expandBoundary(root_, nb);
   root_->increaseToMinimumDepth(preloadDepth);
   ostream &outs = *(errorStream_);
   delete nb;
   Boundary *tempBoundary = root_->getBoundary();
   // while there are new points, pull a new block of points from the loader
   // and push them into the tree
   int Pointcount;

   do
   {
      Pointcount = loader->load(arraySize, nth, temp, flightLineNumber_, horizontalCornerValues, verticalCornerValues, size);
      for (int k = 0; k < Pointcount; k++)
      {
         // try and insert each Point
         if (!insert(temp[k]))
         {

            // this block of code appends various information and error messages
            // regarding out of bounds points to the specified error stream
            outofboundscounter++;
            outs << outofboundscounter << ": Point out of bounds, diff: ";
            if (temp[k].x < tempBoundary->minX)
            {
               outs << "x:" << abs(temp[k].x - tempBoundary->minX) << " below minimum ";
            }
            else
               if (temp[k].x > tempBoundary->maxX)
            {
               outs << "x:" << abs(temp[k].x - tempBoundary->maxX) << " above maximum ";
            }

            if (temp[k].y < tempBoundary->minY)
            {
               outs << "y:" << abs(temp[k].y - tempBoundary->minY) << " below minimum ";
            }
            else
               if (temp[k].y > tempBoundary->maxY)
            {
               outs << "y:" << abs(temp[k].y - tempBoundary->maxY) << " above maximum ";
            }
            outs << endl;
         }
      }
   } while (Pointcount == arraySize);
   flightLineNumber_++;
   delete[] temp;
   if (outofboundscounter > 0)
   {
      throw OutOfBoundsException("points from file outside header Boundary, " + outofboundscounter);
   }
}


Quadtree::~Quadtree()
{
   delete root_;
   delete MCP_;
   boost::filesystem::remove_all(instanceDirectory_);
}

// this is for debugging only usefull for tiny trees (<50)

void Quadtree::print()
{
   root_->print();
}

// returns true if the quadtree is empty (checks if the root contains
// any additional nodes or any points)

bool Quadtree::isEmpty()
{
   return root_->isEmpty();

}

// this method takes a Point struct, it then attempts to insert it into the
// quadtree.

bool Quadtree::insert(Point newPoint)
{
   // check the Point falls within the global Boundary of the tree
   if (!root_->checkBoundary(newPoint))
   {
      // abort
      return false;
   }

   // this counter simple keeps track of the total points inserted
   // WARNING : debug code, dosen't take account of deletions
   static int counter;
   counter++;

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
      throw OutOfBoundsException("could not insert Point into leaf that was chosen, insert broken");
   }

   guessBucket_ = current;
   return true;
}

bool compare(PointBucket *pb1, PointBucket *pb2)
{
   if (pb1->isIncache() && !pb2->isIncache())
   {
      return true;
   }
   return false;
}








// small function which is provided to the qsort c function to sort points in buckets

int heightsort(const void * a, const void * b)
{
   double temp = double(((Point*) a)->z) - double(((Point*) b)->z);
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
   double temp = double(((Point*) a)->time) - double(((Point*) b)->time);
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

// this method takes 2 points and a width, the points denote a line which is the center line
// of a rectangle whos width is defined by the width, returns NULL passed two identical points

vector<PointBucket*>* Quadtree::advSubset(double *horizontalCornerValues, double *verticalCornerValues, int size)
{


   // work out from the 2 points and the forumula of the line they describe the four Point of
   // the subset rectangle

   vector<PointBucket*> *buckets = new vector<PointBucket*>;

   // begin the recursive subsetting of the root node
   root_->advSubset(horizontalCornerValues, verticalCornerValues, size, buckets);
   std::stable_sort(buckets->begin(), buckets->end(), compare);
   return buckets;
}

// returns the Boundary of the entire quadtree

Boundary* Quadtree::getBoundary()
{
   if (root_ == NULL)
   {
      throw NullPointerException("attempted to get Boundary from NULL root");
   }
   return root_->getBoundary();
}

// this method returns the filename of a given flight line number using the
// hashtable built when loading them

string Quadtree::getFileName(uint8_t flightLineNumber)
{
   flighthash::iterator ity = flightTable_.find(flightLineNumber);
   if (ity != flightTable_.end())
   {
      return ity->second;
   }
   else
   {
      throw OutOfBoundsException("flightline number does not exist");
   }
}

void Quadtree::saveFlightLine(uint8_t flightLineNumber, LidarPointSaver *saver)
{
   Point *points = new Point[1000000];
   int counter = 0;
   vector<PointBucket*> *buckets;
   Boundary *b = root_->getBoundary();

   double *Xs = new double[4];
   double *Ys = new double[4];

   // i should have been shot for this
   Xs[0] = Xs[1] = b->minX;
   Xs[2] = Xs[3] = b->maxX;
   Ys[0] = Ys[3] = b->minY;
   Ys[1] = Ys[2] = b->maxY;

   buckets = advSubset(Xs, Ys, 4);
   PointBucket *current;
   for (unsigned int k = 0; k < buckets->size(); k++)
   {
      current = buckets->at(k);
      for (int i = 0; i < current->getNumberOfPoints(0); i++)
      {
         if (current->getPoint(i, 0).flightLine == flightLineNumber)
         {
            points[counter] = current->getPoint(i, 0);
            counter++;
            if (counter == 1000000)
            {
               saver->savepoints(counter, points);
               counter = 0;
            }
         }
      }
   }
   saver->savepoints(counter, points);

   delete buckets;
   delete b;
   delete[] points;
}

void Quadtree::increaseDepth(int i)
{
   root_->increaseDepth(i);
}

void Quadtree::increaseToMinimumDepth(int i)
{
   root_->increaseToMinimumDepth(i);
}