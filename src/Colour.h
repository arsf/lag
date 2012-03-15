#ifndef _COLOUR_H_
#define _COLOUR_H_

/* Class to hold information about a colour.
 *
 * This will hold a red, green, and _b value.
 */

#include <string>

class Colour {

public:
   Colour();
   Colour(float, float, float);
   Colour(const std::string&);
   ~Colour();

   // Comparison operators
   friend bool operator== (Colour& lhs, Colour& rhs);
   friend bool operator!= (Colour& lhs, Colour& rhs);

   // Multiply
   void multiply(Colour);
   void multiply(float);
   void multiply(float, float, float);

   // Add
   void add(Colour);
   void add(float);
   void add(float, float, float);

   // Subtract
   void subtract(Colour);
   void subtract(float);
   void subtract(float, float, float);

   // Getters
   float getR();
   float getG();
   float getB();
   float* getRGB();
   // Setters
   void setR(float);
   void setG(float);
   void setB(float);
   void setRGB(float, float, float);

private:
   float colourValues[3];
};

#endif
