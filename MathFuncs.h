/*
 * File: MathFuncs.h
 * Author: Haraldur Tristan Gunnarsson
 * Written: December 2009 - June 2010
 *
 * */
#include <vector>
#include <iterator>
#include <cmath>
#include "quadtreestructs.h"
#include "pointbucket.h"
using namespace std;
#ifndef MATHFUNCS_H
#define MATHFUNCS_H

//Returns the value of the given percentile of a dataset. Makes a histogram of the dataset and goes along it until it gets a total count corresponding with the percentile.
double percentilevalue(double* data,int datasize,double percentile,double minval,double maxval);

//Determines whether the points in the sent bucket fit within the profile box.
bool* vetpoints(pointbucket* points,double startx,double starty,double endx,double endy,double width);

//Determines whether the points in the sent bucket fit within the profile area.
bool* vetpoints(pointbucket* points,double* xs,double* ys,int numberofcorners);

#endif
