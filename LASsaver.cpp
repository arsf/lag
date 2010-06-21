#include <liblas/lasreader.hpp>

#include "LASsaver.h"
#include "quadtreeexceptions.h"


LASsaver::LASsaver(const char *outputfilename, const char *inputfilename)
{
   this->inputfilename = inputfilename;
   this->outputfilename = outputfilename;
   

   ifs.open(inputfilename, std::ios::in | std::ios::binary);
   if(!ifs.is_open())
   {
      throw fileexception("could not open input file");
   }


   reader = new liblas::LASReader(ifs);
   newheader = reader->GetHeader();
   
   delete reader;
   ifs.close();

   ofs.open(outputfilename, std::ios::out | std::ios::binary);
   if(!ofs.is_open())
   {
      throw fileexception("could not open output file");
   }

   numofreturn = 0;
   numofeachreturn = new int[10];
   for(int k=0; k<10; k++)
   {
      numofeachreturn[k] = 0;
   }
   writer = NULL;
}



LASsaver::~LASsaver()
{
   newheader.SetMax(maxX, maxY, maxZ);
   newheader.SetMin(minX, minY, minZ);
   newheader.SetPointRecordsCount(numofreturn);
   newheader.SetPointRecordsByReturnCount(0, numofeachreturn[0]);
   newheader.SetPointRecordsByReturnCount(1, numofeachreturn[1]);
   newheader.SetPointRecordsByReturnCount(2, numofeachreturn[2]);
   newheader.SetPointRecordsByReturnCount(3, numofeachreturn[3]);
   writer->WriteHeader(newheader);
   
   delete writer;
   ofs.close();
   delete[] numofeachreturn;

}


void LASsaver::savepoints(int n, point* points)
{
   bool first = false;
   if(writer == NULL)
   {
      writer = new liblas::LASWriter(ofs, newheader);
      first = true;
   }
   //cout << "saving " << n << " points" << endl;
   liblas::LASPoint point;
   point = liblas::LASPoint();
   for(int k=0; k<n; k++)
   {
      numofreturn++;
      numofeachreturn[(points[k].packedbyte & returnnumber)-1]++;

      if(first)
      {
         maxX = points[k].x;
         maxY = points[k].y;
         maxZ = points[k].z;
         minX = points[k].x;
         minY = points[k].y;
         minZ = points[k].z;
         first = false;
      }
      else
      {
         if(points[k].x > maxX){maxX = points[k].x;}
         if(points[k].y > maxY){maxY = points[k].y;}
         if(points[k].z > maxZ){maxZ = points[k].z;}
         if(points[k].x < minX){minX = points[k].x;}
         if(points[k].y < minY){minY = points[k].y;}
         if(points[k].z < minZ){minZ = points[k].z;}
      }

      point.SetTime(points[k].time);
      point.SetClassification(points[k].classification);
      point.SetCoordinates(points[k].x,points[k].y,points[k].z);
      point.SetFlightLineEdge(points[k].packedbyte & scanedgeflag);
      point.SetIntensity(points[k].intensity);
      point.SetNumberOfReturns(points[k].packedbyte & numberofreturn);
      point.SetPointSourceID(points[k].pointsourceid);
      point.SetReturnNumber(points[k].packedbyte & returnnumber);
      point.SetScanAngleRank(points[k].scanangle);
      point.SetScanDirection(points[k].packedbyte & scandirectionflag);

      writer->WritePoint(point);
   }
}

