#include <cmath>
#include "quadtreestructs.h"
#ifndef MATHFUNCS_H
#define MATHFUNCS_H


//Returns the value of the given percentile of a dataset. Makes a histogram of the dataset and goes along it until it gets a total count corresponding with the percentile.
double percentilevalue(double* data,int datasize,double percentile,double minval,double maxval);

//Determines whether the points in the sent bucket fit within the profile box.
bool* vetpoints(int numberofpoints,point* points,double startx,double starty,double endx,double endy,double width);

#endif
