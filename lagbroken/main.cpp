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
#include "ASCIIloader.h"
#include <sstream>
#include "cacheminder.h"
using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{

    cout << "hello world";
    cout << " int " << sizeof (int) << endl;
    cout << " float " << sizeof (float) << endl;
    cout << " double " << sizeof (double) << endl;
    cout << " char " << sizeof (char) << endl;
    cout << " unisgned short " << sizeof (unsigned short) << endl;
    cout << " unsigned char " << sizeof (unsigned char) << endl;
    cout << " int " << sizeof (int) << endl;

/*
    cacheminder *MCP = new cacheminder(5);
    pointbucket *bob = new pointbucket(10000000, 0.0, 0.0, 50.0, 50.0, MCP);
    point b;
    b.x = 1;
    b.y = 2;
    for (int k = 0; k < 10000000; k++)
    {
        bob->points[k] = b;
    }
    point stupidcompiler;
    cout << "start function test" << endl;
    for (int x = 0; x < 100; x++)
    {
        for (int k = 0; k < 10000000; k++)
        {
            bob->getpoint(k).x=4;
        }
    }
    cout << "end function test" << endl;

    cout << "start array test" << endl;
    for (int k = 0; k < 100; k++)
    {
        for (int k = 0; k < 10000000; k++)
        {
            bob->points[k].x=4;
        }
    }
    cout << "end array test" << endl;
*/

     //bob->toString();
     cout << "construction begun" << endl;
     quadtree *bob;
     //ASCIIloader *l = new ASCIIloader("/home/scratch/LDR-GB08_02-200928101.txt" , " txyzicr#");
     LASloader *l = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_122647_1.LAS");
     LASloader *l2 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_123711_1.LAS");
     LASloader *l3 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_115218_1.LAS");
     LASloader *l4 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_115948_1.LAS");
     LASloader *l5 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_121017_1.LAS");



     //LASloader *l = new LASloader("/home/scratch/LDR090601_111411_1.LAS");
    
     //bob->load("/home/scratch/LDR090601_111411_1.LAS", 0);
     //bob->load("/home/scratch/LDR090601_111753_1.LAS", 0);
     //boundary *b = l->getboundary();
     //cout << b->minX << " " << b->minY << " " << b->maxX << " " << b->maxY << endl;
     ostringstream *outs = new ostringstream();
    
     try
     {
        bob = new quadtree(l, 1000000, 0, 50000000);
        cout << " done" << endl;
        bob->load(l2, 0);
        cout << " done" << endl;
        bob->load(l3, 0);
        cout << " done" << endl;
        bob->load(l4, 0);
        cout << " done" << endl;
        bob->load(l5, 0);
        cout << " done" << endl;
     }
     catch (const char* c)
     {
        cout << c << endl;
     }
      cout << "MAGIC!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
 sleep(15);

  /*   delete(bob);
    sleep(5);
    
     try
     {
        LASloader *l2 = new LASloader("/home/scratch/LDR090601_112524_1.LAS");
        //bob->load(l2, 0);
        bob = new quadtree(l2, 50000, 0, 1000000000);
     }
     catch (const char* c)
     {
        cout << c << endl;
     }
        
     sleep(5);
     try
     {
        cout << bob->getfilename(1);
     }
     catch (const char* c)
     {
        cout << c << endl;
     }*/

    /*
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
   
     //sleep(1);
    
    // cout << "sorting by height" << endl;
    // bob->sort('H');
     //cout << "sorted" << endl;
    
     cout << "subseting" <<endl;
     boundary* z = bob->getboundary();
     vector<pointbucket*> *wibble = bob->subset(z->minX-10,z->minY-10,z->maxX+10,z->maxY+10);
     cout << "subset complete, " << wibble->size() << " buckets" << endl;
     long counter = 0;
     for (unsigned int k=0; k< wibble->size(); k++)
     {
        counter+=wibble->at(k)->numberofpoints;
        cout << wibble->at(k)->getpoint(0).x;
        cout.flush();
     }
     cout << endl;
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
     cout << outs->str() << endl;
     //delete bill;
    return (EXIT_SUCCESS);
}

