#include "LASloader.h"

#include <iostream>
#include <stdlib.h>   

using namespace std;

// see quadloader.h
LASloader::LASloader(const char *filename)
{
   this->filename = filename;
   ifs.open (filename, std::ios::in | std::ios::binary);
   reader = new liblas::LASReader(ifs);
   header = reader->GetHeader();
   
}

LASloader::~LASloader()
{
   delete reader;
   ifs.close();
}


int LASloader::load(int n, int nth, point *points, double minX, double minY, double maxX, double maxY)
{

   point temp;
   int counter = 0;
   int pointcounter=0;
   static int numpoints;
   liblas::LASPoint p;
   // for each point
   while (reader->ReadNextPoint() && pointcounter < n)
   {

      // discard if not nth point
      if (counter != nth)
      {
         counter++;
         continue;
      }
      
      
      // for each point make a new struct and initlisise it
      p = reader->GetPoint();
      
      if (p.GetX() < minX || p.GetX() > maxX || p.GetY() < minY || p.GetY() > maxY)
      {
         continue;
      }
      
      
      temp.x = p.GetX() ;
      temp.y = p.GetY() ;
      temp.z = p.GetZ() ;
      temp.classification = p.GetClassification();
      temp.flightline = linecounter;
      temp.intensity = p.GetIntensity();
      temp.rnumber = p.GetReturnNumber();
      temp.time = p.GetTime();
      
      // try and insert point into quad tree, print error 
      
      points[pointcounter] = temp;
      pointcounter++;
      numpoints++;
    
      counter=0;
   
   }
   cout << numpoints << endl;
   return pointcounter;
   
}


int LASloader::load(int n, int nth, point *points)
{
   static int numpoints;
   point temp;
   int counter = 0;
   int pointcounter=0;
   
   liblas::LASPoint p;
   // for each point
   while (pointcounter < n && reader->ReadNextPoint())
   {

      // discard if not nth point
      if (counter != nth)
      {
         counter++;
         continue;
      }
      
      
      // for each point make a new struct and initlisise it
      p = reader->GetPoint();
  
      
      temp.x = p.GetX() ;
      temp.y = p.GetY() ;
      temp.z = p.GetZ() ;
      temp.classification = p.GetClassification();
      temp.flightline = linecounter;
      temp.intensity = p.GetIntensity();
      temp.rnumber = p.GetReturnNumber();
      temp.time = p.GetTime();
      
      // try and insert point into quad tree, print error 
      
      points[pointcounter] = temp;
      pointcounter++;
      
    
      counter=0;
      numpoints++;
   
   }
   cout << numpoints << endl;
   return pointcounter;
}
   
// see quadloader.h
// WARNING: placeholder
bool compatibleCheck(const char *filename){return false;}

// see quadloader.h
boundary* LASloader::getboundary()
{
   
     
   // get bounding box and then correct the stupid inaccuracy between 
   // it and the lidar points (-+ 1/1000)
   // (boundry and header have different accuracys) 

   
   // create a new boundary with the above values and return it
   boundary* newboundary = new boundary;
   newboundary->minX=header.GetMinX()-1;
   newboundary->minY=header.GetMaxX()+1;
   newboundary->maxX=header.GetMinY()-1;
   newboundary->maxY=header.GetMaxY()+1;
   return newboundary;
}









