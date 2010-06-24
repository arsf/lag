#include "quadtreenode.h"
#include "quadtreestructs.h"
#include "quadtreeexceptions.h"
#include "collisiondetection.h"

using namespace std;
int quadtreenode::counter;
int quadtreenode::overflowcounter;
// basic constructor which initilizes the boundary and capacity from paramters and the other meta data to defaults

quadtreenode::quadtreenode(double minX, double minY, double maxX, double maxY, int cap, cacheminder *MCP, string instancedirectory, int resolutionbase, int numresolutionlevels)
{
   this->resolutionbase = resolutionbase;
   this->numresolutionlevels = numresolutionlevels;
   this->minX = minX;
   this->minY = minY;
   this->maxX = maxX;
   this->maxY = maxY;
   capacity = cap;
   bucket = NULL;
   this->MCP = MCP;
   this->instancedirectory = instancedirectory;
   leaf = true;
   a = b = c = d = NULL;
   numofpoints = 0;
   this->subset1skip = subset1skip;
   this->subset2skip = subset2skip;
}



// constructor which allows both the boundarys, capacity and the child nodes of the
// quadtree to be defined

quadtreenode::quadtreenode(double minX, double minY, double maxX, double maxY, int cap, quadtreenode* a, quadtreenode* b, quadtreenode* c, quadtreenode* d, cacheminder *MCP, string instancedirectory, int resolutionbase, int numresolutionlevels)
{
   this->resolutionbase = resolutionbase;
   this->numresolutionlevels = numresolutionlevels;
   this->minX = minX;
   this->minY = minY;
   this->maxX = maxX;
   this->maxY = maxY;
   capacity = cap;
   bucket = NULL;
   this->MCP = MCP;
   this->instancedirectory = instancedirectory;
   // these checks ensure the new quadtree is legal (that all child nodes
   //  boundarys fall within the parent node).
   // NOTE: WARNING: this checking neglects to check if the child nodes
   // corretly divide the parent node into 4 equal squares

   if (a->minX < minX || a->minY < minY || a->maxX > maxX || a->maxY > maxY)
      throw outofboundsexception("node argument a does not fit within paraent node");

   if (b->minX < minX || b->minY < minY || b->maxX > maxX || b->maxY > maxY)
      throw outofboundsexception("node argument b does not fit within paraent node");

   if (c->minX < minX || c->minY < minY || c->maxX > maxX || c->maxY > maxY)
      throw outofboundsexception("node argument c does not fit within paraent node");

   if (d->minX < minX || d->minY < minY || d->maxX > maxX || d->maxY > maxY)
      throw outofboundsexception("node argument d does not fit within paraent node");





   leaf = false;
   this->a = a;
   this->b = b;
   this->c = c;
   this->d = d;
   numofpoints = 0;
}

// deconstructor for quad tree, this recursivly calls the deconstructor in the
// nodes below it

quadtreenode::~quadtreenode()
{
   delete a;
   delete b;
   delete c;
   delete d;
   if (bucket != NULL)
   {
      delete bucket;
   }
}


// this method prints out a crude representation of the quadtree
// NOTE: this method is purely for debugging purposes and will be very difficult
// to interpret when the tree contains more than 50 or so points

void quadtreenode::print()
{
   if (leaf == true)
   {
      if (numofpoints == 0)
      {
         cout << "(empty)";
      }
      else
      {
         for (int k = 0; k < numofpoints; k++)
         {
            cout << "(" << bucket->getpoint(k, 0).x << " , " << bucket->getpoint(k, 0).y << ")";
         }
      }
      cout << endl << endl;
   }
   else
   {
      a->print();
      b->print();
      c->print();
      d->print();
   }
}

// simple bool check

bool quadtreenode::isLeaf()
{
   return leaf;
}

// this simply checks if the passed point is within the node boundarys

bool quadtreenode::checkbound(point newP)
{
   if (newP.x < minX || newP.x > maxX || newP.y < minY || newP.y > maxY)
   {
      return false;
   }
   return true;
}

// return the boundary of this node in a boundary struct

boundary* quadtreenode::getbound()
{
   boundary *temp = new boundary;
   temp->minX = minX;
   temp->minY = minY;
   temp->maxX = maxX;
   temp->maxY = maxY;
   return temp;
}

// this method takes a point and returns the child node that the point falls
// into, if none an exception is thrown

quadtreenode* quadtreenode::pickchild(point newP)
{
   if (leaf)
   {
      return NULL;
   }
   if (a->checkbound(newP))
   {
      return a;
   }
   else if (b->checkbound(newP))
   {
      return b;
   }
   else if (c->checkbound(newP))
   {
      return c;
   }
   else if (d->checkbound(newP))
   {
      return d;
   }
   else
   {
      throw outofboundsexception("failed to fit into any of the four child nodes, big problem");
   }
}


void quadtreenode::increasedepth(int i)
{
   if(i <= 0) return;

   if(leaf)
   {
      splitnode();
      a->increasedepth(i-1);
      b->increasedepth(i-1);
      c->increasedepth(i-1);
      d->increasedepth(i-1);
   }
   else
   {
      a->increasedepth(i);
      b->increasedepth(i);
      c->increasedepth(i);
      d->increasedepth(i);
   }
   
   

}

void quadtreenode::increase_to_minimum_depth(int i)
{
   if(i <= 0) return;
   if(leaf)
   {
      splitnode();
   }
   a->increase_to_minimum_depth(i-1);
   b->increase_to_minimum_depth(i-1);
   c->increase_to_minimum_depth(i-1);
   d->increase_to_minimum_depth(i-1);
}

void quadtreenode::splitnode()
{
   if (a == NULL)
      a = new quadtreenode(minX, minY + ((maxY - minY) / 2.0), minX + ((maxX - minX) / 2.0), maxY, capacity, MCP, instancedirectory, resolutionbase, numresolutionlevels);
   if (b == NULL)
      b = new quadtreenode(minX + ((maxX - minX) / 2.0), minY + ((maxY - minY) / 2.0), maxX, maxY, capacity, MCP, instancedirectory, resolutionbase, numresolutionlevels);
   if (c == NULL)
      c = new quadtreenode(minX, minY, minX + ((maxX - minX) / 2.0), minY + ((maxY - minY) / 2.0), capacity, MCP, instancedirectory, resolutionbase, numresolutionlevels);
   if (d == NULL)
      d = new quadtreenode(minX + ((maxX - minX) / 2.0), minY, maxX, minY + ((maxY - minY) / 2.0), capacity, MCP, instancedirectory, resolutionbase, numresolutionlevels);

   for (int k = 0; k < numofpoints; k++)
   {
      point bob = bucket->getpoint(k, 0);
      // attept to insert each point in turn into the child nodes
      if (a->insert(bob))
      {
         //  cout << bob.x << "/" << bob.y << " old inserted into bucket " << bucket << "(" << a->minX << " " << a->maxX << ")" << endl;
      }
      else if (b->insert(bob))
      {
         // cout << bob.x << "/" << bob.y << " old inserted into bucket " << bucket << "(" << b->minX << " " << b->maxX << ")" << endl;
      }
      else if (c->insert(bob))
      {
         //cout << bob.x << "/" << bob.y << " old inserted into bucket " << bucket << "(" << c->minX << " " << c->maxX << ")" << endl;
      }
      else if (d->insert(bob))
      {
         // cout << bob.x << "/" << bob.y << " old inserted into bucket " << bucket << "(" << d->minX << " " << d->maxX << ")" << endl;
      }
      else
      {
         throw outofboundsexception("failed to insert old point into any of the four child nodes, big problem");
      }

   }
   // clean up node and turn into non leaf
   delete bucket;
   bucket = NULL;
   leaf = false;
}

// this method inserts a point into the node. 
// NOTE: this method does not contain functionality for finding the correct node to insert into
// save for when the correct node overflows and 4 children need to be created and reporting
// wrong nodes.

bool quadtreenode::insert(point newP)
{
   quadtreenode::counter++;
   // if the point dosen't belong in this subset of the tree return false
   if (newP.x < minX || newP.x > maxX || newP.y < minY || newP.y > maxY)
   {
      return false;
   }
   else
   {
      // if the node has overflowed and is a leaf
      if ((numofpoints + 1) > capacity && leaf == true)
      {
         quadtreenode::overflowcounter++;
         // this bucket is full, create four new buckets
         // and populate them.
         // NOTE: because it is possible to create a node with one, any or all of
         // the child nodes already created (see constructors ^) new child nodes
         // are only created where needed.
         // WARNING: because there is nothing to control the boundary of childs
         // created during construction this may lead to overlapping children
         splitnode();


         

         // insert the new point that caused the overflow
         if (a->insert(newP))
         {
            return true;
         }
         if (b->insert(newP))
         {
            return true;
         }
         if (c->insert(newP))
         {
            return true;
         }
         if (d->insert(newP))
         {
            return true;
         }
         throw outofboundsexception("failed to insert new point into any of the four child nodes, big problem");
      }

      // if the node falls within the boundary but this node not a leaf
      if (leaf == false)
      {
         return false;
      }
         // if the node falls within the boundary and will not cause an overflow
      else
      {
         // insert new point
         if (bucket == NULL)
         {
            bucket = new pointbucket(capacity, minX, minY, maxX, maxY, MCP, instancedirectory, resolutionbase, numresolutionlevels);
            //cout << "bucket " << bucket << " created " << endl;
         }
         bucket->setpoint(newP);
         
         numofpoints++;
         return true;
      }
   }
}


// recursivly sort each node

void quadtreenode::sort(int ( * comparator) (const void *, const void *))
{
   if (!leaf)
   {
      a->sort(comparator);
      b->sort(comparator);
      c->sort(comparator);
      d->sort(comparator);
   }

   if (bucket == NULL)
   {
      return;
   }
   // the qsort function takes a function to compare elements, by passing
   // different functions the attribute by which the points are sorted is controlled
   //    qsort(bucket->points, bucket->numberofpoints, sizeof (point), comparator);
}

// if this is a leaf and the bucket is NULL then it is empty,
// sort of, WARNING: this dosen't take account of the damn constructor setting
// childs with no thought for the consiquences

bool quadtreenode::isEmpty()
{
   if (leaf && bucket == NULL)
   {
      return true;
   }
   return false;
}





// this method takes 4 points that describe a rectangle of any orientation and 
// fills the passed vector with buckets that collide with this rectangle

void quadtreenode::advsubset(double *Xs, double *Ys, int size, vector<pointbucket*> *buckets)
{

   // calculate the equations for the lines

   //  NOTE : there is no check for axis orientated lines, this is handled by the quadtree

   // work out the forumla for each of the four lines described by the four points (y=mx+c)

   

  if(AOrec_NAOrec(minX, minY, maxX, maxY, Xs, Ys, size))
   {
      if(!leaf)
      {
         a->advsubset(Xs, Ys, size, buckets);
         b->advsubset(Xs, Ys, size, buckets);
         c->advsubset(Xs, Ys, size, buckets);
         d->advsubset(Xs, Ys, size, buckets);
      }
      else
      {
         if(bucket != NULL)
         {
            buckets->push_back(bucket);
         }
      }
   }

   

}
