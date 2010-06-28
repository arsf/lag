
#include "CollisionDetection.h"

#include <math.h>
#include <vector>


using namespace std;

// this method takes the forumula for 4 lines and checks if the x and y arguments are within them

bool kobbPoint(double m1, double c1, double m2, double c2, double m3, double c3, double m4, double c4, double x, double y)
{
   // find the point where an imaginary vertical line through the x value intersects with each line
   double intersec1 = m1 * x + c1;
   double intersec2 = m2 * x + c2;
   double intersec3 = m3 * x + c3;
   double intersec4 = m4 * x + c4;

   // if the xy point is between the intersections with the oposite sides of the rectangle the
   // point is within the rectangle (think about it, it works)
   double err = 0.000001;
   if (((intersec1 - y + err) > 0 && (intersec3 - y - err) < 0) || ((intersec1 - y - err) < 0 && (intersec3 - y + err) > 0))
   {
      if (((intersec2 - y + err) > 0 && (intersec4 - y - err) < 0) || ((intersec2 - y - err) < 0 && (intersec4 - y + err) > 0))
      {
         return true;
      }
   }
   return false;
}

// function to test if a point lies within a polygon using the fact that
// if it does the area of the triangles formed by 2 points from the polgon
// and the test point will all have the same sign

bool triangleTest(double *Xs, double *Ys, int size, double px, double py)
{
   double sign = 0.5 * (Xs[0] * py - Ys[0] * px - Xs[size - 1] * py + Ys[size - 1] * px + Xs[size - 1] * Ys[0] + Ys[size - 1] * Xs[0]);
   for (int k = 0; k < size; k++)
   {
      double test = 0.5 * (Xs[k + 1] * py - Ys[k + 1] * px - Xs[k] * py + Ys[k] * px + Xs[k] * Ys[k + 1] + Ys[k] * Xs[k + 1]);

      if (test * sign < 0)
      {
         return false;
      }

   }
   return true;
}


// function to test if a point lies within a polygon using the fact
// that if it does a vector drawn outwards from that point will intersect
// with the polygon an odd number of times

bool vectortest(double *Xs, double *Ys, int size, double px, double py)
{


   bool in = false;
   int j = size - 1;
   for (int k = 0; k < size; k++)
   {
      if ((Ys[k] < py && Ys[j] >= py) || (Ys[j] < py && Ys[k] >= py))
      {
         if (Xs[k]+((py - Xs[k]) / (Xs[j] - Xs[k])) * (Xs[j] - Xs[k]) < px)
         {
            in = !in;
         }
      }
      j = k;
   }
   return in;
}

bool pointNaoRectangle(double px, double py, double *Xs, double *Ys, int size)
{
   return vectortest(Xs, Ys, size, px, py);
}



// function to detect collision between an axis orientated rectangle and a rotated rectangle

bool aoRectangleNaoRectangle(double minX, double minY, double maxX, double maxY, double *Xs, double *Ys, int size)
{

   // find a simple bounding box for the non axis orientated area,
   double largestX = Xs[0];
   double largestY = Xs[0];
   double smallestX = Ys[0];
   double smallestY = Ys[0];

   for (int k = 1; k < size; k++)
   {
      if (Xs[k] > largestX)
      {
         largestX = Xs[k];
      }
      if (Xs[k] < smallestX)
      {
         smallestX = Xs[k];
      }
      if (Ys[k] > largestY)
      {
         largestY = Ys[k];
      }
      if (Ys[k] < smallestY)
      {
         smallestY = Ys[k];
      }
   }

   // check simple bounding box against axis oriented box
   if (largestX < minX || smallestX > maxX || largestY < minY || smallestY > maxY)
   {
      return false;
   }

   // convert AOrec to arrays of points
   double *bXs = new double[4];
   double *bYs = new double[4];
   bXs[0] = minX;
   bXs[1] = minX;
   bXs[2] = maxX;
   bXs[3] = maxX;
   bYs[0] = minY;
   bYs[1] = maxY;
   bYs[2] = maxY;
   bYs[3] = minY;

   //    compare the boxes as polygons using axis seperation theory
   return (axisSeperationTest(Xs, Ys, 4, bXs, bYs, 4));

   delete[] bXs;
   delete[] bYs;

}


// this method uses the axis seperation test to determine if two convex polygons collide
// the two pairs of arrays it takes as argurments represent the polgons and the points within
// them must be sequential (so that drawing a line from 1 to 2 to 3 to 1 forms a polygon

bool axisSeperationTest(double *poly1Xs, double *poly1Ys, int poly1size, double *poly2Xs, double *poly2Ys, int poly2size)
{
   // these vectors keep track of the perpendicular vectors already checked
   vector<double> previousX = vector<double>();
   vector<double> previousY = vector<double>();

   // for each edge in polygon 1
   int j = poly1size - 1;
   for (int k = 0; k < poly1size; k++)
   {
      double vecx, vecy;
      double abx, aby;

      vecx = poly1Xs[j] - poly1Xs[k];
      vecy = poly1Ys[j] - poly1Ys[k];

      // find the perpendicular vector
      abx = vecy;
      aby = -vecx;

      // check that a parallel vector has not already been checked
      for (unsigned int i = 0; i < previousX.size(); i++)
      {
         if ((previousX[i] * vecy)-(vecx * previousY[i]) == 0)
         {
            continue;
         }
      }

      previousX.push_back(vecx);
      previousY.push_back(vecy);

      // find magnitude of ab
      double abmag = sqrt(abx * abx + aby * aby);

      double min1, max1, min2, max2;

      //for each point in poly1
      min1 = max1 = poly1Xs[0] * abx + poly1Ys[0] * aby / abmag;
      for (int points1 = 1; points1 < poly1size; points1++)
      {
         // project the points onto the perpendicular vector and record the min and max positions
         double temp = poly1Xs[points1] * abx + poly1Ys[points1] * aby / abmag;

         if (temp < min1)
         {
            min1 = temp;
         }
         if (temp > max1)
         {
            max1 = temp;
         }
      }

      //for each point in poly2
      min2 = max2 = poly2Xs[0] * abx + poly2Ys[0] * aby / abmag;
      for (int points2 = 1; points2 < poly2size; points2++)
      {
         // project the points onto the perpendicular vector and record the min and max positions
         double temp = poly2Xs[points2] * abx + poly2Ys[points2] * aby / abmag;

         if (temp < min2)
         {
            min2 = temp;
         }
         if (temp > max2)
         {
            max2 = temp;
         }
      }

      //cout << min1 << " " << max1 << " " << min2 << " " << max2 << endl;
      // if the min max don't overlap when projected on this plane the objects don't collide
      if (min1 > max2 || max1 < min2)
      {
         return false;
      }
      j = k;
   }

   // for each edge in polygon 2
   j = poly2size - 1;
   for (int k = 0; k < poly2size; k++)
   {

      double vecx, vecy;
      double abx, aby;


      vecx = poly2Xs[j] - poly2Xs[k];
      vecy = poly2Ys[j] - poly2Ys[k];

      // find the perpendicular vector
      abx = vecy;
      aby = -vecx;

      // check that a parallel vector has not already been checked
      for (unsigned int i = 0; i < previousX.size(); i++)
      {
         if ((previousX[i] * vecy)-(vecx * previousY[i]) == 0)
         {
            continue;
         }
      }

      // find magnitude of ab
      double abmag = sqrt((abx * abx) + (aby * aby));

      double min1, max1, min2, max2;

      //for each point in poly1
      min1 = max1 = poly1Xs[0] * abx + poly1Ys[0] * aby / abmag;
      for (int points1 = 1; points1 < poly1size; points1++)
      {
         // project the points onto the perpendicular vector and record the min and max positions
         double temp = poly1Xs[points1] * abx + poly1Ys[points1] * aby / abmag;

         if (temp < min1)
         {
            min1 = temp;
         }
         if (temp > max1)
         {
            max1 = temp;
         }
      }

      //for each point in poly2
      min2 = max2 = poly2Xs[0] * abx + poly2Ys[0] * aby / abmag;
      for (int points2 = 1; points2 < poly2size; points2++)
      {
         // project the points onto the perpendicular vector and record the min and max positions
         double temp = poly2Xs[points2] * abx + poly2Ys[points2] * aby / abmag;

         if (temp < min2)
         {
            min2 = temp;
         }
         if (temp > max2)
         {
            max2 = temp;
         }
      }

      // if the min max don't overlap when projected on this vector so the objects don't collide
      if (min1 > max2 || max1 < min2)
      {
         return false;
      }
      j = k;
   }

   return true;
}


