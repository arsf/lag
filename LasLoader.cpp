#include "LasLoader.h"
#include "quadtreeexceptions.h"
#include "CollisionDetection.h"

using namespace std;

// see quadloader.h

LASloader::LASloader(const char *fileName)
{
   fileName_ = fileName;
   ifs_.open(fileName, std::ios::in | std::ios::binary);
   if (!ifs_.is_open())
   {
      throw FileException("could not open file");
   }
   reader_ = new liblas::LASReader(ifs_);
   header_ = reader_->GetHeader();

}


LASloader::~LASloader()
{
   delete reader_;
   ifs_.close();
}


int LASloader::load(int n, int nth, Point *points, int flightLineNumber)
{
   Point temp;
   int counter = 0;
   int pointCounter = 0;
   liblas::LASPoint p;
   temp.flightLine = flightLineNumber;
   // for each point
   while (pointCounter < n && reader_->ReadNextPoint())
   {
      // discard if not nth point
      if (counter != nth)
      {
         counter++;
         continue;
      }

      // for each point make a new struct and initilise it
      p = reader_->GetPoint();
      temp.x = p.GetX();
      temp.y = p.GetY();
      temp.z = p.GetZ();
      temp.classification = p.GetClassification();
      temp.intensity = p.GetIntensity();
      temp.time = p.GetTime();
      temp.packedByte = p.GetReturnNumber();
      temp.packedByte = temp.packedByte | p.GetNumberOfReturns() << 3;
      temp.packedByte = temp.packedByte | p.GetScanDirection() << 6;
      temp.packedByte = temp.packedByte | p.GetFlightLineEdge() << 7;
      temp.scanAngle = p.GetScanAngleRank();
      temp.pointSourceId = p.GetPointSourceID();

      // copy temp into the array
      points[pointCounter] = temp;
      pointCounter++;


      counter = 0;

   }
   return pointCounter;
}


int LASloader::load(int n, int nth, Point *points, int flightLineNumber, double *horizontalCornerValues, double *verticalCornerValues, int size)
{
   Point temp;
   int counter = 0;
   int pointCounter = 0;
   liblas::LASPoint p;
   temp.flightLine = flightLineNumber;
   // for each point
   while (pointCounter < n && reader_->ReadNextPoint())
   {
      // discard if not nth point
      if (counter != nth)
      {
         counter++;
         continue;
      }

      // for each point make a new struct and initilise it
      p = reader_->GetPoint();

      if (!pointNaoRectangle(p.GetX(), p.GetY(), horizontalCornerValues, verticalCornerValues, size))
      {
         counter = 0;
         continue;
      }

      temp.x = p.GetX();
      temp.y = p.GetY();
      temp.z = p.GetZ();
      temp.classification = p.GetClassification();
      temp.intensity = p.GetIntensity();

      temp.time = p.GetTime();
      temp.packedByte = p.GetReturnNumber();
      temp.packedByte = temp.packedByte | (p.GetNumberOfReturns() << 3);
      temp.packedByte = temp.packedByte | (p.GetScanDirection() << 6);
      temp.packedByte = temp.packedByte | (p.GetFlightLineEdge() << 7);
      temp.scanAngle = p.GetScanAngleRank();
      temp.pointSourceId = p.GetPointSourceID();


      // copy temp into the array
      points[pointCounter] = temp;
      pointCounter++;


      counter = 0;

   }
   return pointCounter;


}


// see quadloader.h
// WARNING: placeholder
bool compatibleCheck(const char *fileName)
{
   return false;
}


Boundary* LASloader::getBoundary()
{


   // get bounding box and then correct the stupid inaccuracy between
   // it and the lidar points (-+ 1/1000)
   // (boundry and header have different accuracys)
   // note: now that out of bounds points are ignored the leyway is removed
   Boundary* newboundary = new Boundary;
   newboundary->minX = header_.GetMinX();
   newboundary->minY = header_.GetMinY();
   newboundary->maxX = header_.GetMaxX();
   newboundary->maxY = header_.GetMaxY();
   return newboundary;
}


const char* LASloader::getFileName()
{
   return fileName_;
}








