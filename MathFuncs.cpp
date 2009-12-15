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

bool* vetpoints(int numberofpoints,point* points,double startx,double starty,double endx,double endy,double width){
   double width2 = width/2;
   double error = 0.000001;
   bool* correctpoints = new bool[numberofpoints];
   if(startx==endx){
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
   else if(starty==endy){
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
   else{
      //Lines of bounding box:
      double lengradbox = (endy-starty)/(endx-startx);
      double widgradbox = -1.0/lengradbox;
   //   double lenconsbox = starty - (startx*lengradbox);
      double widconsboxstart = starty - (startx*widgradbox);
      double widconsboxend = endy - (endx*widgradbox);
      double startxleft = startx - width2/sqrt(1+widgradbox*widgradbox);
      double startxright = startx + width2/sqrt(1+widgradbox*widgradbox);
      double startyleft = starty - widgradbox*width2/sqrt(1+widgradbox*widgradbox);
      double startyright = starty + widgradbox*width2/sqrt(1+widgradbox*widgradbox);
      double lenconsboxleft = startyleft - (startxleft*lengradbox);
      double lenconsboxright = startyright - (startxright*lengradbox);
      //Testing points:
      double interstart,interend,interleft,interright;
      for(int i=0;i<numberofpoints;i++){
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
