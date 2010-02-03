
#include "quadtree.h"

#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <sstream>
#include <ostream>
#include "cacheminder.h"
using namespace std;


// this constructor creates a quadtree using the parameters given. it then loads
// into the quadtree the lidarpointloader that was passed
quadtree::quadtree(lidarpointloader *l,int cap, int nth, int cachesize, ostringstream *s)
{
   if (s == NULL)
   {
       // if no stringstream is given it defaults to the error stream
      errorstream = &cerr;
   }
   else
   {
      errorstream = s;
   }
   capacity = cap;
   root = NULL;
   guessbucket = NULL;
   MCP = new cacheminder(cachesize);
   // get the boundary of the file points
   boundary *b = l->getboundary();
   // use boundary to create new tree that incompasses all points
   root = new quadtreenode(b->minX, b->minY, b->maxX, b->maxY, capacity, MCP);
   flightlinenum = 0;
   load(l, nth);   
}



// this constructor creates a quadtree using a loader object for a given area of interest
quadtree::quadtree(lidarpointloader *l,int cap, int nth, double minX, double minY, double maxX, double maxY, int cachesize, ostringstream *s)
{
   if (s == NULL)
   {
       // if no stringstream is given it defaults to the error stream
      errorstream = &cerr;
   }
   else
   {
      errorstream = s;
   }
   capacity = cap;
   root = NULL;
   guessbucket = NULL;
   MCP = new cacheminder(cachesize);
   // use area of interest to create new tree that incompasses all points
   root = new quadtreenode(minX, minY, maxX, maxY, capacity, MCP);
   flightlinenum = 0;

   // use area of intrest load
   load(l, nth, minX, minY, maxX, maxY);
     
}


// this constructor creates an empty quadtree to the input specifications
// NOTE: this could still have data loaded into if using load but
// the points may not fail within the boundry
quadtree::quadtree(double minX, double minY, double maxX, double maxY, int cap, int cachesize, ostringstream *s)
{
   if (s == NULL)
   {
       // if no stringstream is given it defaults to the error stream
      errorstream = &cerr;
   }
   else
   {
      errorstream = s;
   }
   capacity = cap;
   flightlinenum=0;
   guessbucket = NULL;
   MCP = new cacheminder(cachesize);
   root = new quadtreenode(minX,minY,maxX,maxY,capacity, MCP);
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
      quadtreenode* tl = new quadtreenode(newbx1, b->maxY, b->minX, newby2, capacity, MCP);
      quadtreenode* tr = new quadtreenode(b->minX, b->maxY, newbx2, newby2, capacity, MCP);
      quadtreenode* bl = new quadtreenode(newbx1, newby1, b->minX, b->maxY, capacity, MCP);
      
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
      return new quadtreenode(newbx1, newby1, newbx2, newby2, capacity, tl, tr, bl, br, MCP);
   }
   
   // if the old node is in the bottom left
   if (toprightdistance <= topleftdistance && toprightdistance <= bottomleftdistance && toprightdistance <= bottomrightdistance)
   {
      // create nodes that divide up the new boundary with the dividing lines passing through 
      // the top right corner of the old node
      quadtreenode* tl = new quadtreenode(newbx1, b->maxY, b->maxX, newby2, capacity, MCP);
      quadtreenode* tr = new quadtreenode(b->maxX, b->maxY, newbx2, newby2, capacity, MCP);
      
      boundary* subboundary = new boundary;
      subboundary->minX = newbx1;
      subboundary->minY = newby1;
      subboundary->maxX = b->maxX;
      subboundary->maxY = b->maxY;
      
      // the old node then needs to be expanded into its new quarter
      quadtreenode* bl = expandboundary(oldnode, subboundary);
      delete subboundary;
      quadtreenode* br = new quadtreenode(b->maxX, newby1, newbx2, b->maxY, capacity, MCP);
      delete b;
      return new quadtreenode(newbx1, newby1, newbx2, newby2, capacity, tl, tr, bl, br, MCP);
   }
   
   // if the old node is in the top right
   if (bottomleftdistance <= topleftdistance && bottomleftdistance <= toprightdistance && bottomleftdistance <= bottomrightdistance)
   {
      // create nodes that divide up the new boundary with the dividing lines passing through 
      // the bottom left corner of the old node
      quadtreenode* tl = new quadtreenode(newbx1, b->minY, b->minX, newby2, capacity, MCP);
      
      boundary* subboundary = new boundary;
      subboundary->minX = b->minX;
      subboundary->minY = b->minY;
      subboundary->maxX = newbx2;
      subboundary->maxY = newby2;
      
      // the old node then needs to be expanded into its new quarter
      quadtreenode* tr = expandboundary(oldnode, subboundary);
      delete subboundary;
      quadtreenode* bl = new quadtreenode(newbx1, newby1, b->minX, b->minY, capacity, MCP);
      quadtreenode* br = new quadtreenode(b->minX, newby1, newbx2, b->minY, capacity, MCP);
      delete b;
      return new quadtreenode(newbx1, newby1, newbx2, newby2, capacity, tl, tr, bl, br, MCP);
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
      quadtreenode* tr = new quadtreenode(b->maxX, b->minY, newbx2, newby2, capacity, MCP);
      quadtreenode* bl = new quadtreenode(newbx1, newby1, b->maxX, b->minY, capacity, MCP);
      quadtreenode* br = new quadtreenode(b->maxX, newby1, newbx2, b->minY, capacity, MCP);
      delete b;
      return new quadtreenode(newbx1, newby1, newbx2, newby2, capacity, tl, tr, bl, br, MCP);
   }
   
   throw "error expanding quadtree"; 
}


// load a new flight line into the quad tree, nth is the nth points to load
void quadtree::load(lidarpointloader *l, int nth)
{
   // add the flightline name flightline num pair to the table
   string tempstring(l->getfilename());
   flighttable.insert(make_pair(flightlinenum, tempstring));
   
   int outofboundscounter = 0;
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
   ostream &outs = *(errorstream);
   boundary *tempboundary = root->getbound();
   // while there are new points, pull a new block of points from the loader
   // and push them into the tree
   do
   {
      pointcounter = l->load(arraysize, nth, temp, flightlinenum);
      for(int k=0; k<pointcounter; k++)
      {
         // try and insert each point
         if (!insert(temp[k]))
         {
            // this block of code appends various information and error messages
            // regarding out of bounds points to the specified error stream
            outofboundscounter++;
            outs << outofboundscounter << ": point out of bounds, diff: ";
            if (temp[k].x < tempboundary->minX )
            {
               outs << "x:" << abs(temp[k].x-tempboundary->minX) << " below minimum ";
            } else 
            if (temp[k].x > tempboundary->maxX)
            {
               outs << "x:" << abs(temp[k].x-tempboundary->maxX) << " above maximum ";
            } 
            
            if (temp[k].y < tempboundary->minY)
            {
               outs << "y:" << abs(temp[k].y-tempboundary->minY) << " below minimum ";
            } else
            if (temp[k].y > tempboundary->maxY)
            {
               outs << "y:" << abs(temp[k].y-tempboundary->maxY) << " above maximum ";
            }
            outs << endl;
         }
      }
      //hackcounter+=pointcounter;
   }
   while (pointcounter == arraysize);
   flightlinenum++;

  // cout << "hackcounter says \"" << hackcounter <<" have just been loaded\"" << endl;
   
   delete[] temp;
   delete tempboundary;
   
}


// this method loads points from a flightline that fall within an area of intrest
void quadtree::load(lidarpointloader *l, int nth, double minX, double minY, double maxX, double maxY)
{
   // add the flightline name flightline num pair to the table
   string tempstring(l->getfilename());
   flighttable.insert(make_pair(flightlinenum, tempstring));
   
   // get new flight boundary
   boundary *nb = l->getboundary();
   int outofboundscounter = 0;
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
   ostream &outs = *(errorstream);
   delete nb;
   boundary *tempboundary = root->getbound();
   // while there are new points, pull a new block of points from the loader
   // and push them into the tree
   int pointcount;
   
   do
   {
      pointcount = l->load(arraysize, nth, temp, flightlinenum, minX, minY, maxX, maxY);
      for(int k=0; k<pointcount; k++)
      {
         // try and insert each point
         if (!insert(temp[k]))
         {
            
            // this block of code appends various information and error messages
            // regarding out of bounds points to the specified error stream
            outofboundscounter++;
            outs << outofboundscounter << ": point out of bounds, diff: ";
            if (temp[k].x < tempboundary->minX )
            {
               outs << "x:" << abs(temp[k].x-tempboundary->minX) << " below minimum ";
            } else 
            if (temp[k].x > tempboundary->maxX)
            {
               outs << "x:" << abs(temp[k].x-tempboundary->maxX) << " above maximum ";
            } 
            
            if (temp[k].y < tempboundary->minY)
            {
               outs << "y:" << abs(temp[k].y-tempboundary->minY) << " below minimum ";
            } else
            if (temp[k].y > tempboundary->maxY)
            {
               outs << "y:" << abs(temp[k].y-tempboundary->maxY) << " above maximum ";
            }
            outs << endl;
         }
      }
   }
   while (pointcount == arraysize);
   flightlinenum++;
   delete[] temp;
   if (outofboundscounter > 0)
   {
      throw "points out of bounds exception, "+outofboundscounter;
   }
}





//deconstructor
quadtree::~quadtree()
{
   MCP->stopcachethread();
   delete root;
   delete MCP;
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
bool quadtree::insert(point newP)
{  
   // check the point falls within the global boundary of the tree
   if (!root->checkbound(newP))
   {
      // abort
      return false;
   }
   
   // this counter simple keeps track of the total points inserted
   // WARNING : debug code, dosen't take account of deletions
   static int counter;
   counter++;
   
   // the guess bucket records the last bucket that a point
   // was sucsessfully inserted into. because the lidar records read
   // top to bottom; left to right to left neighbouring points generally
   // fall into the same bucket. checking this bucket first saves time.
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
         return true;
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
   return true;
}


// this method takes a boundary and provides all the buckets that may contain 
// points within that boundary
vector<pointbucket*>* quadtree::subset(double minX, double minY, double maxX, double maxY)
{
   // create the return structure 
   // NOTE: the caller of this method is responsible for cleaning up this data object 
   vector<pointbucket*> *buckets = new vector<pointbucket*>;
   root->subset(minX, minY, maxX, maxY, buckets);
   MCP->cachelist(buckets);
   vector<pointbucket*> *extrabuckets = new vector<pointbucket*>;

   // these additional subsets are to provide a list of buckets surrounding the
   // originol subset so as to allow them be precached incase the next subset
   // is only slightly different
   root->subset(minX-100, minY-100, maxX+100, maxY+100, extrabuckets);
   MCP->pushcachetodo(extrabuckets);
   root->subset(minX-200, minY-200, maxX+200, maxY+200, extrabuckets);
   MCP->pushcachetodo(extrabuckets);
   root->subset(minX-200, minY-300, maxX+300, maxY+300, extrabuckets);
   MCP->pushcachetodo(extrabuckets);
   return buckets;
}

vector<pointbucket*>* quadtree::uncachedsubset(double minX, double minY, double maxX, double maxY)
{
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
   MCP->cachelist(buckets);
  
   return buckets;
 }
   
// returns the boundary of the entire quadtree
boundary* quadtree::getboundary()
{
   if (root == NULL)
   {
      throw "exception: attempted to get boundary from NULL root";
   }
   return root->getbound();
}
   
// this method returns the filename of a given flight line number using the
// hashtable built when loading them
string quadtree::getfilename(uint8_t flightlinenum)
{
   flighthash::iterator ity = flighttable.find(flightlinenum);
   if (ity != flighttable.end())
   {
      return ity->second;
   }
   else
   {
      throw "flightline number does not exist";
   }
}
   
   
