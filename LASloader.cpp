#include "LASloader.h"
#include "quadtreeexceptions.h"
#include "collisiondetection.h"

using namespace std;

// see quadloader.h

LASloader::LASloader(const char *filename)
{
    this->filename = filename;
    ifs.open(filename, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
    {
        throw fileexception("could not open file");
    }
    reader = new liblas::LASReader(ifs);
    header = reader->GetHeader();

}

LASloader::~LASloader()
{
    delete reader;
    ifs.close();
}



int LASloader::load(int n, int nth, point *points, int flightlinenum)
{
    static int numpoints;
    point temp;
    int counter = 0;
    int pointcounter = 0;
    liblas::LASPoint p;
    temp.flightline = flightlinenum;
    // for each point
    while (pointcounter < n && reader->ReadNextPoint())
    {

        // discard if not nth point
        if (counter != nth)
        {
            counter++;
            continue;
        }


        // for each point make a new struct and initilise it
        p = reader->GetPoint();
        temp.x = p.GetX();
        temp.y = p.GetY();
        temp.z = p.GetZ();
        temp.classification = p.GetClassification();
        temp.intensity = p.GetIntensity();

        temp.time = p.GetTime();
        temp.packedbyte = p.GetReturnNumber();
        temp.packedbyte = temp.packedbyte | p.GetNumberOfReturns() << 3;
        temp.packedbyte = temp.packedbyte | p.GetScanDirection() << 6;
        temp.packedbyte = temp.packedbyte | p.GetFlightLineEdge() << 7;
        temp.scanangle = p.GetScanAngleRank();
        temp.pointsourceid = p.GetPointSourceID();


        // copy temp into the array
        points[pointcounter] = temp;
        pointcounter++;


        counter = 0;
        numpoints++;

    }
    return pointcounter;
}










int LASloader::load(int n, int nth, point *points, int flightlinenum, double *Xs, double *Ys, int size)
{


   static int numpoints;
    point temp;
    int counter = 0;
    int pointcounter = 0;
    liblas::LASPoint p;
    temp.flightline = flightlinenum;
    // for each point
    while (pointcounter < n && reader->ReadNextPoint())
    {

        // discard if not nth point
        if (counter != nth)
        {
            counter++;
            continue;
        }



        // for each point make a new struct and initilise it
        p = reader->GetPoint();

        if(!point_NAOrec(p.GetX(), p.GetY(), Xs, Ys, size))
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
        temp.packedbyte = p.GetReturnNumber();
        temp.packedbyte = temp.packedbyte | (p.GetNumberOfReturns() << 3);
        temp.packedbyte = temp.packedbyte | (p.GetScanDirection() << 6);
        temp.packedbyte = temp.packedbyte | (p.GetFlightLineEdge() << 7);
        temp.scanangle = p.GetScanAngleRank();
        temp.pointsourceid = p.GetPointSourceID();


        // copy temp into the array
        points[pointcounter] = temp;
        pointcounter++;


        counter = 0;
        numpoints++;

    }
    return pointcounter;


}

// see quadloader.h
// WARNING: placeholder

bool compatibleCheck(const char *filename)
{
    return false;
}

// see quadloader.h

boundary* LASloader::getboundary()
{


    // get bounding box and then correct the stupid inaccuracy between
    // it and the lidar points (-+ 1/1000)
    // (boundry and header have different accuracys)
    // note: now that out of bounds points are ignored the leyway is removed
    boundary* newboundary = new boundary;
    newboundary->minX = header.GetMinX();
    newboundary->minY = header.GetMinY();
    newboundary->maxX = header.GetMaxX();
    newboundary->maxY = header.GetMaxY();
    return newboundary;
}

const char* LASloader::getfilename()
{
    return filename;
}








