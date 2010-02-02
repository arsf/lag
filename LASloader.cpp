#include "LASloader.h"

#include <iostream>
#include <stdlib.h>   

using namespace std;

// see quadloader.h

LASloader::LASloader(const char *filename)
{
    this->filename = filename;
    ifs.open(filename, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
    {
        throw "could not open file";
    }
    reader = new liblas::LASReader(ifs);
    header = reader->GetHeader();

}

LASloader::~LASloader()
{
    delete reader;
    ifs.close();
}

int LASloader::load(int n, int nth, point *points, int flightlinenum, double minX, double minY, double maxX, double maxY)
{

    point temp;
    int counter = 0;
    int pointcounter = 0;
    static int numpoints;
    liblas::LASPoint p;
    temp.flightline = flightlinenum;
    // for each point
    while (reader->ReadNextPoint() && pointcounter < n)
    {

        // discard if not nth point
        if (counter != nth)
        {
            counter++;
            continue;
        }



        p = reader->GetPoint();

        // check if point falls within area of intrest
        if (p.GetX() < minX || p.GetX() > maxX || p.GetY() < minY || p.GetY() > maxY)
        {
            continue;
        }


        temp.x = p.GetX();
        temp.y = p.GetY();
        temp.z = p.GetZ();
        temp.intensity = p.GetIntensity();
        temp.rnumber = p.GetReturnNumber();
        temp.time = p.GetTime();


        // copy temp into the array
        points[pointcounter] = temp;

        // keep track of the number of pointsloaded
        pointcounter++;
        numpoints++;

        counter = 0;

    }
    return pointcounter;

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
        temp.rnumber = p.GetReturnNumber();
        temp.time = p.GetTime();


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








