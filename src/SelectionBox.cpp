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

   points[0].move(firstX, firstY, firstZ);
   points[1].move(secondX, secondY, secondZ);
   points[2].move(thirdX, thirdY, thirdZ);
   points[3].move(fourthX, fourthY, fourthZ);
}

SelectionBox::SelectionBox(Point first, Point second, 
                           Point third, Point fourth) {

   points[0] = first;
   points[1] = second;
   points[2] = third;
   points[3] = fourth;
}

// Destructor needs work?
SelectionBox::~SelectionBox() {}

void SelectionBox::move(double moveX, double moveY, double moveZ) {

   for (int i = 0; i < NUMBER_OF_POINTS; i++)
      points[i].move(moveX, moveY, moveZ);
}

void SelectionBox::translate(double transX, double transY, double transZ) {
   for (int i = 0; i < NUMBER_OF_POINTS; i++)
      points[i].translate(transX, transY, transZ);
}

void SelectionBox::movePoint(int pointNumber, double moveX, 
                             double moveY, double moveZ) {
   points[pointNumber].move(moveX, moveY, moveZ);
}

void SelectionBox::translatePoint(int pointNumber, double transX,
                                  double transY, double transZ) {
   points[pointNumber].translate(transX, transY, transZ);
}

Point SelectionBox::getCorner(int cornerNumber) {
   return points[cornerNumber];
}

Point* SelectionBox::getCorners() {
   return points;
}

// TODO FIXME DELETE THESE ONE DAYS
double* SelectionBox::getXs() {
   double* returner = new double[4];
   for (int i=0; i < 4; i++)
      returner[i] = points[i].getX();
   return returner;
}

double* SelectionBox::getYs() {
   
   double* returner = new double[4];
   for (int i=0; i < 4; i++) {
      returner[i] = points[i].getY();
   }
   return returner;
}
