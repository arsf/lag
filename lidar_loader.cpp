
#include "lidar_loader.h"
#include "liblas/laspoint.hpp"
#include "liblas/lasreader.hpp"
#include <fstream>
#include <iostream>
#include "quadtree.h"

using namespace std;

// returns the boundary pulled from the LAS file header
boundary* lidar_loader::getboundary(const char *filename)
{
   // setup reader
 
   ifstream ifs;
   ifs.open (filename, std::ios::in | std::ios::binary);
   liblas::LASReader reader(ifs);
   liblas::LASHeader const& header = reader.GetHeader();
     
   // get bounding box and then correct the stupid inaccuracy between 
   // it and the lidar points (-+ 1/1000)
   // (boundry and header have different accuracys) 
   double minX = header.GetMinX()-1;
   double maxX = header.GetMaxX()+1;
   double minY = header.GetMinY()-1;
   double maxY = header.GetMaxY()+1;
   
   // create a new boundary with the above values and return it
   boundary* newboundary = new boundary;
   newboundary->minX=minX;
   newboundary->minY=minY;
   newboundary->maxX=maxX;
   newboundary->maxY=maxY;
   ifs.close();
   return newboundary;
}

// this method loads points from a LAS filename passed into the quadtree passed
bool lidar_loader::load(const char *filename, quadtree *qt, int nth)
{
   
   // setup reader
   ifstream ifs;
   ifs.open (filename, std::ios::in | std::ios::binary);
   liblas::LASReader reader(ifs);

   // WARNING: debug counter
   static int count;
   int counter = 0;
   
   
   
   point temp;
   liblas::LASPoint p;
   // for each point
   while (reader.ReadNextPoint())
   {
      count++;
      linecounter++;
      // discard if not nth point
      if (counter != nth)
      {
         counter++;
         continue;
      }
      
      
      // for each point make a new struct and initlisise it
      p = reader.GetPoint();
      temp.x = p.GetX() ;
      temp.y = p.GetY() ;
      temp.z = p.GetZ() ;
      temp.classification = p.GetClassification();
      temp.flightline = linecounter;
      temp.intensity = p.GetIntensity();
      temp.rnumber = p.GetReturnNumber();
      temp.time = p.GetTime();
      
      // try and insert point into quad tree, print error 
      try
      {
         qt->insert(temp);
      }
         catch (char const* c)
      {
         cout << c << endl;
      }
    
      counter=0;
   
   }
   cout << count << endl;
   ifs.close();
   return true;
}


// this method loads points from a LAS filename passed into the quadtree passed
bool lidar_loader::load(const char *filename, quadtree *qt, int nth, double minX, double minY, double maxX, double maxY)
{
   
   // setup reader
   ifstream ifs;
   ifs.open (filename, std::ios::in | std::ios::binary);
   liblas::LASReader reader(ifs);

   // WARNING: debug counter
   static int count;
   int counter = 0;
   
   
   
   point temp;
   liblas::LASPoint p;
   // for each point
   while (reader.ReadNextPoint())
   {
      count++;
      linecounter++;
      // discard if not nth point
      if (counter != nth)
      {
         counter++;
         continue;
      }
      
           
      // for each point make a new struct and initlisise it
      p = reader.GetPoint();
      
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
      try
      {
         qt->insert(temp);
      }
         catch (char const* c)
      {
         cout << c << endl;
      }
    
      counter=0;
   
   }
   cout << count << endl;
   return true;
}