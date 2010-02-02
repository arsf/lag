/*
 * File: MathFuncs.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: December 2009 - January 2010
 *
 * */
#include "MathFuncs.h"

//Returns the value of the given percentile of a dataset. Makes a histogram of the dataset and goes along it until it gets a total count corresponding with the percentile.
double percentilevalue(double* data,int datasize,double percentile,double minval,double maxval){
   unsigned long int* histogram = new unsigned long int[(unsigned long int)floor(maxval-minval)+1];
   for(unsigned int i=0;i<(unsigned long int)floor(maxval-minval);i++)histogram[i]=0;//Fill histogram with zeroes.
   for(int i=0;i<datasize;i++)histogram[(unsigned long int)floor(data[i]-minval)]++;//Increment histogram indeices to give it "shape".
   int temp=0,index=0;
   do{//Determine value of percentile.
      temp+=histogram[index];
      index++;
   }while(temp<(percentile*(double)datasize)/100.0);
   double percval=index+minval;
   delete[] histogram;
   return percval;
}

//Determines whether the points in the sent bucket fit within the profile box.
bool* vetpoints(int numberofpoints,point* points,double startx,double starty,double endx,double endy,double width){
   double width2 = width/2;
   double error = 0.000001;//Allows points on the line to be counted. May or may not be necessary/desirable.
   bool* correctpoints = new bool[numberofpoints];//This will be returned later.
   if(startx==endx){//If the profile is parallel to the y axis:
      for(int i=0;i<numberofpoints;i++){
         correctpoints[i]=false;
         if((starty-points[i].y+error>0 && endy-points[i].y-error<0) ||
            (starty-points[i].y-error<0 && endy-points[i].y+error>0)){
            if((startx+width2-points[i].x+error>0 && startx-width2-points[i].x-error<0) ||
               (startx+width2-points[i].x-error<0 && startx-width2-points[i].x+error>0)){
               correctpoints[i] = true;
            }
         }
      }
   }
   else if(starty==endy){//If the profile is parallel to the x axis:
      for(int i=0;i<numberofpoints;i++){
         correctpoints[i]=false;
         if((startx-points[i].x+error>0 && endx-points[i].x-error<0) ||
            (startx-points[i].x-error<0 && endx-points[i].x+error>0)){
            if((starty+width2-points[i].y+error>0 && starty-width2-points[i].y-error<0) ||
               (starty+width2-points[i].y-error<0 && starty-width2-points[i].y+error>0)){
               correctpoints[i] = true;
            }
         }
      }
   }
   else{//If the profile is skewed:
      //Lines of bounding box:
      double lengradbox = (endy-starty)/(endx-startx);//Gradients of length and width lines
      double widgradbox = -1.0/lengradbox;//...
      double widconsboxstart = starty - (startx*widgradbox);//Constant values (y intercept) of the formulae for the sides of the box perpendiculr to the direction of the profile
      double widconsboxend = endy - (endx*widgradbox);//...
      double startxleft = startx - width2/sqrt(1+widgradbox*widgradbox);//Values for the end points of the starting perpendicular side
      double startxright = startx + width2/sqrt(1+widgradbox*widgradbox);//...
      double startyleft = starty - widgradbox*width2/sqrt(1+widgradbox*widgradbox);//...
      double startyright = starty + widgradbox*width2/sqrt(1+widgradbox*widgradbox);//...
      double lenconsboxleft = startyleft - (startxleft*lengradbox);//The constants for the longitudinal sides relative to the profile line itself.
      double lenconsboxright = startyright - (startxright*lengradbox);//...
      //Testing points:
      double interstart,interend,interleft,interright;
      for(int i=0;i<numberofpoints;i++){//Tests whether the the points, if they each have a line parallel to the y axis on them, intersect with all four sides of the box. If so, they fit within the profile and should be drawn
         interstart = points[i].x * widgradbox + widconsboxstart;
         interend = points[i].x * widgradbox + widconsboxend;
         interleft = points[i].x * lengradbox + lenconsboxleft;
         interright = points[i].x * lengradbox + lenconsboxright;
         correctpoints[i]=false;
         if((interstart-points[i].y+error>0 && interend-points[i].y-error<0) ||
            (interstart-points[i].y-error<0 && interend-points[i].y+error>0)){
            if((interleft-points[i].y+error>0 && interright-points[i].y-error<0) ||
               (interleft-points[i].y-error<0 && interright-points[i].y+error>0)){
               correctpoints[i] = true;
            }
         }
      }
   }
   return correctpoints;
}

//

//struct CompCallback{
//   CompCallback(Profile *ptr,void(*f)(Profile *)):
//}

//int get_closest_element_position(point* value,vector<point*>::iterator first,vector<point*>::iterator last,bool (*comp)(point*,point*)){
//int get_closest_element_position(point* value,vector<point*>::iterator first,vector<point*>::iterator last,Profile *prof){
//   vector<point*>::iterator originalFirst = first;
//   vector<point*>::iterator middle;
//   while(true){//INFINITE LOOP interrupted by returns.
//      middle = first + distance(first,last)/2;
//      if(comp(*middle,value))last = middle;
//      else if(comp(value,*middle))first = middle;
//      if(prof->linecomp(*middle,value))last = middle;
//      else if(prof->linecomp(value,*middle))first = middle;
//      else return distance(originalFirst,middle);
//      if(distance(first,last)==0)return distance(originalFirst,middle);
//   }
//}
