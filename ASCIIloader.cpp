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
   point temp;
   
   char delim[2] = "x";
   delim[0]=formatstring[0];
   
   boost::char_separator<char> sep(delim);
   int wordcounter;
   int counter = 0;
   int pointcounter = 0;
   
  

      
      char *tok;
      char line[300];
      
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
        while (tok != NULL)
        {

           wordcounter++;


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
   
   
   

      points[pointcounter] = temp;
      pointcounter++;
      counter=0;
   }
   //cout << pointcounter << endl;
   return pointcounter;
}
   
   
int ASCIIloader::load(int n, int nth, point *points)
{

   point temp;
   
   char delim[2] = "x";
   delim[0]=formatstring[0];
   
   boost::char_separator<char> sep(delim);
   int wordcounter;
   int counter = 0;
   int pointcounter = 0;
   
  

      
      char *tok;
      char line[300];
      
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
        while (tok != NULL)
        {

           wordcounter++;


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
   
   
   

      points[pointcounter] = temp;
      pointcounter++;
      counter=0;
   }
   //cout << pointcounter << endl;
   return pointcounter;
}
   
   

boundary* ASCIIloader::getboundary()
{
   if (b == NULL)
   {



      b = new boundary();



      char delim[2] = "x";
      delim[0]=formatstring[0];


      boost::char_separator<char> sep(delim);





      cout << "|" << delim << "|" << endl;
      int wordcounter = 0;
      double temp;
      

      

      
      char *tok;
      char line[300];
      
      
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
      
      while (fgets(line, 300, fp) != NULL) 
      {
 
        tok = strtok(line, delim);
        wordcounter=0;
        while (tok != NULL)
        {
           wordcounter++;
        
         
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

   } 
   fclose(fp);
   fp = fopen(filename, "r");
   return b;
}     
      
      