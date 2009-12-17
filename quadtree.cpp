
#include "quadtree.h"

#include <stdlib.h>
#include <iostream>
#include <cmath>


using namespace std;

// this constructor creates a quadtree using a loader object
quadtree::quadtree(lidarpointloader *l,int cap, int nth)
{
   capacity = cap;
   root = NULL;

   // get the boundary of the file points
   boundary *b = l->getboundary();
   // use boundary to create new tree that incompasses all points
   root = new quadtreenode(b->minX, b->minY, b->maxX, b->maxY, capacity);
   flightlinenum = -1;
   load(l, nth);   
}

// this constructor creates a quadtree using a loader object for a given area of interest
quadtree::quadtree(lidarpointloader *l,int cap, int nth, double minX, double minY, double maxX, double maxY)
{
   capacity = cap;
   root = NULL;

   // use area of interest to create new tree that incompasses all points
   root = new quadtreenode(minX, minY, maxX, maxY, capacity);
   flightlinenum = -1;
   // use area of intrest load
   load(l, nth, minX, minY, maxX, maxY);
     
}


// this constructor creates an empty quadtree to the input specifications
// NOTE: this could still have data loaded into if using load but
// the points may not fail within the boundry
quadtree::quadtree(double sx, double sy, double bx, double by, int cap)
{
   capacity = cap;
   flightlinenum=-1;
   root = new quadtreenode(sx,sy,bx,by,capacity);
}


// this method expands a quadtree to encompass a new boundary
quadtreenode* quadtree::expandboundary(quadtreenode* oldnode, boundary* nb)
{
   
   boundary* b = oldnode->getbound();
   
   // if the node already covers the area just return the node
   if (b->maxX == nb->maxX && b->maxY == nb->maxY && b->minX == nb->minX && b->minY == nb->minY)
   {
      return oldnode;
   }
   
   
   double newbx1 = b->minX;
   double newby1 = b->minY;
   double newbx2 = b->maxX;
   double newby2 = b->maxY;
   // find the boundary that encompasses the old node and the new boundary
   if (nb->minX < newbx1) { newbx1 = nb->minX; }
   if (nb->minY < newby1) { newby1 = nb->minY; }
   if (nb->maxX > newbx2) { newbx2 = nb->maxX; }
   if (nb->maxY > newby2) { newby2 = nb->maxY; }
   
   // work out the center point of the new boundary 
   double cx = newbx1+((newbx2-newbx1)/2);
   double cy = newby1+((newby2-newby1)/2);
   
   // find the distance from each of the 4 corners of the nodes boundary to the new center point
   // (work out the general location of the node within the new boundary (top left, top right etc))
   double topleftdistance = sqrt(pow(abs(cx-b->minX),2) + pow((abs(cy-b->maxY)),2));
   double toprightdistance = sqrt(pow(abs(cx-b->maxX),2) + pow((abs(cy-b->maxY)),2));
   double bottomleftdistance = sqrt(pow(abs(cx-b->minX),2) + pow((abs(cy-b->minY)),2));
   double bottomrightdistance = sqrt(pow(abs(cx-b->maxX),2) + pow((abs(cy-b->minY)),2));
   
   // if the old node is in the bottom right
   if (topleftdistance <= toprightdistance && topleftdistance <= bottomleftdistance && topleftdistance <= bottomrightdistance)
   {
      // create nodes that divide up the new boundary with the dividing lines passing through 
      // the top left corner of the old node
      quadtreenode* tl = new quadtreenode(newbx1, b->maxY, b->minX, newby2, capacity);
      quadtreenode* tr = new quadtreenode(b->minX, b->maxY, newbx2, newby2, capacity);
      quadtreenode* bl = new quadtreenode(newbx1, newby1, b->minX, b->maxY, capacity);
      
      boundary* subboundary = new boundary;
      subboundary->minX = b->minX;
      subboundary->minY = newby1;
      subboundary->maxX = newbx2;
      subboundary->maxY = b->maxY;
      
      // the old node then needs to be expanded into its new quarter (this is to deal with
      // instances where the old node only touches one side of the new boundary and 
      // therefore only fills half its new quarter.
      quadtreenode* br = expandboundary(oldnode, subboundary);
      delete subboundary;
      delete b;
      // create a new node above the old containing the 3 new child nodes and the expaned old node
      return new quadtreenode(newbx1, newby1, newbx2, newby2, capacity, tl, tr, bl, br);
   }
   
   // if the old node is in the bottom left
   if (toprightdistance <= topleftdistance && toprightdistance <= bottomleftdistance && toprightdistance <= bottomrightdistance)
   {
      // create nodes that divide up the new boundary with the dividing lines passing through 
      // the top right corner of the old node
      quadtreenode* tl = new quadtreenode(newbx1, b->maxY, b->maxX, newby2, capacity);
      quadtreenode* tr = new quadtreenode(b->maxX, b->maxY, newbx2, newby2, capacity);
      
      boundary* subboundary = new boundary;
      subboundary->minX = newbx1;
      subboundary->minY = newby1;
      subboundary->maxX = b->maxX;
      subboundary->maxY = b->maxY;
      
      // the old node then needs to be expanded into its new quarter
      quadtreenode* bl = expandboundary(oldnode, subboundary);
      delete subboundary;
      quadtreenode* br = new quadtreenode(b->maxX, b->minY, newbx2, newby2, capacity);
      delete b;
      return new quadtreenode(newbx1, newby1, newbx2, newby2, capacity, tl, tr, bl, br);
   }
   
   // if the old node is in the top right
   if (bottomleftdistance <= topleftdistance && bottomleftdistance <= toprightdistance && bottomleftdistance <= bottomrightdistance)
   {
      // create nodes that divide up the new boundary with the dividing lines passing through 
      // the bottom left corner of the old node
      quadtreenode* tl = new quadtreenode(newbx1, b->minY, b->minX, newby2, capacity);
      
      boundary* subboundary = new boundary;
      subboundary->minX = b->minX;
      subboundary->minY = b->minY;
      subboundary->maxX = newbx2;
      subboundary->maxY = newby2;
      
      // the old node then needs to be expanded into its new quarter
      quadtreenode* tr = expandboundary(oldnode, subboundary);
      delete subboundary;
      quadtreenode* bl = new quadtreenode(newbx1, newby1, b->minX, b->minY, capacity);
      quadtreenode* br = new quadtreenode(b->minX, newby1, newbx2, b->minY, capacity);
      delete b;
      return new quadtreenode(newbx1, newby1, newbx2, newby2, capacity, tl, tr, bl, br);
   }
   
   // if the old node is in the top left
   if (bottomrightdistance <= topleftdistance && bottomrightdistance <= toprightdistance && bottomrightdistance <= bottomleftdistance)
   {
      boundary* subboundary = new boundary;
      subboundary->minX = newbx1;
      subboundary->minY = b->minY;
      subboundary->maxX = b->maxX;
      subboundary->maxY = newby2;
      
      // the old node then needs to be expanded into its new quarter
      quadtreenode* tl = expandboundary(oldnode, subboundary);
      delete subboundary;
      
      // create nodes that divide up the new boundary with the dividing lines passing through 
      // the bottom right corner of the old node
      quadtreenode* tr = new quadtreenode(b->maxX, b->minY, newbx2, newby2, capacity);
      quadtreenode* bl = new quadtreenode(newbx1, newby1, b->maxX, b->minY, capacity);
      quadtreenode* br = new quadtreenode(b->maxX, newby1, newbx2, b->minY, capacity);
      delete b;
      return new quadtreenode(newbx1, newby1, newbx2, newby2, capacity, tl, tr, bl, br);
   }
   
   throw "error expanding quadtree"; 
}


// load a new flight line into the quad tree, nth is the nth points to load
void quadtree::load(lidarpointloader *l, int nth)
{
   // get new flight boundary
   boundary *nb = l->getboundary();
   //int hackcounter = 0;
   // size of each block of points loaded
   int arraysize = 10000;
   
   point *temp = new point[arraysize];
   
   // resize the array to accomadate new points 
   root = expandboundary(root, nb);

   delete nb;
   int pointcounter;
   flightlinenum++;
   // while there are new points, pull a new block of points from the loader
   // and push them into the tree
   do
   {
      pointcounter = l->load(arraysize, nth, temp, flightlinenum);
      for(int k=0; k<pointcounter; k++)
      {
         insert(temp[k]);
      }
      //hackcounter+=pointcounter;
   }
   while (pointcounter == arraysize);
   

  // cout << "hackcounter says \"" << hackcounter <<" have just been loaded\"" << endl;
   
   delete[] temp;
}


// this method loads points from a flightline that fall within an area of intrest
void quadtree::load(lidarpointloader *l, int nth, double minX, double minY, double maxX, double maxY)
{
   // get new flight boundary
   boundary *nb = l->getboundary();

   if ((minX > nb->maxX && maxX > nb->maxX) || (minX < nb->minX && maxX < nb->minX))
   {
      throw "area of interest falls outside new file";
   }
   
   if ((minY > nb->maxY && maxY > nb->maxY) || (minY < nb->minY && maxY < nb->minY))
   {
      throw "area of interest falls outside new file";
   }
   
   nb->minX = minX;
   nb->maxX = maxX;
   nb->minY = minY;
   nb->maxY = maxY;
   
   int arraysize = 10000;
   point *temp = new point[arraysize];
   // expand boundary to cover new points
   root = expandboundary(root, nb);
 
   delete nb;
   
   // while there are new points, pull a new block of points from the loader
   // and push them into the tree
   int pointcount;
   flightlinenum++;
   do
   {
      pointcount = l->load(arraysize, nth, temp, flightlinenum, minX, minY, maxX, maxY);
      for(int k=0; k<pointcount; k++)
      {
         insert(temp[k]);
      }
   }
   while (pointcount == arraysize);
   
   
}





//deconstructor
quadtree::~quadtree()
{
   delete root;
}

// this is for debugging only usefull for tiny trees (<50)
void quadtree::print()
{
   root->print();
}

// returns true if the quadtree is empty (checks if the root contains
// any additional nodes or any points)
bool quadtree::isEmpty()
{
   return root->isEmpty();
   
}

// this method takes a point struct, it then attempts to insert it into the
// quadtree. 
void quadtree::insert(point newP)
{  
   // check the point falls within the global boundary oof the tree
   if (!root->checkbound(newP))
   {
      throw "point out of bounds error";
   }
   
   // this counter simple keeps track of the total points inserted
   // WARNING : debug code, dosen't take account of deleteions
   static int counter;
   counter++;
   
   // the guess bucket records the last bucket that a point
   // was sucsessfully inserted into. because the lidar records read
   // top to bottom; left to right to left neighbouring points generally
   // fall into the same bucket. checking this bucket first saves time.
   static quadtreenode* guessbucket;
   // this pointer keeps track of the location when a full search is
   // needed
   quadtreenode *current;
   
   // if there is no guessbucket this must be the first run
   if (guessbucket == NULL)
   {   
      current = root;
   }
   else
   {
      // first try the guess bucket, if this works we can just return
      if (guessbucket->insert(newP))
      {
         return;
      }
      else
      {
         // otherwise start at the root of the tree
         current = root;
      }
   }
   
   // untill a leaf is reached keep picking the child node of the 
   // current node that the new point falls into
   while (!current->isLeaf())
   {
      current = current->pickchild(newP);
   }
   
   // once the correct leaf is reached attempt to insert
   // NOTE: if the point does not fit the wrong node has 
   // been picked and an error is thrown, this is very bad and means 
   // there is a bug in the insert method
   if (!current->insert(newP))
   {
      throw "point out of bounds error, insert broken";
   }
   
   guessbucket = current;
}


// this method takes a boundary and provides all the buckets that may contain 
// points within that boundary
vector<pointbucket*>* quadtree::subset(double minX, double minY, double maxX, double maxY)
{
   // create the return structure 
   // NOTE: the caller of this method is responsible for cleaning up this data object 
   vector<pointbucket*> *buckets = new vector<pointbucket*>;
   root->subset(minX, minY, maxX, maxY, buckets);
   return buckets;
}




// small function which is provided to the qsort c function to sort points in buckets
int heightsort(const void * a, const void * b)
{
   double temp = double(((point*)a)->z) - double(((point*)b)->z);
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
   double temp = double(((point*)a)->time) - double(((point*)b)->time);
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
void quadtree::sort(char v)
{
   if (v == 'H')
   {
      root->sort(heightsort);
   }
}

// this method takes 2 points and a width, the points denote a line which is the center line
// of a rectangle whos width is defined by the width
vector<pointbucket*>* quadtree::advsubset(double x1, double y1, double x2, double y2, double width)
{
   // check its a line
   if(x1==x2 && y1 == y2)
   {
      throw "muppet, thats a point not a line";
   }
   
   // work out from the 2 points and the forumula of the line they describe the four point of
   // the subset rectangle
   vector<pointbucket*> *buckets = new vector<pointbucket*>;
   
   double m = NULL;
   if(x1 == x2 || y1 == y2)
   {
      double sx = x1;
      double sy = y1;
      double bx = x1;
      double by = y1;
      // as it is not known which of the subset points is which the smallest and largest 
      // must be found and the bottom left and top right cords found
      if (x1 == x2 && y1 > y2)
      {
         sx = x1 - (width / 2);
         sy = y2;
         bx = x2 + (width / 2);
         by = y1; 
      }
      if (x1 == x2 && y1 < y2)
      {
         sx = x1 - (width / 2);
         sy = y1;
         bx = x2 + (width / 2);
         by = y2; 
      }
      
      // as it is not known which of the subset points is which the smallest and largest 
      // must be found and the bottom left and top right cords found   
      if (y1 == y2 && x1 > x2)
      {
         sx = x2;
         sy = y2 - (width / 2);
         bx = x1;
         by = y2 + (width / 2);
      }
      if (y1 == y2 && x1 < x2)
      {
         sx = x1;
         sy = y2 - (width / 2);
         bx = x2;
         by = y2 + (width / 2);
      }
      
      // call the subset method. the recursion will continue through this now
      // NOTE : this will only ever happen in the root node as it will
      // be picked up immediatly, this means that a call to advsubset with a 
      // axis orientated box is almost identical to a call to subset.
      root->subset(sx,sy,bx,by,buckets);
      return buckets;
   }
   else if ( x1 > x2)
   {
      m =  (y1 - y2) / (x1 - x2);
   }
   else if ( x1 < x2)
   {
      m =  (y2 - y1) / (x2 - x1);
   }
   
   // black magic maths to find the four corners of the rectangle
   double pm = -1/m;
   double theta = atan(pm);
   double L = width/2;
   double deltax = L * cos(theta);
   double deltay = L * sin(theta);
   double sx1 = x1 + deltax;
   double sy1 = y1 + deltay;
   double sx2 = x1 - deltax;
   double sy2 = y1 - deltay;
   double sx4 = x2 + deltax;
   double sy4 = y2 + deltay;
   double sx3 = x2 - deltax;
   double sy3 = y2 - deltay;
   
   // begin the recursive subsetting of the root node 
   root->advsubset(sx1,sy1,sx2,sy2,sx3,sy3,sx4,sy4,buckets);
   return buckets;
 }
   
   
boundary* quadtree::getboundary()
{
   return root->getbound();
}
   
   
   
   
