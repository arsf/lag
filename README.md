LAG - LiDAR Analysis GUI
========================

This file contains the following sections:

GENERAL NOTES
INSTALLATION
LICENSE

GENERAL NOTES
=============
LAG is a software for visualisation, analysis and processing of LiDAR data. It has
been initially created at Airborne Reasearch and Survey faciliy in Plymouth Marine
Laboratory to help with data processing.

For more information see: [website?]
For user's guide see: http://arsf-dan.nerc.ac.uk/trac/wiki/Processing/laguserguide
If you are a developer see: http://arsf-dan.nerc.ac.uk/trac/wiki/Processing/lagdevelopersfaq

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

LICENSE
=======

LAG is licensed under the GNU General Public License, version 2. 
Read the file COPYING in the source distribution for details.
