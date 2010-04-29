/**************************************************************************
*     COMMONFUNCTIONS.CPP - PROGRAMMED BY M WARREN - DECEMBER 1ST 2008
*     
*     Collection of functions that are required for various other 
*     cpp files (such as BILReader.cpp, commandline.cpp) 
***************************************************************************/

#include <string>
#include "commonfunctions.h"

//String function to strip leading and trailing whitespace 
std::string TrimWhitespace(std::string str)
{
   //This will trim both leading and trailing whitespace characters
   std::string whitespace(" \t\n\r\f\v");
   int index=str.find_last_not_of(whitespace); //find last occurrance of non-whitespace
   if(index!=std::string::npos)
      str.erase(index+1); //erase everything after this character
   else
      str.clear();   //entire string is whitespace

   //Now trim leading characters
   index=str.find_first_not_of(whitespace);
   if(index!=std::string::npos)
      str.erase(0,index); //erase everything after this character

   return str;
}

//This function will trim the trailing punctuation of a string
std::string TrimPunctuation(std::string str)
{
   //This will trim trailing punctuation characters
   std::string punct(",;.:{}");
   int index=str.find_last_not_of(punct); //find last occurrance of non-whitespace
   if(index!=std::string::npos)
      str.erase(index+1); //erase everything after this character
   else
      str.clear();   //entire string is punctuation

   //Now trim leading characters
   //index=str.find_first_not_of(punct);
   //if(index!=std::string::npos)
   //   str.erase(0,index); //erase everything after this character

   return str;
}

//This function will trim the trailing punctuation of a string
std::string TrimTrailingChars(std::string str,const char* chars)
{
   //This will trim trailing characters
   std::string char_string(chars);
   int index=str.find_last_not_of(char_string); //find last occurrance of non-whitespace
   if(index!=std::string::npos)
      str.erase(index+1); //erase everything after this character
   else
      str.clear();   //entire string is made up of only characters from chars array

   return str;
}

//This function will trim the trailing punctuation of a string
std::string TrimLeadingChars(std::string str,const char* chars)
{
   //This will trim trailing characters
   std::string char_string(chars);
   int index=str.find_first_not_of(char_string); //find last occurrance of non-whitespace
   if(index!=std::string::npos)
      str.erase(0,index); //erase everything after this character
   else
      str.clear();   //entire string is made up of only characters from chars array

   return str;
}


//This function will remove all occurrences of chars in str
std::string RemoveAllBut(std::string str,const char* chars)
{
      std::string char_string(chars);
      int index=str.find_first_not_of(char_string); //find first non-chars char
      while(index!=std::string::npos)
      {
         str.erase(index,1);
         index=str.find_first_not_of(char_string);
      }
      return str;
}

//This function will get the itemnum'th element from the string delimited by space
std::string GetItemFromString(std::string str,const int itemnum)
{
   int i=0,index=0,pos=0;
   if(itemnum==0)
   {
      index=str.find(' ',pos); //find first space char
      return str.substr(pos,index-pos);
   }
   else
   {
      while(i<itemnum)
      {
         index=str.find(' ',pos); //find first space char
         pos=index+1;
         i++;
      }   
      if(index!=std::string::npos)
         return str.substr(pos,index-pos);
      else
         return "";
   } 
   return "";
}


//Function to convert a string to unsigned int
unsigned int StringToUINT(std::string str)
{
   std::string integers("0123456789"); //create a string containing integers
   int index=str.find_first_not_of(integers);//find first non-occurance
   unsigned int retval=0; //return value
   if(index == std::string::npos)
   {
      //Only integers found...therefore safe(-ish) to convert
      retval=(unsigned)atoi(str.c_str());
   }
   else
   {
      //Non integers found therefore unsafe to convert
      retval=0; //will have to return 0 as no other suitable value can be returned
   }
   return retval;    
}


//Function to convert a string to a signed int
int StringToINT(std::string str)
{
   std::string integers("0123456789"); //create a string containing integers
   int index=str.find_first_not_of(integers);//find first non-occurance
   int retval=0; //return value
   if(index == std::string::npos)
   {
      //Only integers found...therefore safe(-ish) to convert
      retval=atoi(str.c_str());
   }
   else if((index == 0) && (str[0]=='-' )) //minus sign at start of string
   {
      index=str.find_first_not_of(integers,1);//check for other non-integers
      if(index == std::string::npos) //none found so convert to number
         retval=atoi(str.c_str());
   }
   else
   {
      //Non integers found therefore unsafe to convert
      retval=0; //will have to return 0 as no other suitable value can be returned
   }
   return retval;    
}


// function to count the number of occurences of c in str
int TotalOccurence(std::string str, const char c)
{
   int l=0;
   int count=0;
   while(l!=std::string::npos)
   {
      l=str.find(c,++l);
      ++count;
   }
   return count-1;
}

//Function to replace all occurences of "replace" with "sub" in string strtosearch
std::string ReplaceAllWith(const std::string* strtosearch, const std::string replace, const std::string sub)
{
   std::string newstr("");
   int index=strtosearch->find_first_of(replace);
   if(index==std::string::npos)
   {
      // replace is not in strtosearch
      newstr.assign(*strtosearch);
      return newstr;
   }
   else
   {
      int c_count=0;
      while(c_count<strtosearch->length())
      {
         
         c_count++;
      }
   }
   return newstr;
}

//Function to replace all occurences of "replace" with "sub" in string strtosearch
std::string ReplaceAllWith(const std::string* strtosearch, const char replace, const char sub)
{
   std::string newstr("");
   newstr.assign(*strtosearch);//copy strtosearch to newstr
   int index=strtosearch->find_first_of(replace);
   if(index==std::string::npos)
   {
      // replace is not in strtosearch
      return newstr;
   }
   else
   {
      int c_count=0;
      while(c_count<strtosearch->length())
      {
         if(strtosearch->at(c_count)==replace)
            newstr.replace(c_count,1,&sub,1);
         c_count++;
      }
   }
   return newstr;
}


