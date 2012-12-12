This README file is here to indicate that this version of LASlib has been patched with unofficial modifications to its Makefiles. These modifications fall under the same licensing conditions as the rest of the library, and are in no way intended to fork or discredit LASlib.

Building and installing LASlib in this way is required for other software to be built on LASlib and subsequently distributed to users who do not have the time or knowledge to compile and install LASlib on the level of compiling and linking individual files

The changes are as follows:
* LASlib now builds dynamically linkable libraries aswell as the static ones
* Libraries are now named liblaslib, to avoid conflict with the libLAS library on linux systems which need to support both (-llas refers to libLAS, -llaslib refers to this (LASlib))
* Versions of the library are notated by date, in the form YYYYMMDD, as LASlib has no official versioning system
* make now supports an install command for the laslib subdirectory, installing LASlib to a location specified in the Makefile, /usr/local by default.

For queries, please contact: arsf-processing@pml.ac.uk
