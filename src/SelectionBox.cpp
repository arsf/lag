/* Class to hold info about a selection box.  This will be a rectangle holding
 * four Points - One for each corner.
 */

#include "SelectionBox.h"
#include <stdio.h>

// Point default constructor has already been called, 
// which means the SelectionBox is 4 Points which are 
// all (0.00, 0.00, 0.00)
SelectionBox::SelectionBox() {
}

SelectionBox::SelectionBox(double firstX, double firstY, double firstZ,
                           double secondX, double secondY, double secondZ,
                           double thirdX, double thirdY, double thirdZ,
                           double fourthX, double fourthY, double fourthZ) {

   _points[0].move(firstX, firstY, firstZ);
   _points[1].move(secondX, secondY, secondZ);
   _points[2].move(thirdX, thirdY, thirdZ);
   _points[3].move(fourthX, fourthY, fourthZ);
}

SelectionBox::SelectionBox(Point first, Point second, 
                           Point third, Point fourth) {

   _points[0] = first;
   _points[1] = second;
   _points[2] = third;
   _points[3] = fourth;
}

// Destructor needs work?
SelectionBox::~SelectionBox() {}

void SelectionBox::move(double moveX, double moveY, double moveZ) {

   for (int i = 0; i < NUMBER_OF_POINTS; i++)
      _points[i].move(moveX, moveY, moveZ);
}

void SelectionBox::translate(double transX, double transY, double transZ) {
   for (int i = 0; i < NUMBER_OF_POINTS; i++)
      _points[i].translate(transX, transY, transZ);
}

void SelectionBox::movePoint(int pointNumber, double moveX, 
                             double moveY, double moveZ) {
   _points[pointNumber].move(moveX, moveY, moveZ);
}

void SelectionBox::translatePoint(int pointNumber, double transX,
                                  double transY, double transZ) {
   _points[pointNumber].translate(transX, transY, transZ);
}

Point SelectionBox::getCorner(int cornerNumber) {
   return _points[cornerNumber];
}

Point* SelectionBox::getCorners() {
   return _points;
}

// TODO FIXME DELETE THESE ONE DAYS
double* SelectionBox::getXs() {
   double* returner = new double[4];
   for (int i=0; i < 4; i++)
      returner[i] = _points[i].getX();
   return returner;
}

double* SelectionBox::getYs() {
   
   double* returner = new double[4];
   for (int i=0; i < 4; i++) {
//      printf("%d\n", i);
      returner[i] = _points[i].getY();
   }
   return returner;
}
