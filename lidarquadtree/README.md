lidarquadtree library
=====================

This file contains the following sections:  

GENERAL NOTES  
INSTALLATION  
LICENSE  

GENERAL NOTES
=============

Lidarquadtree is a library initially created for LiDAR Analysis GUI (LAG).  
It provides an efficient data structure for storing and indexing points.  

INSTALLATION
============

Dependencies
------------

A few libraries are required to make this library compile.
   * boost
   * liblzo2
   * laslib

Compiling on GNU/Linux x86
--------------------------

This codebase uses the GNU packaging tools.  To get things to a sane state
after checkout, do:  

    libtoolize && autoheader && aclocal && automake --add-missing && autoconf

Then you can do the normal:

    ./configure  
    make  
    make install  

Use make clean if you want a fresh rebuild. The resulting .so library will be put in .libs
and installed in (prefix)/lib/lidar/quadtree.  

Compiling on win32
------------------

Hopefully in the future. :-)

Documentation
-------------

To make the docs, type:

    doxygen doxygen_settings

Which will create html documentation in doc/html.

LICENSE
=======

See COPYING for the GNU GENERAL PUBLIC LICENSE.
