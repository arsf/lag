// see lidarpointloader.h

#include "AsciiLoader.h"
#include "quadtreeexceptions.h"
#include "CollisionDetection.h"

#include <string>
#include <cstring>

#include <boost/lexical_cast.hpp>

AsciiLoader::AsciiLoader(const char *fileName, const char *formatString)
{
   file_ = fopen(fileName_, "r");
   if (file_ == NULL)
   {
      throw FileException("error opening file");
   }
   fclose(file_);

   ifs_.open(fileName);
   formatString_ = formatString;
   fileName_ = fileName;
   boundary_ = NULL;
}


AsciiLoader::~AsciiLoader() {
 }


int AsciiLoader::load(int n, int nth, Point *points, int flightLineNumber, double *horizontalCornerValues, double *verticalCornerValues, int size)
{
   // set delim to a string to generate the end of string character then set the first character
   char delim[2] = "x";
   delim[0] = formatString_[0];

   Point temp;
   int wordCounter;
   int counter = 0;
   int pointCounter = 0;
   char *token;
   char line[300];
   temp.flightLine = flightLineNumber;

   // get each line till enough points are loaded or the file ends
   while (pointCounter != n && fgets(line, 300, file_) != NULL)
   {
      // discard if not nth point
      if (counter != nth)
      {
         counter++;
         continue;
      }

      temp.classification = 0;
      temp.intensity = 0;
      temp.x = 0;
      temp.y = 0;
      temp.z = 0;
      temp.time = 0;
      temp.packedByte = 0;
      temp.pointSourceId = 0;
      temp.scanAngle = 0;

      token = strtok(line, delim);
      wordCounter = 0;

      //loop untill the line has no more tokens
      while (token != NULL)
      {
         wordCounter++;
         // use the corrisponding character in the format string to determine how
         // to handle the corrisponding token
         switch (formatString_[wordCounter]) {
            case 'x':
            {
               temp.x = atof(token);
               break;
            }
            case 'y':
            {
               temp.y = atof(token);
               break;
            }
            case 'z':
            {
               temp.z = atof(token);
               break;
            }
            case 'i':
            {
               temp.intensity = atoi(token);
               break;
            }
            case 'c':
            {
               temp.classification = atoi(token);
               break;
            }
            case 'r':
            {
               temp.packedByte = temp.packedByte | atoi(token);
               break;
            }
            case 't':
            {
               temp.time = atof(token);
               break;
            }
            case 's':
            {
               temp.scanAngle = atoi(token);
               break;
            }
            case 'd':
            {
               temp.packedByte = temp.packedByte | (atoi(token) << 6);
               break;
            }
            case 'n':
            {
               temp.packedByte = temp.packedByte | (atoi(token) << 3);
               break;
            }
            case 'e':
            {
               temp.packedByte = temp.packedByte | (atoi(token) << 7);
               break;
            }
            case 'p':
            {
               temp.pointSourceId = temp.pointSourceId;
               break;
            }
            default:
            {
            }
         }
         token = strtok(NULL, delim);
      }

      if (!pointNaoRectangle(temp.x, temp.y, horizontalCornerValues, verticalCornerValues, size))
      {
         counter = 0;
         continue;
      }

      // copy the valid points into the array
      points[pointCounter] = temp;
      pointCounter++;
      counter = 0;
   }
   return pointCounter;
}


int AsciiLoader::load(int n, int nth, Point *points, int flightLineNumber)
{
   // set delim to a string to generate the end of string character then set the first character
   char delim[2] = "x";
   delim[0] = formatString_[0];
   Point temp;
   int wordCounter;
   int counter = 0;
   int pointCounter = 0;
   char *token;
   char line[300];
   temp.flightLine = flightLineNumber;

   // get each line till enough points are loaded or the file ends
   while (pointCounter != n && fgets(line, 300, file_) != NULL)
   {
      // discard if not nth point
      if (counter != nth)
      {
         counter++;
         continue;
      }

      temp.classification = 0;
      temp.intensity = 0;
      temp.x = 0;
      temp.y = 0;
      temp.z = 0;
      temp.time = 0;
      temp.packedByte = 0;
      temp.pointSourceId = 0;
      temp.scanAngle = 0;

      token = strtok(line, delim);
      wordCounter = 0;

      //loop untill the line has no more tokens
      while (token != NULL)
      {
         wordCounter++;
         // use the corrisponding character in the format string to determine how
         // to handle the corrisponding token
         switch (formatString_[wordCounter]) {
            case 'x':
            {
               temp.x = atof(token);
               break;
            }
            case 'y':
            {
               temp.y = atof(token);
               break;
            }
            case 'z':
            {
               temp.z = atof(token);
               break;
            }
            case 'i':
            {
               temp.intensity = atoi(token);
               break;
            }
            case 'c':
            {
               temp.classification = atoi(token);
               break;
            }
            case 'r':
            {
               temp.packedByte = temp.packedByte | atoi(token);
               break;
            }
            case 't':
            {
               temp.time = atof(token);
               break;
            }
            case 's':
            {
               temp.scanAngle = atoi(token);
               break;
            }
            case 'd':
            {
               temp.packedByte = temp.packedByte | (atoi(token) << 6);
               break;
            }
            case 'n':
            {
               temp.packedByte = temp.packedByte | (atoi(token) << 3);
               break;
            }
            case 'e':
            {
               temp.packedByte = temp.packedByte | (atoi(token) << 7);
               break;
            }
            case 'p':
            {
               temp.pointSourceId = temp.pointSourceId;
               break;
            }
            default:
            {
            }
         }
         token = strtok(NULL, delim);
      }

      // copy the valid points into the array
      points[pointCounter] = temp;
      pointCounter++;
      counter = 0;
   }
   //cout << pointcounter << endl;
   return pointCounter;
}


Boundary* AsciiLoader::getBoundary()
{
   // because retrieveing the boundary takes a long time it is stored and
   // retrieved rather than recalculating if it has been calculated before
   if (boundary_ == NULL)
   {
      boundary_ = new Boundary();
      char delim[2] = "x";
      delim[0] = formatString_[0];
      int wordCounter = 0;
      double temp;
      char *token;
      char line[300];

      // the first point is read in on its own and used to initilize the min and max values
      fgets(line, 300, file_);
      token = strtok(line, delim);
      while (token != NULL)
      {
         wordCounter++;
         if (formatString_[wordCounter] == 'x')
         {
            temp = atof(token);
            boundary_->minX = temp;
            boundary_->maxX = temp;

         }
         else if (formatString_[wordCounter] == 'y')
         {
            temp = atof(token);
            boundary_->minY = temp;
            boundary_->maxY = temp;
         }
         token = strtok(NULL, delim);
      }



      // for each line
      while (fgets(line, 300, file_) != NULL)
      {
         token = strtok(line, delim);
         wordCounter = 0;
         //for each token in that line
         while (token != NULL)
         {
            wordCounter++;
            // if the format string indicates that the token corrisponds to either x or y
            if (formatString_[wordCounter] == 'x')
            {
               temp = atof(token);
               if (temp < boundary_->minX)
               {
                  boundary_->minX = temp;
               }
               if (temp > boundary_->maxX)
               {
                  boundary_->maxX = temp;
               }
            }
            else if (formatString_[wordCounter] == 'y')
            {
               temp = atof(token);
               if (temp < boundary_->minY)
               {
                  boundary_->minY = temp;
               }
               if (temp > boundary_->maxY)
               {
                  boundary_->maxY = temp;
               }
            }
            token = strtok(NULL, delim);
         }
      }
      // reset the file pointer
      fclose(file_);
      file_ = fopen(fileName_, "r");
   }
   return boundary_;
}


const char* AsciiLoader::getFileName()
{
   return fileName_;
}

