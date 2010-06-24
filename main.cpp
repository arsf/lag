/* 
 * File:   main.cpp
 * Author: chrfi
 *
 * Created on October 6, 2009, 4:21 PM
 */

<<<<<<< HEAD
#include "quadtree.h"
#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "boost/thread.hpp"

=======
>>>>>>> clean_code

#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include "boost/thread.hpp"
#include <stdlib.h>
#include <malloc.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "quadtree.h"
#include "quadtreestructs.h"
#include <vector>
#include <unistd.h>
#include "liblas/laspoint.hpp"
#include "liblas/lasreader.hpp"
#include "quadtreeexceptions.h"
#include "LASloader.h"
#include "ASCIIloader.h"
#include <sstream>
#include "collisiondetection.h"
#include "LASsaver.h"
#include "time.h"
#include "cacheminder.h"

using namespace std;

/*
 * 
 */





pointbucket **buckets;
int numberbuckets;
quadtree *bob;

void testfunc()
{
   cout << "stopping for a nap" << endl;
   //malloc_stats();

   cout << "that's better :)" << endl;
   for (int k = 0; k < numberbuckets; k++)
   {
      double sid, y, z;

      for (int i = 0; i < buckets[k]->getnumberofpoints(0); i += 7)
      {

         buckets[k]->getpoint(i,0).x;
         buckets[k]->getpoint(i,0).y;
         buckets[k]->getpoint(i,0).z;


      }
   }

   //delete[] buckets;
}

void dostuff()
{

   LASloader *l = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_122647_1.LAS");
   LASloader *l2 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_123711_1.LAS");
   LASloader *l3 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_115218_1.LAS");
   LASloader *l4 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_115948_1.LAS");
   LASloader *l5 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_121017_1.LAS");


   bob = new quadtree(0, 0, 1, 1, 100000, 30000000, 5);
<<<<<<< HEAD
   bob->load(l2, 0);
   cout << " done" << endl;
   cout << "wait a sec" << endl;
   sleep(5);
   bob->load(l, 0);
   cout << " done" << endl;
   cout << "wait a sec" << endl;
   sleep(5);
   bob->load(l3, 0);
=======
   bob->load(l2, 0, 0);
   cout << " done" << endl;
   cout << "wait a sec" << endl;
   sleep(5);
   bob->load(l, 0, 0);
   cout << " done" << endl;
   cout << "wait a sec" << endl;
   sleep(5);
   bob->load(l3, 0, 0);
>>>>>>> clean_code
}

int main(int argc, char** argv)
{
   double *Xs = new double[4];
   double *Ys = new double[4];
   Xs[0] = 1;
   Ys[0] = 4;
   Xs[1] = 3;
   Ys[1] = 7;
   Xs[2] = 6;
   Ys[2] = 4;
   Xs[3] = 4;
   Ys[3] = 1;
   if (AOrec_NAOrec(4, 3, 60, 90, Xs, Ys, 4))
   {
      cout << "it's a me mario" << endl;
   }
   else
   {
      cout << "oh noes" << endl;
   }

   if (point_NAOrec(5, 4, Xs, Ys, 4))
   {
      cout << "its in the box" << endl;
   }

   liblas::LASPoint p = liblas::LASPoint();
   point p2 = point();


   cout << sizeof (p) << endl;
   cout << sizeof (p2) << endl;

   cout << "hello world";
   cout << " int " << sizeof (int) << endl;
   cout << " float " << sizeof (float) << endl;
   cout << " double " << sizeof (double) << endl;
   cout << " char " << sizeof (char) << endl;
   cout << " unisgned short " << sizeof (unsigned short) << endl;
   cout << " unsigned char " << sizeof (unsigned char) << endl;
   cout << " int " << sizeof (int) << endl;
   point bgt = point();
   cout << " point " << sizeof (bgt) << endl;

   int x = 1 % 1;
   cout << "pow " << pow(3,0) << endl;

   //bob->toString();
   cout << "construction begun" << endl;
ostringstream *outs = new ostringstream();
   time_t stopwatch;
   time_t totaltime;

  // ASCIIloader *l = new ASCIIloader("/home/scratch/LDR-GB08_02-200928101.txt" , " txyzicr#");
   
   LASloader *l7 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_122647_1.las");
   LASloader *l8 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_123711_1.las");
   LASloader *l9 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_115218_1.las");
   LASloader *l10 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_115948_1.las");
   LASloader *l11 = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_121017_1.las");
   //
   LASloader *l = new LASloader("/users/rsg/arsf/workspace/GB08_08-2009_154a_Wytham_woods/leica/proc_laser/LDR090603_114730_1.LAS");
   LASloader *l2 = new LASloader("/users/rsg/arsf/workspace/GB08_08-2009_154a_Wytham_woods/leica/proc_laser/LDR090603_115233_1.LAS");
   LASloader *l3 = new LASloader("/users/rsg/arsf/workspace/GB08_08-2009_154a_Wytham_woods/leica/proc_laser/LDR090603_120010_1.LAS");
   LASloader *l4 = new LASloader("/users/rsg/arsf/workspace/GB08_08-2009_154a_Wytham_woods/leica/proc_laser/LDR090603_120339_1.LAS");
   LASloader *l5 = new LASloader("/users/rsg/arsf/workspace/GB08_08-2009_154a_Wytham_woods/leica/proc_laser/LDR090603_120801_1.LAS");
   LASloader *l6 = new LASloader("/users/rsg/arsf/workspace/GB08_08-2009_154a_Wytham_woods/leica/proc_laser/LDR090603_121403_1.LAS");

     LASloader *loader1 = new LASloader("/users/rsg/arsf/workspace/ET07_01-2009_299_Afar_north/leica/proc_laser/LDR091026_060148_1_classified.LAS");
     LASloader *loader2 = new LASloader("/users/rsg/arsf/workspace/ET07_01-2009_299_Afar_north/leica/proc_laser/LDR091026_060903_1_classified.LAS");
     LASloader *loader3 = new LASloader("/users/rsg/arsf/workspace/ET07_01-2009_299_Afar_north/leica/proc_laser/LDR091026_061706_1_classified.LAS");
     LASloader *loader4 = new LASloader("/users/rsg/arsf/workspace/ET07_01-2009_299_Afar_north/leica/proc_laser/LDR091026_062508_1_classified.LAS");
     LASloader *loader5 = new LASloader("/users/rsg/arsf/workspace/ET07_01-2009_299_Afar_north/leica/proc_laser/LDR091026_064145_1_classified.LAS");
     LASloader *loader6 = new LASloader("/users/rsg/arsf/workspace/ET07_01-2009_299_Afar_north/leica/proc_laser/LDR091026_065032_1_classified.LAS");
     LASloader *loader7 = new LASloader("/users/rsg/arsf/workspace/ET07_01-2009_299_Afar_north/leica/proc_laser/LDR091026_070213_1_classified.LAS");
     LASloader *loader8 = new LASloader("/users/rsg/arsf/workspace/ET07_01-2009_299_Afar_north/leica/proc_laser/LDR091026_072520_1_classified.LAS");


//     LASloader *loader1 = new LASloader("/tmp/lidar/LDR091026_060148_1_classified.LAS");
//     LASloader *loader2 = new LASloader("/tmp/lidar/LDR091026_060903_1_classified.LAS");
//     LASloader *loader3 = new LASloader("/tmp/lidar/LDR091026_061706_1_classified.LAS");
//     LASloader *loader4 = new LASloader("/tmp/lidar/LDR091026_062508_1_classified.LAS");
//     LASloader *loader5 = new LASloader("/tmp/lidar/LDR091026_064145_1_classified.LAS");
//     LASloader *loader6 = new LASloader("/tmp/lidar/LDR091026_065032_1_classified.LAS");
//     LASloader *loader7 = new LASloader("/tmp/lidar/LDR091026_070213_1_classified.LAS");
//     LASloader *loader8 = new LASloader("/tmp/lidar/LDR091026_072520_1_classified.LAS");
   //LASloader *test = new LASloader("/tmp/lidar/LDR090603_114730_1.LAS");


   //LASloader *l = new LASloader("/home/scratch/LDR090601_111411_1.LAS");

   //bob->load("/home/scratch/LDR090601_111411_1.LAS", 0);
   //bob->load("/home/scratch/LDR090601_111753_1.LAS", 0);
   //boundary *b = l->getboundary();
   //cout << b->minX << " " << b->minY << " " << b->maxX << " " << b->maxY << endl;
   

   //  boost::thread threadythethird;
   // threadythethird = boost::thread(dostuff);
   //  threadythethird.join();
   //  dostuff();
   


    //  try
      {
         

         stopwatch = time(NULL);
         totaltime = stopwatch;
         bob = new quadtree(loader1, 100000, 0, 35000000, 5, 6, 4);
         stopwatch = time(NULL) - stopwatch;
         cout << "1 : " << stopwatch / 464.0 << endl;
         cout << "insert calls : " << quadtreenode::counter / 464.0 << endl;
         cout << "bucket overflows : " << quadtreenode::overflowcounter / 464.0 << endl;
         quadtreenode::counter = 0;
         quadtreenode::overflowcounter = 0;

         stopwatch = time(NULL);
         bob->load(loader2, 0, 5);
         stopwatch = time(NULL) - stopwatch;
         cout << "2 : " << stopwatch / 490.0 << endl;
         cout << "insert calls : " << quadtreenode::counter / 490.0 << endl;
         cout << "bucket overflows : " << quadtreenode::overflowcounter / 490.0 << endl;
         quadtreenode::counter = 0;
         quadtreenode::overflowcounter = 0;

         stopwatch = time(NULL);
         bob->load(loader3, 0, 5);
         stopwatch = time(NULL) - stopwatch;
         cout << "3 : " << stopwatch / 534.0 << endl;
         cout << "insert calls : " << quadtreenode::counter / 534.0 << endl;
         cout << "bucket overflows : " << quadtreenode::overflowcounter / 534.0 << endl;
         quadtreenode::counter = 0;
         quadtreenode::overflowcounter = 0;

         stopwatch = time(NULL);
         bob->load(loader4, 0, 5);
         stopwatch = time(NULL) - stopwatch;
         cout << "4 : " << stopwatch / 555.7 << endl;
         cout << "insert calls : " << quadtreenode::counter / 555.7 << endl;
         cout << "bucket overflows : " << quadtreenode::overflowcounter / 555.7 << endl;
         quadtreenode::counter = 0;
         quadtreenode::overflowcounter = 0;


//         stopwatch = time(NULL);

//         bob->load(loader5, 0, 5);

//         stopwatch = time(NULL) - stopwatch;
//         cout << "5 : " << stopwatch / 593.0 << endl;
//         cout << "insert calls : " << quadtreenode::counter / 593.0 << endl;
//         cout << "bucket overflows : " << quadtreenode::overflowcounter / 593.0 << endl;
//         quadtreenode::counter = 0;
//         quadtreenode::overflowcounter = 0;
//
//         stopwatch = time(NULL);

//         bob->load(loader6, 0, 5);

//         stopwatch = time(NULL) - stopwatch;
//         cout << "6 : " << stopwatch / 611.0 << endl;
//         cout << "insert calls : " << quadtreenode::counter / 611.0 << endl;
//         cout << "bucket overflows : " << quadtreenode::overflowcounter / 611.0 << endl;
//         quadtreenode::counter = 0;
//         quadtreenode::overflowcounter = 0;
//
//         stopwatch = time(NULL);

//         bob->load(loader7, 0, 5);

//         stopwatch = time(NULL) - stopwatch;
//         cout << "7 : " << stopwatch / 974.0 << endl;
//         cout << "insert calls : " << quadtreenode::counter / 974.0 << endl;
//         cout << "bucket overflows : " << quadtreenode::overflowcounter / 974.0 << endl;
//         quadtreenode::counter = 0;
//         quadtreenode::overflowcounter = 0;
//
//         stopwatch = time(NULL);

//         bob->load(loader8, 0, 5);

//         stopwatch = time(NULL) - stopwatch;
//         cout << "8 : " << stopwatch / 973.0 << endl;
//         cout << "insert calls : " << quadtreenode::counter / 973.0 << endl;
//         cout << "bucket overflows : " << quadtreenode::overflowcounter / 973.0 << endl;
//         quadtreenode::counter = 0;
//         quadtreenode::overflowcounter = 0;







//         stopwatch = time(NULL);
//         totaltime = stopwatch;
//         bob = new quadtree(l, 100000, 0, 25000000, 5);
//         stopwatch = time(NULL) - stopwatch;
//         cout << "1 : " << stopwatch / 96.0 << endl;
//         cout << "insert calls : " << quadtreenode::counter / 96.0 << endl;
//         cout << "bucket overflows : " << quadtreenode::overflowcounter / 96.0 << endl;
//         quadtreenode::counter = 0;
//         quadtreenode::overflowcounter = 0;
//
//         stopwatch = time(NULL);

//         bob->load(l2, 0, 5);

//         stopwatch = time(NULL) - stopwatch;
//         cout << "2 : " << stopwatch / 119.0 << endl;
//         cout << "insert calls : " << quadtreenode::counter / 119.0 << endl;
//         cout << "bucket overflows : " << quadtreenode::overflowcounter / 119.0 << endl;
//         quadtreenode::counter = 0;
//         quadtreenode::overflowcounter = 0;
//
//         stopwatch = time(NULL);

//         bob->load(l3, 0, 5);

//         stopwatch = time(NULL) - stopwatch;
//         cout << "3 : " << stopwatch / 89.0 << endl;
//         cout << "insert calls : " << quadtreenode::counter / 89.0 << endl;
//         cout << "bucket overflows : " << quadtreenode::overflowcounter / 89.0 << endl;
//         quadtreenode::counter = 0;
//         quadtreenode::overflowcounter = 0;
//
//         stopwatch = time(NULL);

//         bob->load(l4, 0, 5);

//         stopwatch = time(NULL) - stopwatch;
//         cout << "4 : " << stopwatch / 107.0 << endl;
//         cout << "insert calls : " << quadtreenode::counter / 107.0 << endl;
//         cout << "bucket overflows : " << quadtreenode::overflowcounter / 107.0 << endl;
//         quadtreenode::counter = 0;
//         quadtreenode::overflowcounter = 0;
//
//
//         stopwatch = time(NULL);

//         bob->load(l5, 0, 5);

//         stopwatch = time(NULL) - stopwatch;
//         cout << "5 : " << stopwatch / 125.0 << endl;
//         cout << "insert calls : " << quadtreenode::counter / 125.0 << endl;
//         cout << "bucket overflows : " << quadtreenode::overflowcounter / 125.0 << endl;
//         quadtreenode::counter = 0;
//         quadtreenode::overflowcounter = 0;
//
//         stopwatch = time(NULL);

//         bob->load(l6, 0, 5);

//         stopwatch = time(NULL) - stopwatch;
//         cout << "6 : " << stopwatch / 93.0 << endl;
//         cout << "insert calls : " << quadtreenode::counter / 93.0 << endl;
//         cout << "bucket overflows : " << quadtreenode::overflowcounter / 93.0 << endl;
//         quadtreenode::counter = 0;
//         quadtreenode::overflowcounter = 0;
//

         bob->MCP->clearcache();
         cout << "total time : " << time(NULL) - totaltime << endl;
         cout << "read in : " << (pointbucket::i_counter/1024)/1024 << endl;
         cout << "wrote out : " << (pointbucket::o_counter/1024)/1024 << endl;

      }
      /*catch (descriptiveexception e)
      {
         cout << e.why() << endl;
      }
      cout << "MAGIC!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
*/

   
   // sleep(15);

   // delete(bob);
   // sleep(5);

   //   try
   //    {
   //       LASloader *l2 = new LASloader("/home/scratch/LDR090601_112524_1.LAS");
   //bob->load(l2, 0);
   //      bob = new quadtree(l2, 50000, 0, 1000000000);
   //   }
   //   catch (const char* c)
   //   {
   //      cout << c << endl;
   //   }

   //  sleep(5);
   //   try
   //   {
   //     cout << bob->getfilename(1);
   //  }
   //   catch (const char* c)
   //   {
   //      cout << c << endl;
   //   }


   
//       point temp;
//       temp.x = 1;
//       temp.y = 1;
//       temp.z = 1.5;
//       bob->insert(temp);
//
//       temp.x = 2;
//       temp.y = 2;
//       temp.z = 4.5;
//       bob->insert(temp);
//       temp.x = 3;
//       temp.y = 4;
//       temp.z = 8.5;
//       bob->insert(temp);
//       temp.x = 5;
//       temp.y = 3;
//       temp.z = 2.4;
//       bob->insert(temp);
//       temp.x = 9;
//       temp.y = 2;
//       temp.z = 9.6;
//       bob->insert(temp);
//       temp.x = 4;
//       temp.y = 8;
//       temp.z = 3.8;
//       bob->insert(temp);
//       temp.x = 7;
//       temp.y = 3;
//       temp.z = 6.7;
//       bob->insert(temp);
//       temp.x = 5;
//       temp.y = 1;
//       temp.z = 5.9;
//       bob->insert(temp);
//       temp.x = 1;
//       temp.y = 3;
//       temp.z = 7.7;
//       bob->insert(temp);

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


   Glib::Thread* thready;
   boost::thread *threadzilla;

   cout << "construction complete" << endl;

   //sleep(1);

   // cout << "sorting by height" << endl;
   // bob->sort('H');
   //cout << "sorted" << endl;
   // cout << " clearing cache " << endl;
   // bob->subset(0, 0, 1, 1);


   cout << "subseting" << endl;
   boundary* z = bob->getboundary();

   // delete bob;
   // l = new LASloader("/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_122647_1.LAS");
   // bob = new quadtree(l, 100000, 0, z->minX, z->maxY, z->maxX, z->minY, 4800, 20000000);

   //z = bob->getboundary();

  // LASsaver *sally = new LASsaver("/tmp/lidar/test.LAS", "/tmp/lidar/test.LAS");
  // bob->saveflightline(0, sally);



   Xs[0] = Xs[1] = z->minX;
   Xs[2] = Xs[3] = z->maxX;
   Ys[0] = Ys[3] = z->minY;
   Ys[1] = Ys[2] = z->maxY;
   vector<pointbucket*> *wibble = bob->advsubset(Xs, Ys, 4);

   cout << z->minX << "  " << z->minY << "  " << z->maxX << "  " << z->maxY << endl;

   cout << "subset complete, " << wibble->size() << " buckets" << endl;
   //    sleep(10);
   int counter = 0;

   for (int i = 0; i < wibble->size(); i++)
   {

      counter += wibble->at(i)->getnumberofpoints(0);

   }
   cout << "number of points : " << counter << endl;

   
   stopwatch = time(NULL);
   for(int k=0; k<wibble->size(); k++)
   {
      wibble->at(k)->getpoint(0, 0);
   }
   stopwatch = time(NULL) - stopwatch;
   cout << "to load every point at res 0 takes " << stopwatch << endl;
   bob->MCP->clearcache();

   stopwatch = time(NULL);
   for(int k=0; k<wibble->size(); k++)
   {
      wibble->at(k)->getpoint(0, 1);
   }
   stopwatch = time(NULL) - stopwatch;
   cout << "to load every point at res 1  takes " << stopwatch << endl;
   bob->MCP->clearcache();

   stopwatch = time(NULL);
   for(int k=0; k<wibble->size(); k++)
   {
      wibble->at(k)->getpoint(0, 2);
   }
   stopwatch = time(NULL) - stopwatch;
   cout << "to load every point at res 2  takes " << stopwatch << endl;
   bob->MCP->clearcache();


   stopwatch = time(NULL);
   for(int k=0; k<wibble->size(); k++)
   {
      wibble->at(k)->getpoint(0, 3);
   }
   stopwatch = time(NULL) - stopwatch;
   cout << "to load every point at res 3  takes " << stopwatch << endl;
   bob->MCP->clearcache();


   


   numberbuckets = wibble->size();
   buckets = new pointbucket*[wibble->size()];


   //    for (int i = 0; i < wibble->size(); i++)
   //    {
   //       for(int k=0; k<wibble->at(i)->getnumberofpoints(); k++)
   //       {
   //          wibble->at(i)->setclassification(k, k%12);
   //       }
   //    }

   //    LASsaver *sally = new LASsaver("/users/rsg/chrfi/scratch/oh_i_hope_this_works2.LAS", "/users/rsg/arsf/workspace/GB07_07-2009_277_Inverclyde/leica/proc_laser/LDR091004_122647_1.las");
   //    bob->saveflightline(0, sally);
   //    delete sally;
   // threadzilla = new boost::thread(testfunc);
   //threadzilla->join();
   //delete threadzilla;
   //cout << "one down one to go" << endl;

   //malloc_stats();

   //threadzilla = new boost::thread(testfunc);
   // threadzilla->join();

   //Glib::thread_init();
   //thready = Glib::Thread::create(sigc::ptr_fun(testfunc), true);
   //thready->join();


   //cout << "ive just deleted thready" << endl;
   //sleep(10);
   //testfunc();

   //delete[] buckets;

   //wibble = bob->advsubset(0, 0, 4, 1, 5);

   // sleep(20);


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




