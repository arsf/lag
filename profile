Flat profile:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 91.21     44.94    44.94 78421708     0.00     0.00  quadtreenode::insert(point)
  4.67     47.24     2.30     4669     0.00     0.00  quadtreenode::quadtreenode(double, double, double, double, int)
  2.15     48.30     1.06       12     0.09     4.11  lidar_loader::load(char const*, quadtreenode**, int, int)
  0.91     48.75     0.45 78421707     0.00     0.00  liblas::LASPoint::GetX() const
  0.65     49.07     0.32 78421707     0.00     0.00  liblas::LASPoint::GetZ() const
  0.44     49.29     0.22 78421707     0.00     0.00  liblas::LASPoint::GetY() const
  0.07     49.32     0.04       12     0.00     0.00  std::operator|(std::_Ios_Openmode, std::_Ios_Openmode)
  0.00     49.32     0.00       12     0.00     0.00  lidar_loader::lidar_loader()
  0.00     49.32     0.00       12     0.00     0.00  qloader::qloader()
  0.00     49.32     0.00       12     0.00     4.11  quadtree::load(char const*, int)
  0.00     49.32     0.00        1     0.00     0.00  global constructors keyed to _ZN12lidar_loader4loadEPKcPP12quadtreenodeii
  0.00     49.32     0.00        1     0.00     0.00  global constructors keyed to _ZN12quadtreenodeC2Eddddi
  0.00     49.32     0.00        1     0.00     0.00  global constructors keyed to _ZN8quadtreeC2EPKcii
  0.00     49.32     0.00        1     0.00     0.00  global constructors keyed to main
  0.00     49.32     0.00        1     0.00     0.00  __static_initialization_and_destruction_0(int, int)
  0.00     49.32     0.00        1     0.00     0.00  __static_initialization_and_destruction_0(int, int)
  0.00     49.32     0.00        1     0.00     0.00  __static_initialization_and_destruction_0(int, int)
  0.00     49.32     0.00        1     0.00     0.00  __static_initialization_and_destruction_0(int, int)
  0.00     49.32     0.00        1     0.00     0.00  quadtreenode::~quadtreenode()
  0.00     49.32     0.00        1     0.00     0.00  quadtree::insert(point)
  0.00     49.32     0.00        1     0.00     0.00  quadtree::quadtree(double, double, double, double, int)
  0.00     49.32     0.00        1     0.00     0.00  quadtree::~quadtree()

 %         the percentage of the total running time of the
time       program used by this function.

cumulative a running sum of the number of seconds accounted
 seconds   for by this function and those listed above it.

 self      the number of seconds accounted for by this
seconds    function alone.  This is the major sort for this
           listing.

calls      the number of times this function was invoked, if
           this function is profiled, else blank.
 
 self      the average number of milliseconds spent in this
ms/call    function per call, if this function is profiled,
	   else blank.

 total     the average number of milliseconds spent in this
ms/call    function and its descendents per call, if this 
	   function is profiled, else blank.

name       the name of the function.  This is the minor sort
           for this listing. The index shows the location of
	   the function in the gprof listing. If the index is
	   in parenthesis it shows where it would appear in
	   the gprof listing if it were to be printed.

		     Call graph (explanation follows)


granularity: each sample hit covers 2 byte(s) for 0.02% of 49.32 seconds

index % time    self  children    called     name
                                                 <spontaneous>
[1]    100.0    0.00   49.32                 main [1]
                0.00   49.32      12/12          quadtree::load(char const*, int) [3]
                0.00    0.00       1/1           quadtree::quadtree(double, double, double, double, int) [10]
                0.00    0.00       1/1           quadtree::insert(point) [11]
                0.00    0.00       1/1           quadtree::~quadtree() [29]
-----------------------------------------------
                1.06   48.26      12/12          quadtree::load(char const*, int) [3]
[2]    100.0    1.06   48.26      12         lidar_loader::load(char const*, quadtreenode**, int, int) [2]
               44.94    2.30 78421707/78421708     quadtreenode::insert(point) [4]
                0.45    0.00 78421707/78421707     liblas::LASPoint::GetX() const [6]
                0.32    0.00 78421707/78421707     liblas::LASPoint::GetZ() const [7]
                0.22    0.00 78421707/78421707     liblas::LASPoint::GetY() const [8]
                0.04    0.00      12/12          std::operator|(std::_Ios_Openmode, std::_Ios_Openmode) [9]
-----------------------------------------------
                0.00   49.32      12/12          main [1]
[3]    100.0    0.00   49.32      12         quadtree::load(char const*, int) [3]
                1.06   48.26      12/12          lidar_loader::load(char const*, quadtreenode**, int, int) [2]
                0.00    0.00      12/12          lidar_loader::lidar_loader() [18]
-----------------------------------------------
                             2785513012             quadtreenode::insert(point) [4]
                0.00    0.00       1/78421708     quadtree::insert(point) [11]
               44.94    2.30 78421707/78421708     lidar_loader::load(char const*, quadtreenode**, int, int) [2]
[4]     95.8   44.94    2.30 78421708+2785513012 quadtreenode::insert(point) [4]
                2.30    0.00    4668/4669        quadtreenode::quadtreenode(double, double, double, double, int) [5]
                             2785513012             quadtreenode::insert(point) [4]
-----------------------------------------------
                0.00    0.00       1/4669        quadtree::quadtree(double, double, double, double, int) [10]
                2.30    0.00    4668/4669        quadtreenode::insert(point) [4]
[5]      4.7    2.30    0.00    4669         quadtreenode::quadtreenode(double, double, double, double, int) [5]
-----------------------------------------------
                0.45    0.00 78421707/78421707     lidar_loader::load(char const*, quadtreenode**, int, int) [2]
[6]      0.9    0.45    0.00 78421707         liblas::LASPoint::GetX() const [6]
-----------------------------------------------
                0.32    0.00 78421707/78421707     lidar_loader::load(char const*, quadtreenode**, int, int) [2]
[7]      0.6    0.32    0.00 78421707         liblas::LASPoint::GetZ() const [7]
-----------------------------------------------
                0.22    0.00 78421707/78421707     lidar_loader::load(char const*, quadtreenode**, int, int) [2]
[8]      0.4    0.22    0.00 78421707         liblas::LASPoint::GetY() const [8]
-----------------------------------------------
                0.04    0.00      12/12          lidar_loader::load(char const*, quadtreenode**, int, int) [2]
[9]      0.1    0.04    0.00      12         std::operator|(std::_Ios_Openmode, std::_Ios_Openmode) [9]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[10]     0.0    0.00    0.00       1         quadtree::quadtree(double, double, double, double, int) [10]
                0.00    0.00       1/4669        quadtreenode::quadtreenode(double, double, double, double, int) [5]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[11]     0.0    0.00    0.00       1         quadtree::insert(point) [11]
                0.00    0.00       1/78421708     quadtreenode::insert(point) [4]
-----------------------------------------------
                0.00    0.00      12/12          quadtree::load(char const*, int) [3]
[18]     0.0    0.00    0.00      12         lidar_loader::lidar_loader() [18]
                0.00    0.00      12/12          qloader::qloader() [19]
-----------------------------------------------
                0.00    0.00      12/12          lidar_loader::lidar_loader() [18]
[19]     0.0    0.00    0.00      12         qloader::qloader() [19]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [40]
[20]     0.0    0.00    0.00       1         global constructors keyed to _ZN12lidar_loader4loadEPKcPP12quadtreenodeii [20]
                0.00    0.00       1/1           __static_initialization_and_destruction_0(int, int) [25]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [40]
[21]     0.0    0.00    0.00       1         global constructors keyed to _ZN12quadtreenodeC2Eddddi [21]
                0.00    0.00       1/1           __static_initialization_and_destruction_0(int, int) [24]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [40]
[22]     0.0    0.00    0.00       1         global constructors keyed to _ZN8quadtreeC2EPKcii [22]
                0.00    0.00       1/1           __static_initialization_and_destruction_0(int, int) [26]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [40]
[23]     0.0    0.00    0.00       1         global constructors keyed to main [23]
                0.00    0.00       1/1           __static_initialization_and_destruction_0(int, int) [27]
-----------------------------------------------
                0.00    0.00       1/1           global constructors keyed to _ZN12quadtreenodeC2Eddddi [21]
[24]     0.0    0.00    0.00       1         __static_initialization_and_destruction_0(int, int) [24]
-----------------------------------------------
                0.00    0.00       1/1           global constructors keyed to _ZN12lidar_loader4loadEPKcPP12quadtreenodeii [20]
[25]     0.0    0.00    0.00       1         __static_initialization_and_destruction_0(int, int) [25]
-----------------------------------------------
                0.00    0.00       1/1           global constructors keyed to _ZN8quadtreeC2EPKcii [22]
[26]     0.0    0.00    0.00       1         __static_initialization_and_destruction_0(int, int) [26]
-----------------------------------------------
                0.00    0.00       1/1           global constructors keyed to main [23]
[27]     0.0    0.00    0.00       1         __static_initialization_and_destruction_0(int, int) [27]
-----------------------------------------------
                                4668             quadtreenode::~quadtreenode() [28]
                0.00    0.00       1/1           quadtree::~quadtree() [29]
[28]     0.0    0.00    0.00       1+4668    quadtreenode::~quadtreenode() [28]
                                4668             quadtreenode::~quadtreenode() [28]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[29]     0.0    0.00    0.00       1         quadtree::~quadtree() [29]
                0.00    0.00       1/1           quadtreenode::~quadtreenode() [28]
-----------------------------------------------

 This table describes the call tree of the program, and was sorted by
 the total amount of time spent in each function and its children.

 Each entry in this table consists of several lines.  The line with the
 index number at the left hand margin lists the current function.
 The lines above it list the functions that called this function,
 and the lines below it list the functions this one called.
 This line lists:
     index	A unique number given to each element of the table.
		Index numbers are sorted numerically.
		The index number is printed next to every function name so
		it is easier to look up where the function in the table.

     % time	This is the percentage of the `total' time that was spent
		in this function and its children.  Note that due to
		different viewpoints, functions excluded by options, etc,
		these numbers will NOT add up to 100%.

     self	This is the total amount of time spent in this function.

     children	This is the total amount of time propagated into this
		function by its children.

     called	This is the number of times the function was called.
		If the function called itself recursively, the number
		only includes non-recursive calls, and is followed by
		a `+' and the number of recursive calls.

     name	The name of the current function.  The index number is
		printed after it.  If the function is a member of a
		cycle, the cycle number is printed between the
		function's name and the index number.


 For the function's parents, the fields have the following meanings:

     self	This is the amount of time that was propagated directly
		from the function into this parent.

     children	This is the amount of time that was propagated from
		the function's children into this parent.

     called	This is the number of times this parent called the
		function `/' the total number of times the function
		was called.  Recursive calls to the function are not
		included in the number after the `/'.

     name	This is the name of the parent.  The parent's index
		number is printed after it.  If the parent is a
		member of a cycle, the cycle number is printed between
		the name and the index number.

 If the parents of the function cannot be determined, the word
 `<spontaneous>' is printed in the `name' field, and all the other
 fields are blank.

 For the function's children, the fields have the following meanings:

     self	This is the amount of time that was propagated directly
		from the child into the function.

     children	This is the amount of time that was propagated from the
		child's children to the function.

     called	This is the number of times the function called
		this child `/' the total number of times the child
		was called.  Recursive calls by the child are not
		listed in the number after the `/'.

     name	This is the name of the child.  The child's index
		number is printed after it.  If the child is a
		member of a cycle, the cycle number is printed
		between the name and the index number.

 If there are any cycles (circles) in the call graph, there is an
 entry for the cycle-as-a-whole.  This entry shows who called the
 cycle (as parents) and the members of the cycle (as children.)
 The `+' recursive calls entry shows the number of function calls that
 were internal to the cycle, and the calls entry for each member shows,
 for that member, how many times it was called from other members of
 the cycle.


Index by function name

  [20] global constructors keyed to _ZN12lidar_loader4loadEPKcPP12quadtreenodeii (lidar_loader.cpp) [2] lidar_loader::load(char const*, quadtreenode**, int, int) [10] quadtree::quadtree(double, double, double, double, int)
  [21] global constructors keyed to _ZN12quadtreenodeC2Eddddi (quadtreenode.cpp) [18] lidar_loader::lidar_loader() [29] quadtree::~quadtree()
  [22] global constructors keyed to _ZN8quadtreeC2EPKcii (quadtree.cpp) [4] quadtreenode::insert(point) [6] liblas::LASPoint::GetX() const
  [23] global constructors keyed to main (main.cpp) [5] quadtreenode::quadtreenode(double, double, double, double, int) [8] liblas::LASPoint::GetY() const
  [24] __static_initialization_and_destruction_0(int, int) (quadtreenode.cpp) [28] quadtreenode::~quadtreenode() [7] liblas::LASPoint::GetZ() const
  [25] __static_initialization_and_destruction_0(int, int) (lidar_loader.cpp) [19] qloader::qloader() [9] std::operator|(std::_Ios_Openmode, std::_Ios_Openmode)
  [26] __static_initialization_and_destruction_0(int, int) (quadtree.cpp) [3] quadtree::load(char const*, int)
  [27] __static_initialization_and_destruction_0(int, int) (main.cpp) [11] quadtree::insert(point)
