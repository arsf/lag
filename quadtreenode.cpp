#include "quadtreenode.h"
#include <iostream>
#include "quadtreestructs.h"
#include <stdlib.h>
#include <limits>
#include <limits>
using namespace std;

quadtreenode::quadtreenode(double sx, double sy, double bx, double by, int cap)
{
   minX = sx; minY = sy; maxX = bx; maxY = by;
   capacity = cap;
   bucket = NULL;
   
   leaf = true;
   a = b = c = d = NULL;
   currentCap = 0;
}



 // constructor which allows both the boundarys and the child nodes of the
 // quadtree to be defined
 quadtreenode::quadtreenode(double sx, double sy, double bx, double by, int cap, quadtreenode* a, quadtreenode* b, quadtreenode* c, quadtreenode* d)
 {
    minX = sx; minY = sy; maxX = bx; maxY = by;
   capacity = cap;
   bucket = NULL;
     
   // these checks ensure the new quadtree is legal (that all child nodes 
   //  boundarys fall within the parent node).
   // NOTE: WARNING: this checking neglects to check if the child nodes
   // corretly divide the parent node into 4 equal squares
   
      if (a->minX < minX || a->minY < minY || a->maxX > maxX || a->maxY > maxY)
         throw "node argument a does not fit within paraent node";
   
      if (b->minX < minX || b->minY < minY || b->maxX > maxX || b->maxY > maxY)
         throw "node argument b does not fit within paraent node";
   
      if (c->minX < minX || c->minY < minY || c->maxX > maxX || c->maxY > maxY)
         throw "node argument c does not fit within paraent node";
   
      if (d->minX < minX || d->minY < minY || d->maxX > maxX || d->maxY > maxY)
         throw "node argument d does not fit within paraent node";
   
   
   
   
   
   leaf = false;
   this->a = a;
   this->b = b;
   this->c = c;
   this->d = d;
   currentCap = 0;
 }

// deconstructor for quad tree, this recursivly calls the deconstructor in the
// nodes below it
quadtreenode::~quadtreenode()
{
   delete a;
   delete b;
   delete c;
   delete d;
   delete bucket;
}


// this method prints out a crude representation of the quadtree
// NOTE: this method is purely for debugging purposes and will be very difficult
// to interpret when the tree contains more than 50 or so points
void quadtreenode::print()
{
   if (leaf == true)
   {
      if (currentCap == 0)
      {
         cout << "(empty)";
      }
      else
      {
         for (int k=0; k<currentCap; k++)
         {
            cout << "(" << bucket->points[k].x << " , " << bucket->points[k].y << ")";
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
   temp->minX=minX;
   temp->minY=minY;
   temp->maxX=maxX;
   temp->maxY=maxY;
   return temp;
}

// this method takes a point and returns the child node that the point falls
// into, if none an exception is thrown
quadtreenode* quadtreenode::pickchild(point newP)
{
   if(a->checkbound(newP))          
   {return a;}
   else if(b->checkbound(newP))
   {return b;}
   else if(c->checkbound(newP))
   {return c;}
   else if(d->checkbound(newP))
   {return d;}
   else
   {
      throw "failed to fit into any of the four child nodes, big problem";
   }
}



// this method creates the subset of the tree contained within the boundary 
// specified. to do this the method is called recursivly on every node and
// each node checks if it falls within the boundary, if it does
// it adds its bucket to the vecter
// NOTE : this method is designed to only find subsets definied by an axis oriented rectangle
void quadtreenode::subset(double minX, double minY, double maxX, double maxY, vector<pointbucket*> *buckets)
{
   // check if this bucket contains any part of the subset
   if (this->minX >= maxX) return;
   if (this->maxX <= minX) return;
   if (this->minY >= maxY) return;
   if (this->maxY <= minY) return;
   
   
   if (!leaf)
   { 
      // call subset recursivly on child nodes
      a->subset(minX, minY, maxX, maxY, buckets);
      b->subset(minX, minY, maxX, maxY, buckets);
      c->subset(minX, minY, maxX, maxY, buckets);
      d->subset(minX, minY, maxX, maxY, buckets);
   }
   
   
      if (bucket != NULL)
      {
         // add bucket to the vector of buckets
         buckets->push_back(bucket);
      }
      // HACK : this is a HACK
    /*  else
      {
         pointbucket *hackbucket; 
         hackbucket = new pointbucket(capacity, this->minX, this->minY, this->maxX, this->maxY);
         buckets->push_back(hackbucket);
      }*/
   
   
      
}


// this method inserts a point into the node. 
// NOTE: this method does not contain functionality for finding the correct node to insert into
// save for when the correct node overflows and 4 children need to be created and reporting
// wrong nodes.
bool quadtreenode::insert(point newP)
{
   // if the point dosen't belong in this subset of the tree return false
   if (newP.x < minX || newP.x > maxX || newP.y < minY || newP.y > maxY)
   {
      return false;
   }
   else
   {
      // if the node has overflown and is a leaf
      if ((currentCap + 1) > capacity && leaf == true)
      {
         // this bucket is full, create four new buckets
         // and populate them.
         // NOTE: because it is possible to create a node with one, any or all of
         // the child nodes already created (see constructors ^) new child nodes
         // are only created where needed.
         // WARNING: because there is nothing to control the boundary of childs
         // created during construction this may lead to overlapping children
         if (a == NULL)
            a = new quadtreenode(minX, minY+((maxY-minY)/2.0), minX+((maxX-minX)/2.0), maxY, capacity);
         if (b == NULL)
            b = new quadtreenode(minX+((maxX-minX)/2.0), minY+((maxY-minY)/2.0), maxX, maxY, capacity);
         if (c == NULL)
            c = new quadtreenode(minX, minY, minX+((maxX-minX)/2.0), minY+((maxY-minY)/2.0), capacity);
         if (d == NULL)
            d = new quadtreenode(minX+((maxX-minX)/2.0), minY, maxX, minY+((maxY-minY)/2.0), capacity);

         for (int k=0; k<currentCap; k++)
         {
            // attept to insert each point in turn into the child nodes
            if(a->insert(bucket->points[k]))          
            {}else if(b->insert(bucket->points[k]))
            {}else if(c->insert(bucket->points[k]))
            {}else if(d->insert(bucket->points[k]))
            {}else
            {
               throw "failed to insert old point into any of the four child nodes, big problem";
            }
            
         }
         
         
         // clean up node and turn into non leaf
         delete bucket;
         bucket = NULL;
         leaf = false; 
         
         // insert the new point that caused the overflow
         if (a->insert(newP)){ return true;}
         if (b->insert(newP)){ return true;}
         if (c->insert(newP)){ return true;}
         if (d->insert(newP)){ return true;}
         throw "failed to insert new point into any of the four child nodes, big problem";
      }
      
      // if the node falls within the boundary but this node isn't not a leaf
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
            bucket = new pointbucket(capacity, minX, minY, maxX, maxY);
         }
         if (bucket->numberofpoints == 0)
         {
            bucket->maxintensity = newP.intensity;
            bucket->minintensity = newP.intensity;
            bucket->maxz = newP.z;
            bucket->minz = newP.z;
         }
         if (newP.intensity > bucket->maxintensity){bucket->maxintensity=newP.intensity;}
         if (newP.intensity < bucket->minintensity){bucket->minintensity=newP.intensity;}
         if (newP.z > bucket->maxz){bucket->maxz=newP.z;}
         if (newP.z < bucket->minz){bucket->minz=newP.z;}
         
         bucket->points[currentCap] = newP;
         bucket->numberofpoints++;
         currentCap++;
         return true;
      }
   }   
}


// recursivly sort each node
void quadtreenode::sort(int ( * comparator ) ( const void *, const void * ))
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
   qsort(bucket->points, bucket->numberofpoints, sizeof(point), comparator);
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




//  checks for line line intersection between lines (x1,y1 -> x2,y2) , (x3,y3 -> x4,y4)
bool lineintersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
   
   // if you can rule out lines because both points are to the left, right, top or bottom
   // do so
   if (x1 > x3 && x2 > x3 && x1 > x4 && x2 > x4){return false;}
   if (x1 < x3 && x2 < x3 && x1 < x4 && x2 < x4){return false;}
   if (y1 > y3 && y2 > y3 && y1 > y4 && y2 > y4){return false;}
   if (y1 < y3 && y2 < y3 && y1 < y4 && y2 < y4){return false;}
   
   
   
   
 
   
   // find the x,y value of the intersect
   double x = (((x1*y2)-(y1*x2))*(x3-x4) - (x1-x2)*((x3*y4)-(y3*x4)))  /  (((x1-x2)*(y3-y4)) - ((y1-y2)*(x3-x4)));
   double y = (((x1*y2)-(y1*x2))*(y3-y4) - (y1-y2)*((x3*y4)-(y3*x4)))  /  (((x1-x2)*(y3-y4)) - ((y1-y2)*(x3-x4)));
   
   // check if both are segments of the same line
   if (x!=x)
   {
      // check if they overlap
         if (y1 > y3 && y2 > y3 && y1 > y4 && y2 > y4){return false;}
         if (y1 < y3 && y2 < y3 && y1 < y4 && y2 < y4){return false;}
         return true;
   }
   // check if the lines are parallel
   if (std::numeric_limits<double>::infinity() == x)
   {
      return false;
   }
   
   
   
   bool flag = false;
   double err = 0.000001;
   
   //check if the x value falls within the line segments
   // if the first line is vertical you have to check the y value
   if (x1 == x2)
   {
      if ((y1-y2+err) > 0 && (y-y1-err) < 0 && (y-y2+err) > 0)
      {
         flag=true;
      }
      else if ((y1-y2-err) < 0 && (y-y1+err) > 0 && (y-y2-err) < 0)
      {
         flag=true;
      } 
   } // if the first line is not vertical just check if the x intersection falls
     // within both line segments
   else if ( (x1-x2 +err) > 0  && (x-x1-err) < 0 && (x-x2+err) > 0)
   {
      flag=true;
   }
   else if ((x1-x2-err) < 0 && (x-x1+err) > 0 && (x-x2-err) < 0)
   {
      flag=true;
   }
   else
   {
      return false;
   }
   
   
   // if the second line is vertical you have to check the y value
   if (x3 == x4)
   {
      if ((y3-y4+err) > 0 && (y-y3-err) <0 && (y-y4+err) > 0)
      {
         if (flag){return true;}
      }
      else if ((y3-y4-err) < 0 && (y-y3+err) >0 && (y-y4-err) < 0)
      {
         if (flag){return true;}
      } 
   } // if the second line is not vertical just check if the x intersection falls
     // within both line segments
   else if ((x3-x4+err) > 0 && (x-x3-err) <0 && (x-x4+err) > 0)
   {
      if (flag){return true;}
   }
   else if ((x3-x4-err) < 0 && (x-x3+err) >0 && (x-x4-err) < 0)
   {
      if (flag){return true;}
   }
   
   
   return false; 
}

// this method takes the forumula for 4 lines and checks if the x and y arguments are within them
bool OBBpoint(double m1, double c1, double m2, double c2, double m3, double c3, double m4, double c4, double x, double y)
{
   // find the point where an imaginary vertical line through the x value intersects with each line
   double intersec1 = m1 * x + c1;   
   double intersec2 = m2 * x + c2;
   double intersec3 = m3 * x + c3;
   double intersec4 = m4 * x + c4;
   
   // if the xy point is between the intersections with the oposite sides of the rectangle the
   // point is within the rectangle (think about it, it works)
   double err = 0.000001;
   if (((intersec1-y+err) > 0 && (intersec3-y-err) < 0) || ((intersec1-y-err) < 0 && (intersec3-y+err) > 0))
   {
      if(((intersec2-y+err) > 0 && (intersec4-y-err) < 0) || ((intersec2-y-err) < 0 && (intersec4-y+err) > 0))
      {
         return true;
      }
   }
   return false;
}

// method to add the bucket or recure into the child nodes
// NOTE : this has been seperated from the advsubset method to allow a more logical format to that method
void quadtreenode::addsubset(double x1, double y1, double x2, double y2, double x3, double y3 ,double x4, double y4, vector<pointbucket*> *buckets)
{
   if (!leaf)
   { 
      // call subset recursivly on child nodes
      a->advsubset(x1, y1, x2, y2, x3, y3, x4, y4, buckets);
      b->advsubset(x1, y1, x2, y2, x3, y3, x4, y4, buckets);
      c->advsubset(x1, y1, x2, y2, x3, y3, x4, y4, buckets);
      d->advsubset(x1, y1, x2, y2, x3, y3, x4, y4, buckets);
   }
   else
   {
      if (bucket != NULL)
      {
         // add bucket to the vector of buckets
         buckets->push_back(bucket);
      }
      
   }
   
}



// this method takes 4 points that describe a rectangle of any orientation and 
// fills the passed vector with buckets that collide with this rectangle
void quadtreenode::advsubset(double x1, double y1, double x2, double y2, double x3, double y3 ,double x4, double y4, vector<pointbucket*> *buckets) 
{

   // calculate the equations for the lines
   
   //  NOTE : there is no check for axis orientated lines, this is handled by the quadtree
   
      // work out the forumla for each of the four lines described by the four points (y=mx+c)
      double m1,c1,m2,c2,m3,c3,m4,c4;
      if (x1>x2)
      {
         m1 = (y1-y2) / (x1-x2); 
      }
      else 
      {
         m1 =  (y2-y1) / (x2-x1);
      }
      c1 = y1-(m1*x1); 

      
      if (x2>x3)
      {
         m2 =  (y2-y3) / (x2-x3); 
      }
      else 
      {
         m2 =  (y3-y2) / (x3-x2);
      }
      c2 = y2-(m2*x2);
 
      
      if (x3>x4)
      {
         m3 =  (y3-y4) / (x3-x4); 
      }
      else 
      {
         m3 =  (y4-y3) / (x4-x3);
      }
      c3 = y3-(m3*x3);
   
   
      if (x4>x1)
      {
         m4 =  (y4-y1) / (x4-x1); 
      }
      else 
      {
         m4 =  (y1-y4) / (x1-x4) ;
      } 
      c4 = y4-(m4*x4);
      
      // pass the line formulas into the OBBpoint method, do this for each
      // of the four points of the node bounding box
      // if any return true then one of the points of the node bounding box is within
      // the subset area
      if(OBBpoint(m1,c1,m2,c2,m3,c3,m4,c4,minX,minY) ||
         OBBpoint(m1,c1,m2,c2,m3,c3,m4,c4,minX,maxY) ||
         OBBpoint(m1,c1,m2,c2,m3,c3,m4,c4,maxX,maxY) ||
         OBBpoint(m1,c1,m2,c2,m3,c3,m4,c4,maxX,minY)) 
      {
         addsubset(x1,y1,x2,y2,x3,y3,x4,y4,buckets);
         return;
      }
   


   
   
   
   // this checks if any of the points of the subset fall within the nodes boundary
   // showing a collision
   if((x1 < maxX && x1 > minX && y1 < maxY && y1 > minY) ||
   (x2 < maxX && x2 > minX && y2 < maxY && y2 > minY) ||
   (x3 < maxX && x3 > minX && y3 < maxY && y3 > minY) ||
   (x4 < maxX && x4 > minX && y4 < maxY && y4 > minY))
   {
      addsubset(x1,y1,x2,y2,x3,y3,x4,y4,buckets);
      return;
   }
   
   
   // finally the four lines of the subset are compared to each of the four lines of
   // the node bounding box to check if there are any intersections between the 
   // two boxes. this covers situations where none of the subset points fall within
   // the node boundary or visa versa (for instance where the two boxes form a cross)
   if(lineintersect(x1,y1,x2,y2,minX,minY,minX,maxY) ||
   lineintersect(x1,y1,x2,y2,minX,maxY,maxX,maxY) ||
   lineintersect(x1,y1,x2,y2,maxX,maxY,maxX,minY) ||
   lineintersect(x1,y1,x2,y2,maxX,minY,minX,minY) ||
   
   lineintersect(x2,y2,x3,y3,minX,minY,minX,maxY) ||
   lineintersect(x2,y2,x3,y3,minX,maxY,maxX,maxY) ||
   lineintersect(x2,y2,x3,y3,maxX,maxY,maxX,minY) ||
   lineintersect(x2,y2,x3,y3,maxX,minY,minX,minY) ||
   
   lineintersect(x3,y3,x4,y4,minX,minY,minX,maxY) ||
   lineintersect(x3,y3,x4,y4,minX,maxY,maxX,maxY) ||
   lineintersect(x3,y3,x4,y4,maxX,maxY,maxX,minY) ||
   lineintersect(x3,y3,x4,y4,maxX,minY,minX,minY) ||
   
   lineintersect(x4,y4,x1,y1,minX,minY,minX,maxY) ||
   lineintersect(x4,y4,x1,y1,minX,maxY,maxX,maxY) ||
   lineintersect(x4,y4,x1,y1,maxX,maxY,maxX,minY) ||
   lineintersect(x4,y4,x1,y1,maxX,minY,minX,minY))
   {
      addsubset(x1,y1,x2,y2,x3,y3,x4,y4,buckets);
      return;
   }

}
