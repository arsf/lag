#include "LasSaver.h"
#include "quadtreeexceptions.h"

#include <liblas/lasreader.hpp>


LasSaver::LasSaver(const char *outputFileName, const char *inputFileName)
{
   this->inputFileName_ = inputFileName;
   this->outputFileName_ = outputFileName;

   ifs_.open(inputFileName_, std::ios::in | std::ios::binary);
   if (!ifs_.is_open())
   {
      throw FileException("could not open input file");
   }

   reader_ = new liblas::LASReader(ifs_);
   newHeader_ = reader_->GetHeader();

   delete reader_;
   ifs_.close();

   ofs_.open(outputFileName_, std::ios::out | std::ios::binary);
   if (!ofs_.is_open())
   {
      throw FileException("could not open output file");
   }

   numOfReturn_ = 0;
   numOfEachReturn_ = new int[10];
   for (int k = 0; k < 10; k++)
   {
      numOfEachReturn_[k] = 0;
   }
   writer_ = NULL;
}


LasSaver::~LasSaver()
{
   newHeader_.SetMax(maxX_, maxY_, maxZ_);
   newHeader_.SetMin(minX_, minY_, minZ_);
   newHeader_.SetPointRecordsCount(numOfReturn_);
   newHeader_.SetPointRecordsByReturnCount(0, numOfEachReturn_[0]);
   newHeader_.SetPointRecordsByReturnCount(1, numOfEachReturn_[1]);
   newHeader_.SetPointRecordsByReturnCount(2, numOfEachReturn_[2]);
   newHeader_.SetPointRecordsByReturnCount(3, numOfEachReturn_[3]);
   writer_->WriteHeader(newHeader_);

   delete writer_;
   ofs_.close();
   delete[] numOfEachReturn_;

}


void LasSaver::savepoints(int n, Point* points)
{
   bool first = false;
   if (writer_ == NULL)
   {
      writer_ = new liblas::LASWriter(ofs_, newHeader_);
      first = true;
   }
   liblas::LASPoint point;
   point = liblas::LASPoint();
   for (int k = 0; k < n; k++)
   {
      numOfReturn_++;
      numOfEachReturn_[(points[k].packedByte & returnnumber) - 1]++;

      if (first)
      {
         maxX_ = points[k].x;
         maxY_ = points[k].y;
         maxZ_ = points[k].z;
         minX_ = points[k].x;
         minY_ = points[k].y;
         minZ_ = points[k].z;
         first = false;
      }
      else
      {
         if (points[k].x > maxX_)
         {
            maxX_ = points[k].x;
         }
         if (points[k].y > maxY_)
         {
            maxY_ = points[k].y;
         }
         if (points[k].z > maxZ_)
         {
            maxZ_ = points[k].z;
         }
         if (points[k].x < minX_)
         {
            minX_ = points[k].x;
         }
         if (points[k].y < minY_)
         {
            minY_ = points[k].y;
         }
         if (points[k].z < minZ_)
         {
            minZ_ = points[k].z;
         }
      }

      point.SetTime(points[k].time);
      point.SetClassification(points[k].classification);
      point.SetCoordinates(points[k].x, points[k].y, points[k].z);
      point.SetFlightLineEdge(points[k].packedByte & scanedgeflag);
      point.SetIntensity(points[k].intensity);
      point.SetNumberOfReturns(points[k].packedByte & numberofreturn);
      point.SetPointSourceID(points[k].pointSourceId);
      point.SetReturnNumber(points[k].packedByte & returnnumber);
      point.SetScanAngleRank(points[k].scanAngle);
      point.SetScanDirection(points[k].packedByte & scandirectionflag);

      writer_->WritePoint(point);
   }
}

