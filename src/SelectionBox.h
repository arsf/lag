#ifndef _SELECTIONBOX_H_
#define _SELECTIONBOX_H_

#include "Point.h"

class SelectionBox
{

public:
   SelectionBox() {}

   SelectionBox(double, double, double,
                double, double, double,
                double, double, double,
                double, double, double);

   SelectionBox(Point, Point, Point, Point);

   ~SelectionBox();

   Point getCorner(int);
   Point* getCorners();

   // TODO FIXME
   //
   // Temp methods to return arrays containing x and y coords
   // Which I'll fix once I get this compiling
   double* getXs();
   double* getYs();
   
   // Mutators for the whole SelectionBox
   void move(double, double, double);
   void translate(double, double, double);

   // Mutators for the individual Points.
   void movePoint(int, double, double, double);
   void translatePoint(int, double, double, double);

   // Rotate one day?

private:
   static const int NUMBER_OF_POINTS = 4;
   Point points[4];
};

#endif
