// see lidarpointloader.h
   
#include "ASCIIloader.h"
#include <string>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <cstring>
#include <stdio.h>



ASCIIloader::ASCIIloader(const char *filename, const char *newformatstring)
{
   ifs.open (filename); 
   formatstring = newformatstring;
   this->filename = filename;
   b=NULL;
   
   fp = fopen(filename, "r");
   if(fp == NULL)
   {
      throw "error opening file";
   }
}


ASCIIloader::~ASCIIloader()
{
      
}
   
   
int ASCIIloader::load(int n, int nth, point *points, double minX, double minY, double maxX, double maxY)
{
   
   
   char delim[2] = "x";
   delim[0]=formatstring[0];
   
   point temp;
   int wordcounter;
   int counter = 0;
   int pointcounter = 0;
   char *tok;
   char line[300];
   

   // get each line till enough points are loaded or the file ends
   while (pointcounter != n && fgets(line, 300, fp) != NULL) 
   {

     // discard if not nth point
      if (counter != nth)
      {
         counter++;
         continue;
      }


      // set temp point to null, this is important as unlike when a las file is loaded
      // when an ascii file is loaded it is not garunteed that it contains all the relevent
      // data.
      temp.x = NULL;
      temp.y = NULL;
      temp.z = NULL;
      temp.time = NULL;
      temp.intensity = NULL;
      temp.classification = NULL;
      temp.rnumber = NULL;

      
      tok = strtok(line, delim);
      wordcounter=0;
      
      //loop untill the line has no more tokens
      while (tok != NULL)
      {

         wordcounter++;


         // use the corrisponding character in the format string to determine how
         // to handle the corrisponding token
         switch(formatstring[wordcounter])
         {
            case 'x':
            {
               temp.x = atof(tok);
               break;
            }
            case 'y':
            {
               temp.y = atof(tok);
               break;
            }
            case 'z':
            {
               temp.z = atof(tok);
               break;
            }
            case 'i':
            {
               temp.intensity = atoi(tok);
               break;
            }
            case 'c':
            {
               temp.classification = atoi(tok);
               break;
            }
            case 'r':
            {
               temp.rnumber = atoi(tok);
               break;
            }
            case 't':
            {
               temp.time = atof(tok);
               break;
            }
            default:
            {
            }
         }
         tok = strtok (NULL, delim);
      }

      // check if point falls within area of intrest
      if (temp.x < minX || temp.x > maxX || temp.y < minY || temp.y > maxY)
      {
         continue;
      }
   
      // copy the valid points into the array
      points[pointcounter] = temp;
      pointcounter++;
      counter=0;
   }
   //cout << pointcounter << endl;
   return pointcounter;
}
   
   
int ASCIIloader::load(int n, int nth, point *points)
{

   
   
   char delim[2] = "x";
   delim[0]=formatstring[0];
   
   point temp;
   int wordcounter;
   int counter = 0;
   int pointcounter = 0;
   char *tok;
   char line[300];
   
   
   // get each line till enough points are loaded or the file ends
   while (pointcounter != n && fgets(line, 300, fp) != NULL) 
   {

      // discard if not nth point
      if (counter != nth)
      {
         counter++;
         continue;
      }

      tok = strtok(line, delim);
      wordcounter=0;
     
      //loop untill the line has no more tokens
      while (tok != NULL)
      {

         wordcounter++;

         // use the corrisponding character in the format string to determine how
         // to handle the corrisponding token
         switch(formatstring[wordcounter])
         {
            case 'x':
            {
               temp.x = atof(tok);
               break;
            }
             case 'y':
            {
               temp.y = atof(tok);
               break;
            }
            case 'z':
            {
               temp.z = atof(tok);
               break;
            }
            case 'i':
            {
               temp.intensity = atoi(tok);
               break;
            }
            case 'c':
            {
               temp.classification = atoi(tok);
               break;
            }
            case 'r':
            {
               temp.rnumber = atoi(tok);
               break;
            }
            case 't':
            {
               temp.time = atof(tok);
               break;
            }
            default:
            {
            }
         }
         tok = strtok (NULL, delim);
      }
   
   
   
      // copy the valid points into the array
      points[pointcounter] = temp;
      pointcounter++;
      counter=0;
   }
   //cout << pointcounter << endl;
   return pointcounter;
}
   
   

boundary* ASCIIloader::getboundary()
{
   // because retrieveing the boundary takes a long time it is stored and
   // retrieved rather than reloading if it has been loaded before
   if (b == NULL)
   {
      
      b = new boundary();
      char delim[2] = "x";
      delim[0]=formatstring[0];
      int wordcounter = 0;
      double temp;     
      char *tok;
      char line[300];
      
      
      // the first point is read in on its own and used to initilize the min and max values
      fgets(line, 300, fp);
      tok = strtok(line, delim);
      while (tok != NULL)
      {
         wordcounter++;
         if(formatstring[wordcounter]=='x')
         {
            temp = atof(tok);
            b->minX = temp;
            b->maxX = temp;

         }
         else if(formatstring[wordcounter]=='y')
         {
            temp = atof(tok);
            b->minY = temp;
            b->maxY = temp;
         }
         tok = strtok (NULL, delim);
      }
      
      
      
      // for each line
      while (fgets(line, 300, fp) != NULL) 
      {
 
        tok = strtok(line, delim);
        wordcounter=0;
        //for each token in that line
        while (tok != NULL)
        {
           wordcounter++;
        
            // if the format string indicates that the token corrisponds to either x or y
            if(formatstring[wordcounter]=='x')
            {
               temp = atof(tok);
               if (temp < b->minX){b->minX = temp;}
               if (temp > b->maxX){b->maxX = temp;}

            }
            else if(formatstring[wordcounter]=='y')
            {
               temp = atof(tok);
               if (temp < b->minY){b->minY = temp;}
               if (temp > b->maxY){b->maxY = temp;}
            }
         tok = strtok (NULL, delim);
         }
         
      }
   // reset the file pointer
   fclose(fp);
   fp = fopen(filename, "r");
   } 
   
   return b;
}     
      
      