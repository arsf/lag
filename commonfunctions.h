#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <cstdlib>

//Header file to contain common functions that may be useful elsewhere

std::string TrimPunctuation(std::string str);
std::string TrimWhitespace(std::string str);
unsigned int StringToUINT(std::string str);
int StringToINT(std::string str);
std::string TrimTrailingChars(std::string str,const char* chars);
std::string TrimLeadingChars(std::string str,const char* chars);
int TotalOccurence(std::string str, const char c);

//Functions to convert the int/float/etc ival into a string
//a template function is "nicer" but would need to be included in all files
inline std::string ToString(short int ival) {std::stringstream ss;  ss<<ival;   return ss.str();}
inline std::string ToString(unsigned short int ival) {std::stringstream ss;  ss<<ival;   return ss.str();}
inline std::string ToString(unsigned int ival) {std::stringstream ss;  ss<<ival;   return ss.str();}
inline std::string ToString(int ival) {std::stringstream ss;  ss<<ival;   return ss.str();}
inline std::string ToString(float ival) {std::stringstream ss;  ss<<ival;   return ss.str();}
inline std::string ToString(double ival) {std::stringstream ss;  ss<<ival;   return ss.str();}

std::string ReplaceAllWith(const std::string* strtosearch, const char replace, const char sub);
std::string RemoveAllBut(std::string str,const char* chars);
std::string GetItemFromString(std::string str,const int itemnum);

//struct to compare two strings using the standard string.h functions {for use in map methods in CommandLine}
#ifndef FUNCTION_CMPSTR
#define FUNCTION_CMPSTR
struct cmpstr
{
   bool operator()(const std::string c1, const std::string c2)const
   {
      return c1.compare(c2) < 0;
   }
};
#endif


#endif
