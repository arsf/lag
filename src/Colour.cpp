/* Class to hold colour values, with the option of extending
 * functionality to do something more exciting
 */

#include "Colour.h"

Colour::Colour()
{
   colourValues[0] = 0.0;
   colourValues[1] = 0.0;
   colourValues[2] = 0.0;
}

Colour::Colour(float r, float g, float b)
{
   colourValues[0] = r;
   colourValues[1] = g;
   colourValues[2] = b;
}

Colour::Colour(const std::string& colourName)
{
   if (colourName == "red") {
      colourValues[0] = 1.0f;
      colourValues[1] = 0.0f;
      colourValues[2] = 0.0f;
   }
   else if (colourName == "white") {
      colourValues[0] = 1.0f;
      colourValues[1] = 1.0f;
      colourValues[2] = 1.0f;
   }
   else if (colourName == "green") {
      colourValues[0] = 0.0f;
      colourValues[1] = 1.0f;
      colourValues[2] = 0.0f;
   }
   else if (colourName == "blue") {
      colourValues[0] = 0.0f;
      colourValues[1] = 0.0f;
      colourValues[2] = 1.0f;
   }
}

void Colour::multiply(Colour other)
{
   colourValues[0] *= other.getR();
   if (colourValues[0] > 1.0) colourValues[0] = 1.0;

   colourValues[1] *= other.getG();
   if (colourValues[1] > 1.0) colourValues[1] = 1.0;

   colourValues[2] *= other.getB();
   if (colourValues[2] > 1.0) colourValues[2] = 1.0;
}

void Colour::multiply(float multiplicant)
{
   colourValues[0] *= multiplicant;
   if (colourValues[0] > 1.0) colourValues[0] = 1.0;

   colourValues[1] *= multiplicant;
   if (colourValues[1] > 1.0) colourValues[1] = 1.0;

   colourValues[2] *= multiplicant;
   if (colourValues[2] > 1.0) colourValues[2] = 1.0;
}

void Colour::multiply(float r, float g, float b)
{
   colourValues[0] *= r;
   if (colourValues[0] > 1.0) colourValues[0] = 1.0;

   colourValues[1] *= g;
   if (colourValues[1] > 1.0) colourValues[1] = 1.0;

   colourValues[2] *= b;
   if (colourValues[2] > 1.0) colourValues[2] = 1.0;
}

void Colour::add(Colour other)
{
   colourValues[0] += other.getR();
   if (colourValues[0] > 1.0) colourValues[0] = 1.0;

   colourValues[1] += other.getG();
   if (colourValues[1] > 1.0) colourValues[1] = 1.0;

   colourValues[2] += other.getB();
   if (colourValues[2] > 1.0) colourValues[2] = 1.0;
}

void Colour::add(float numberToAdd)
{
   colourValues[0] += numberToAdd;
   if (colourValues[0] > 1.0) colourValues[0] = 1.0;

   colourValues[1] += numberToAdd; 
   if (colourValues[1] > 1.0) colourValues[1] = 1.0;

   colourValues[2] += numberToAdd; 
   if (colourValues[2] > 1.0) colourValues[2] = 1.0;
}

void Colour::add(float r, float g, float b)
{
   colourValues[0] += r;
   if (colourValues[0] > 1.0) colourValues[0] = 1.0;

   colourValues[1] += g;
   if (colourValues[1] > 1.0) colourValues[1] = 1.0;

   colourValues[2] += b; 
   if (colourValues[2] > 1.0) colourValues[2] = 1.0;
}

void Colour::subtract(Colour other)
{
   colourValues[0] -= other.getR();
   if (colourValues[0] < 0.0) colourValues[0] = 0.0;

   colourValues[1] -= other.getG();
   if (colourValues[1] < 0.0) colourValues[1] = 0.0;

   colourValues[2] -= other.getB();
   if (colourValues[2] < 0.0) colourValues[2] = 0.0;
}

void Colour::subtract(float numberToSubtract)
{
   colourValues[0] -= numberToSubtract;
   if (colourValues[0] < 0.0) colourValues[0] = 0.0;

   colourValues[1] -= numberToSubtract; 
   if (colourValues[1] < 0.0) colourValues[1] = 0.0;

   colourValues[2] -= numberToSubtract; 
   if (colourValues[2] < 0.0) colourValues[2] = 0.0;
}

void Colour::subtract(float r, float g, float b)
{
   colourValues[0] -= r;
   if (colourValues[0] < 0.0) colourValues[0] = 0.0;

   colourValues[1] -= g;
   if (colourValues[1] < 0.0) colourValues[1] = 0.0;

   colourValues[2] -= b;
   if (colourValues[2] < 0.0) colourValues[2] = 0.0;
}
Colour::~Colour()
{
}

// Getters
float Colour::getR() { return colourValues[0]; }
float Colour::getG() { return colourValues[1]; }
float Colour::getB() { return colourValues[2]; }
float* Colour::getRGB() { return colourValues; }

// Setters
void Colour::setR(float r) { colourValues[0] = r; }
void Colour::setG(float g) { colourValues[1] = g; }
void Colour::setB(float b) { colourValues[2] = b; }
void Colour::setRGB(float r, float g, float b)
{
   colourValues[0] = r;
   colourValues[1] = g;
   colourValues[2] = b;
}
