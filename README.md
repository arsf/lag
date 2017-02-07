LAG - LiDAR Analysis GUI
========================

This file contains the following sections:

GENERAL NOTES
INSTALLATION
LICENSE

GENERAL NOTES
=============
LAG is a software for visualisation, analysis and processing of LiDAR data. It has
been initially created at NERC Airborne Reasearch Faciliy Data Analysis Node (NERC-ARF-DAN) in Plymouth Marine
Laboratory to help with data processing.

For user's guide see: https://nerc-arf-dan.pml.ac.uk/trac/wiki/Processing/laguserguide

If you are a developer see: https://nerc-arf-dan.pml.ac.uk/trac/wiki/Processing/lagdevelopersfaq

INSTALLATION
============

Required Dependencies
---------------------

To be able to build LAG you will need a compiled version of lidarquadtree and
you may need to modify Makefile.am to point to its location.

The following libraries are required to make LAG compile.
   * boost
   * GTKmm
   * GTKGLextmm
   * GThread
   * libgeotiff

   * laslib
   * lidarquadtree

laslib
------

LASlib, courtesy of Martin Isenburg, is a component of LAStools used in LAG for
handling LAS reading and writing. However, the version provided on the LAStools
website (http://www.cs.unc.edu/~isenburg/lastools/) is not made or intended for
use as a standard linux library. This means that every program intending to use
it must bundle the entire library with every distribution.

In keeping with a more standard linux/gnu build+release process, a patched
version of LAStools, forked from the main version, is available from:

https://github.com/arsf/LAStools

This patch changes only the Makefiles used to build LASlib, to support both 
static and dynamic linking, and an installation procedure which allows other 
programs to utilise laslib as though it were a standard linux library.

This also renames "liblas" to "liblaslib" to be more distinct from the
(unsupported) libLAS library. This should make it possible to use libLAS and
laslib alongside each other on the same system, but as a side effect, also
breaks backwards compatibility with any existing laslib programs.

lidarquadtree
-------------

Lidarquadtree is a library originally built and used for LAG. It features disk
caching of points to allow it to load potentially very huge datasets and still
support spatial indexing, meaning it can be using for LAS processing.

It will need to be built and installed before LAG can be compiled.

Ubuntu Linux Users
------------------

Ubuntu linux users will find, and other linux users dependent on distribution,
may find that ubuntu fails to compile towards the end of the process, with a
lot of messages including "undefined reference to" about halfway through.

This has been seen in Ubuntu 11.10 and 12.04.

If this happens, you have two options:
1) Install the binutils-gold package before compiling
2) After ./configure and before running make, apply the provided patch:
   patch Makefile < ubuntu-Makefile.patch

Compiling on GNU/Linux x86
--------------------------

This codebase uses the GNU packaging tools.  To get things to a sane state
after checkout, do:

libtoolize && autoheader && aclocal && automake --add-missing && autoconf

Depending on where you've installed laslib, you may need to do the following 
to allow configure to find it:

export PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig/

Then you can do the normal:

./configure
make
make install

Compiling on win32
------------------

Hopefully in the future. :-)


INSTALLATION WITH CMAKE (ALPHA)
===============================

Required Dependencies
---------------------

To be able to build LAG the following libraries are required to be installed.
   * GTKmm
   * GTKGLextmm
   * GThread
   * lzo2

you basiccaly have to do :
   $ sudo apt-get install libglib2.0-dev libgtkmm-2.4-dev libgtkglextmm-x11-1.2-dev liblzo2-dev

Installing laslib
--------------------

Go to http://www.cs.unc.edu/~isenburg/lastools/ and download lastools. If 
you want that lag find automatically lastools, install lastools next to lag source directory.

Then compile laslib and rename liblas.a to liblaslib.a. You can do normal :

    cd lastools/laslib
    make
    cp lastools/laslib/liblas.a lastools/laslib/liblaslib.a


Compiling with CMake
--------------------------

use cmake-gui or ccmake.

with ccmake do :

    cd lag
    mkdir build_cmake
    cd build_cmake
    ccmake ../ -D LASTOOLS_ROOT="/dir/to/lastools/"
    make

lag is then installing into build_cmake/bin


LICENSE
=======

LAG is licensed under the GNU General Public License, version 2. 
Read the file COPYING in the source distribution for details.
