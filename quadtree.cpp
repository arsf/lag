
#include "quadtree.h"


#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <sstream>
#include <ostream>
#include "cacheminder.h"
#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"
#include "time.h"
#include "unistd.h"
#include "collisiondetection.h"

using namespace std;


// this constructor creates a quadtree using the parameters given. it then loads
// into the quadtree the lidarpointloader that was passed

quadtree::quadtree(lidarpointloader *loader, int cap, int nth, int cachesize, ostringstream *errorstream)
{
   if (errorstream == NULL)
   {
      // if no stringstream is given it defaults to the error stream
      this->errorstream = &cerr;
   }
   else
   {
      this->errorstream = errorstream;
   }
   capacity = cap;
   root = NULL;
   guessbucket = NULL;
   instancedirectory = "/tmp/lag_";
   instancedirectory.append(boost::lexical_cast<string > (time(NULL)));
   instancedirectory.append(boost::lexical_cast<string > (getpid()));
   instancedirectory.append(boost::lexical_cast<string > (this));
   boost::filesystem::create_directory(instancedirectory);
   MCP = new cacheminder(cachesize);
   // get the boundary of the file points
   boundary *b = loader->getboundary();
   // use boundary to create new tree that incompasses all points
   root = new quadtreenode(b->minX, b->minY, b->maxX, b->maxY, capacity, MCP, instancedirectory);
   flightlinenum = 0;
   load(loader, nth);
}



// this constructor creates a quadtree using a loader object for a given area of interest

quadtree::quadtree(lidarpointloader *loader, int cap, int nth, double x1, double y1, double x2, double y2, double width, int cachesize, ostringstream *errorstream)
{
   if (errorstream == NULL)
   {
      // if no stringstream is given it defaults to the error stream
      this->errorstream = &cerr;
   }
   else
   {
      this->errorstream = errorstream;
   }
   capacity = cap;
   root = NULL;
   guessbucket = NULL;
   MCP = new cacheminder(cachesize);
   instancedirectory = "/tmp/lag_";
   instancedirectory.append(boost::lexical_cast<string > (time(NULL)));
   instancedirectory.append(boost::lexical_cast<string > (getpid()));
   instancedirectory.append(boost::lexical_cast<string > (this));
   boost::filesystem::create_directory(instancedirectory);

   root = NULL;
   flightlinenum = 0;

   // use area of intrest load
   load(loader, nth, x1, y1, x2, y2, width);

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
   flightlinenum = 0;
   guessbucket = NULL;
   MCP = new cacheminder(cachesize);
   instancedirectory = "/tmp/lag_";
   instancedirectory.append(boost::lexical_cast<string > (time(NULL)));
   instancedirectory.append(boost::lexical_cast<string > (getpid()));
   instancedirectory.append(boost::lexical_cast<string > (this));
   boost::filesystem::create_directory(instancedirectory);
   root = new quadtreenode(minX, minY, maxX, maxY, capacity, MCP, instancedirectory);
}


// this method expands a quadtree to encompass a new boundary
quadtreenode* quadtree::expandboundary(quadtreenode* oldnode, boundary* nb)
{

   if(oldnode == NULL)
   {
      oldnode = new quadtreenode(nb->minX, nb->minY, nb->maxX, nb->maxY, capacity, MCP, instancedirectory);
   }

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
   if (nb->minX < newbx1)
   {
      newbx1 = nb->minX;
   }
   if (nb->minY < newby1)
   {
      newby1 = nb->minY;
   }
   if (nb->maxX > newbx2)
   {
      newbx2 = nb->maxX;
   }
   if (nb->maxY > newby2)
   {
      newby2 = nb->maxY;
   }

   // work out the center point of the new boundary 
   double cx = newbx1 + ((newbx2 - newbx1) / 2);
   double cy = newby1 + ((newby2 - newby1) / 2);

   // find the distance from each of the 4 corners of the nodes boundary to the new center point
   // (work out the general location of the node within the new boundary (top left, top right etc))
   double topleftdistance = sqrt(pow(abs(cx - b->minX), 2) + pow((abs(cy - b->maxY)), 2));
   double toprightdistance = sqrt(pow(abs(cx - b->maxX), 2) + pow((abs(cy - b->maxY)), 2));
   double bottomleftdistance = sqrt(pow(abs(cx - b->minX), 2) + pow((abs(cy - b->minY)), 2));
   double bottomrightdistance = sqrt(pow(abs(cx - b->maxX), 2) + pow((abs(cy - b->minY)), 2));

   // if the old node is in the bottom right
   if (topleftdistance <= toprightdistance && topleftdistance <= bottomleftdistance && topleftdistance <= bottomrightdistance)
   {
      // create nodes that divide up the new boundary with the dividing lines passing through 
      // the top left corner of the old node
      quadtreenode* tl = new quadtreenode(newbx1, b->maxY, b->minX, newby2, capacity, MCP, instancedirectory);
      quadtreenode* tr = new quadtreenode(b->minX, b->maxY, newbx2, newby2, capacity, MCP, instancedirectory);
      quadtreenode* bl = new quadtreenode(newbx1, newby1, b->minX, b->maxY, capacity, MCP, instancedirectory);

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
      return new quadtreenode(newbx1, newby1, newbx2, newby2, capacity, tl, tr, bl, br, MCP, instancedirectory);
   }

   // if the old node is in the bottom left
   if (toprightdistance <= topleftdistance && toprightdistance <= bottomleftdistance && toprightdistance <= bottomrightdistance)
   {
      // create nodes that divide up the new boundary with the dividing lines passing through 
      // the top right corner of the old node
      quadtreenode* tl = new quadtreenode(newbx1, b->maxY, b->maxX, newby2, capacity, MCP, instancedirectory);
      quadtreenode* tr = new quadtreenode(b->maxX, b->maxY, newbx2, newby2, capacity, MCP, instancedirectory);

      boundary* subboundary = new boundary;
      subboundary->minX = newbx1;
      subboundary->minY = newby1;
      subboundary->maxX = b->maxX;
      subboundary->maxY = b->maxY;

      // the old node then needs to be expanded into its new quarter
      quadtreenode* bl = expandboundary(oldnode, subboundary);
      delete subboundary;
      quadtreenode* br = new quadtreenode(b->maxX, newby1, newbx2, b->maxY, capacity, MCP, instancedirectory);
      delete b;
      return new quadtreenode(newbx1, newby1, newbx2, newby2, capacity, tl, tr, bl, br, MCP, instancedirectory);
   }

   // if the old node is in the top right
   if (bottomleftdistance <= topleftdistance && bottomleftdistance <= toprightdistance && bottomleftdistance <= bottomrightdistance)
   {
      // create nodes that divide up the new boundary with the dividing lines passing through 
      // the bottom left corner of the old node
      quadtreenode* tl = new quadtreenode(newbx1, b->minY, b->minX, newby2, capacity, MCP, instancedirectory);

      boundary* subboundary = new boundary;
      subboundary->minX = b->minX;
      subboundary->minY = b->minY;
      subboundary->maxX = newbx2;
      subboundary->maxY = newby2;

      // the old node then needs to be expanded into its new quarter
      quadtreenode* tr = expandboundary(oldnode, subboundary);
      delete subboundary;
      quadtreenode* bl = new quadtreenode(newbx1, newby1, b->minX, b->minY, capacity, MCP, instancedirectory);
      quadtreenode* br = new quadtreenode(b->minX, newby1, newbx2, b->minY, capacity, MCP, instancedirectory);
      delete b;
      return new quadtreenode(newbx1, newby1, newbx2, newby2, capacity, tl, tr, bl, br, MCP, instancedirectory);
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
      quadtreenode* tr = new quadtreenode(b->maxX, b->minY, newbx2, newby2, capacity, MCP, instancedirectory);
      quadtreenode* bl = new quadtreenode(newbx1, newby1, b->maxX, b->minY, capacity, MCP, instancedirectory);
      quadtreenode* br = new quadtreenode(b->maxX, newby1, newbx2, b->minY, capacity, MCP, instancedirectory);
      delete b;
      return new quadtreenode(newbx1, newby1, newbx2, newby2, capacity, tl, tr, bl, br, MCP, instancedirectory);
   }

   return NULL;
}



// load a new flight line into the quad tree, nth is the nth points to load
void quadtree::load(lidarpointloader *loader, int nth)
{
   // add the flightline name flightline num pair to the table
   string tempstring(loader->getfilename());
   flighttable.insert(make_pair(flightlinenum, tempstring));

   int outofboundscounter = 0;
   // get new flight boundary
   boundary *nb = loader->getboundary();
   //int hackcounter = 0;
   // size of each block of points loaded
   int arraysize = 1000000;

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
      pointcounter = loader->load(arraysize, nth, temp, flightlinenum);
      for (int k = 0; k < pointcounter; k++)
      {
         // try and insert each point
         if (!insert(temp[k]))
         {
            // this block of code appends various information and error messages
            // regarding out of bounds points to the specified error stream
            outofboundscounter++;
            outs << outofboundscounter << ": point out of bounds, diff: ";
            if (temp[k].x < tempboundary->minX)
            {
               outs << "x:" << abs(temp[k].x - tempboundary->minX) << " below minimum ";
            }
            else
               if (temp[k].x > tempboundary->maxX)
            {
               outs << "x:" << abs(temp[k].x - tempboundary->maxX) << " above maximum ";
            }

            if (temp[k].y < tempboundary->minY)
            {
               outs << "y:" << abs(temp[k].y - tempboundary->minY) << " below minimum ";
            }
            else
               if (temp[k].y > tempboundary->maxY)
            {
               outs << "y:" << abs(temp[k].y - tempboundary->maxY) << " above maximum ";
            }
            outs << endl;
         }
      }

   }   while (pointcounter == arraysize);
   flightlinenum++;


   delete[] temp;
   delete tempboundary;

}



// this method loads points from a flightline that fall within an area of intrest
void quadtree::load(lidarpointloader *loader, int nth, double x1, double y1, double x2, double y2, double width)
{
   double a1,a2,a3;

   a1=x1-x2;
   a2=y1-y2;
   a3=0;

   double b1,b2,b3;

   b1=0;
   b2=0;
   b3=1;

   double ab1,ab2,ab3;

   ab1 = (a2*b3)-(a3*b2);
   ab2 = (a3*b1)-(a1*b3);
   ab3 = (a1*b2)-(a2*b1);

   double abmagnitude = sqrt(ab1*ab1+ab2*ab2+ab3*ab3);
   double unitab1,unitab2,unitab3;
   unitab1 = ab1/abmagnitude;
   unitab2 = ab2/abmagnitude;
   unitab3 = ab3/abmagnitude;

   double *Xs = new double[4];
   double *Ys = new double[4];

   Xs[0]=x1+(unitab1*width/2);
   Ys[0]=y1+(unitab2*width/2);
   Xs[1]=x1-(unitab1*width/2);
   Ys[1]=y1-(unitab2*width/2);
   Xs[2]=x2-(unitab1*width/2);
   Ys[2]=y2-(unitab2*width/2);
   Xs[3]=x2+(unitab1*width/2);
   Ys[3]=y2+(unitab2*width/2);

   


   // add the flightline name flightline num pair to the table
   string tempstring(loader->getfilename());
   flighttable.insert(make_pair(flightlinenum, tempstring));

   // get new flight boundary
   boundary *nb = new boundary();
   int outofboundscounter = 0;




   if ((AOrec_NAOrec(nb->minX, nb->minY, nb->maxX, nb->maxY, Xs, Ys, 4)))
   {
      throw outofboundsexception("area of interest falls outside new file");
   }

   // find the simple bounding box of the new fence (using 4 as size as its a rectangle
   double largestX,largestY,smallestX,smallestY;
   largestX=Xs[0];smallestX=Xs[0];largestY=Ys[0];smallestY=Ys[0];
   for (int k=1; k<4; k++)
   {
      if(Xs[k] > largestX) {largestX=Xs[k];}
      if(Xs[k] < smallestX) {smallestX=Xs[k];}
      if(Ys[k] > largestY) {largestY=Ys[k];}
      if(Ys[k] < smallestY) {smallestY=Ys[k];}
   }

   nb->minX = smallestX;
   nb->maxX = largestX;
   nb->minY = smallestY;
   nb->maxY = largestY;

   int arraysize = 1000000;
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
      pointcount = loader->load(arraysize, nth, temp, flightlinenum, Xs, Ys, 4);
      for (int k = 0; k < pointcount; k++)
      {
         // try and insert each point
         if (!insert(temp[k]))
         {

            // this block of code appends various information and error messages
            // regarding out of bounds points to the specified error stream
            outofboundscounter++;
            outs << outofboundscounter << ": point out of bounds, diff: ";
            if (temp[k].x < tempboundary->minX)
            {
               outs << "x:" << abs(temp[k].x - tempboundary->minX) << " below minimum ";
            }
            else
               if (temp[k].x > tempboundary->maxX)
            {
               outs << "x:" << abs(temp[k].x - tempboundary->maxX) << " above maximum ";
            }

            if (temp[k].y < tempboundary->minY)
            {
               outs << "y:" << abs(temp[k].y - tempboundary->minY) << " below minimum ";
            }
            else
               if (temp[k].y > tempboundary->maxY)
            {
               outs << "y:" << abs(temp[k].y - tempboundary->maxY) << " above maximum ";
            }
            outs << endl;
         }
      }
   }   while (pointcount == arraysize);
   flightlinenum++;
   delete[] temp;
   delete[] Xs;
   delete[] Ys;
   if (outofboundscounter > 0)
   {
      throw outofboundsexception("points from file outside header boundary, " + outofboundscounter);
   }
}








//deconstructor

quadtree::~quadtree()
{
   MCP->stopcachethread();
   delete root;
   delete MCP;
   boost::filesystem::remove_all(instancedirectory);
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
      throw outofboundsexception("could not insert point into leaf that was chosen, insert broken");
   }

   guessbucket = current;
   return true;
}

bool compare(pointbucket *pb1, pointbucket *pb2)
{
   if (pb1->isincache() && !pb2->isincache())
   {
      return true;
   }
   return false;
}








// small function which is provided to the qsort c function to sort points in buckets

int heightsort(const void * a, const void * b)
{
   double temp = double(((point*) a)->z) - double(((point*) b)->z);
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
   double temp = double(((point*) a)->time) - double(((point*) b)->time);
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
// of a rectangle whos width is defined by the width, returns NULL passed two identical points
vector<pointbucket*>* quadtree::advsubset(double x1, double y1, double x2, double y2, double width)
{
   // check its a line
   if (x1 == x2 && y1 == y2)
   {
      return NULL;
   }

   // work out from the 2 points and the forumula of the line they describe the four point of
   // the subset rectangle
     
   vector<pointbucket*> *buckets = new vector<pointbucket*>;


   double a1,a2,a3;

   a1=x1-x2;
   a2=y1-y2;
   a3=0;

   double b1,b2,b3;

   b1=0;
   b2=0;
   b3=1;

   double ab1,ab2,ab3;

   ab1 = (a2*b3)-(a3*b2);
   ab2 = (a3*b1)-(a1*b3);
   ab3 = (a1*b2)-(a2*b1);

   double abmagnitude = sqrt(ab1*ab1+ab2*ab2+ab3*ab3);
   double unitab1,unitab2,unitab3;
   unitab1 = ab1/abmagnitude;
   unitab2 = ab2/abmagnitude;
   unitab3 = ab3/abmagnitude;

   double *Xs = new double[4];
   double *Ys = new double[4];

   Xs[0]=x1+(unitab1*(width/2));
   Ys[0]=y1+(unitab2*(width/2));
   Xs[1]=x1-(unitab1*(width/2));
   Ys[1]=y1-(unitab2*(width/2));
   Xs[2]=x2-(unitab1*(width/2));
   Ys[2]=y2-(unitab2*(width/2));
   Xs[3]=x2+(unitab1*(width/2));
   Ys[3]=y2+(unitab2*(width/2));

   // begin the recursive subsetting of the root node
   root->advsubset(Xs, Ys, 4, buckets);
   std::stable_sort(buckets->begin(), buckets->end(), compare);
   return buckets;
}

// returns the boundary of the entire quadtree

boundary* quadtree::getboundary()
{
   if (root == NULL)
   {
      throw nullpointerexception("attempted to get boundary from NULL root");
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
      throw outofboundsexception("flightline number does not exist");
   }
}


void quadtree::saveflightline(uint8_t flightlinenum, lidarpointsaver *saver)
{
   point *points = new point[1000000];
   int counter = 0;
   vector<pointbucket*> *buckets;
   boundary *b = root->getbound();
   int width = (b->maxX-b->minX);
   buckets = advsubset(b->minX+(width/2), b->minY, b->minX+(width/2), b->maxY, (width+100));
   pointbucket *current;
   for(unsigned int k=0; k<buckets->size(); k++)
   {
      current = buckets->at(k);
      for(int i=0; i<current->getnumberofpoints(); i++)
      {
         if(current->getpoint(i).flightline == flightlinenum)
         {
            points[counter] = current->getpoint(i);
            counter++;
            if(counter == 1000000)
            {
               saver->savepoints(counter, points);
               counter = 0;
            }

         }

      }
   }
   saver->savepoints(counter, points);
   saver->finalizesave();

   delete buckets;
   delete b;
   delete[] points;
}