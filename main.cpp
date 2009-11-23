/* 
 * File:   main.cpp
 * Author: chrfi
 *
 * Created on October 6, 2009, 4:21 PM
 */

#include "quadtree.h"





#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "quadtree.h"
#include "quadtreestructs.h"
#include <vector>
#include <unistd.h>
#include "liblas/laspoint.hpp"
#include "liblas/lasreader.hpp"
#include "lidar_loader.h"
#include "LASloader.h"
using namespace std;


/*
 * 
 */
int main(int argc, char** argv) {
    
    cout << "hello world";
    cout << " int " << sizeof(int) << endl;
    cout << " float " << sizeof(float) << endl;
    cout << " double " << sizeof(double) << endl;
    cout << " char " << sizeof(char) << endl;
    cout << " unisgned short " << sizeof(unsigned short) << endl;
    cout << " unsigned char " << sizeof(unsigned char) << endl;
    cout << " int " << sizeof(int) << endl;
    
    
    
   
    
    //bob->toString();
    cout << "construction begun" << endl;
    quadtree *bob; 
    LASloader *l = new LASloader("/home/scratch/LDR090601_115048_1.LAS");
    boundary *b = l->getboundary();
    try 
    {
      bob = new quadtree(l, 500, 0, b->minX, b->minY+2000, b->maxX, b->maxY-2000);
     }
         catch (char const* c)
      {
         cout << c << endl;
      }
    //  bob = new quadtree("/home/scratch/LDR090601_115048_1.LAS", 100000, 0);
   /* bob = new quadtree(0, 0, 10, 10, 5);
    
    point temp;
    temp.x = 1;
    temp.y = 1;
    temp.z = 1.5;
    bob->insert(temp);
    
    temp.x = 2;
    temp.y = 2;
    temp.z = 4.5;
    bob->insert(temp);
    temp.x = 3;
    temp.y = 4;
    temp.z = 8.5;
    bob->insert(temp);
    temp.x = 5;
    temp.y = 3;
    temp.z = 2.4;
    bob->insert(temp);
    temp.x = 9;
    temp.y = 2;
    temp.z = 9.6;
    bob->insert(temp);
    temp.x = 4;
    temp.y = 8;
    temp.z = 3.8;
    bob->insert(temp);
    temp.x = 7;
    temp.y = 3;
    temp.z = 6.7;
    bob->insert(temp);
    temp.x = 5;
    temp.y = 1;
    temp.z = 5.9;
    bob->insert(temp);
    temp.x = 1;
    temp.y = 3;
    temp.z = 7.7;
    bob->insert(temp);*/
    
    //bob->print();
    //newloader *bibble = new newloader("/home/scratch/LDR090601_110650_1.LAS");
  //  bob->load("/home/scratch/LDR090601_110312_1.LAS", 0);
   // bob->load(bibble, 0);
    //bob->load("/home/scratch/LDR090601_111020_1.LAS", 0);
    //bob->load("/home/scratch/LDR090601_111411_1.LAS", 0);
    //bob->load("/home/scratch/LDR090601_111753_1.LAS", 0);
  //bob->load("/home/scratch/LDR090601_112149_1.LAS", 0);
   // bob->load("/home/scratch/LDR090601_112524_1.LAS", 0);
   // bob->load("/home/scratch/LDR090601_112915_1.LAS", 0);
    //bob->load("/home/scratch/LDR090601_113327_1.LAS", 5);
   //bob->load("/home/scratch/LDR090601_114640_1.LAS", 5);
   // bob->load("/home/scratch/LDR090601_115533_1.LAS", 5);
    
   
    
 
    
    cout << "construction complete" <<endl;
   
    sleep(1);
    
   // cout << "sorting by height" << endl;
   // bob->sort('H');
    //cout << "sorted" << endl;
    
    cout << "subseting" <<endl;
    boundary* z = bob->getboundary();
    vector<pointbucket*> *wibble = bob->advsubset(z->minX,z->maxY,z->maxX,z->minY, 10000);
    cout << "subset complete" <<endl;
    long counter = 0;
    for (unsigned int k=0; k< wibble->size(); k++)
    {
       counter+=wibble->at(k)->numberofpoints;
    }
    cout << "number of points in subset = " << counter << endl;
    
    boundary* temp = bob->getboundary();
    cout << temp->minX << endl;
    cout << temp->minY << endl;
    cout << temp->maxX << endl;
    cout << temp->maxY << endl;
   // bob->toString();
 
    cout << "deleteing numbers" << endl;
    delete bob;
    cout << "numbers deleted" << endl;
    //delete bill;
    return (EXIT_SUCCESS);
}

