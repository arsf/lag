****************************************************************

  lasnoise:

  This tool flags or removes noise points in LAS/LAZ/BIN/ASCII
  files. The tool looks for isolated points according to certain
  criteria that can be modified via '-step 3' and '-isolated 3'
  as needed. The default for step is 4 and for isolated is 5. It
  is possible to specify the xy and the z size of the 27 cells
  separately with '-step_xy 5' and '-step_z 10' which would create
  cells of size 5 by 5 by 10 units.

  The tool tries to find points that have only few other points
  in their surrounding 3 by 3 by 3 grid of cells with the cell
  the respective point falls into being in the center. The size
  of each of the 27 cells is set with the '-step 5' parameter.
  The maximal number of few other points in the 3 by 3 by 3 grid
  still designating a point as isolated is set with '-isolated 6'.

  By default the noise points are given the classification code
  7 (low or high noise). Using the '-remove_noise' flag will
  instead remove them from the output file. Alternatively with
  the '-classify_as 31' switch a different classification code
  can be selected.

  It is also important to tell the tool whether the horizontal
  and vertical units are meters (which is assumed by default)
  or '-feet' or '-elevation_feet'. Should the LAS file contain
  proper projection VLRs then there is no need to specify this
  explicitly. Internally the step size in xy or in z direction
  will be multiplied with 3 if the coordinates are found or are
  specified to be in feet. The size of the grid cells needs to
  be changed with '-step 0.0001' or similar if the input is in
  geographic longlat representation.

  You can avoid having certain points being classified with the
  '-ignore_class 2' option.

  Please license from martin@rapidlasso.com before using lasnoise
  commercially. Please note that the unlicensed version will set
  intensity, gps_time, user data, and point source ID to zero,
  slightly change the LAS point order, and randomly add a tiny
  bit of white noise to the points coordinates once a certain
  number of points in the input file is exceeded.

  For updates check the website or join the LAStools mailing list.

  http://rapidlasso.com/
  http://lastools.org/
  http://groups.google.com/group/lastools/
  http://twitter.com/lastools/
  http://facebook.com/lastools/
  http://linkedin.com/groups?gid=4408378

  Martin @rapidlasso

****************************************************************

example usage:

>> lasnoise -i tiles\*.laz ^
            -step 2 -isolated 3 ^
            -odix _denoised -olaz ^
            -cores 7

classifies all points that have only 3 or fewer other points in
their surrounding 3 by 3 by 3 grid (with the respective point
in the center cell) where each cell is 2 by 2 by 2 meters in size
as classification code 7.

>> lasnoise -i lidar.las ^
            -remove_noise ^
            -ignore_class 2 ^
            -o lidar_without_noise.laz

removes all points - except those classified as 2 (ground) - that
have only 5 or fewer other points in their surrounding 3 by 3 by 3
grid (with the respective point in the center cell) where each cell
is 4 by 4 by 4 meters in size.

>> lasnoise -i tiles_raw\*.laz ^
            -classify_as 31 ^
            -step 2 - isolated 2 ^
            -odir tiles_denoised\ -olaz ^
            -cores 3

running on whole folder of files on 3 cores where each cell being
only 2 by 2 by 2 meters in size and where points that have only
two or fewer other points in the 27 cell neighborhood of their
surrounding 3 by 3 by 3 grid are considered noise. here points
are classified to class 31 instead of being removed.

for more info:

D:\lastools\bin>lasnoise -h
Filter points based on their coordinates.
  -clip_tile 631000 4834000 1000 (ll_x ll_y size)
  -clip_circle 630250.00 4834750.00 100 (x y radius)
  -clip_box 620000 4830000 100 621000 4831000 200 (min_x min_y min_z max_x max_y max_z)
  -clip 630000 4834000 631000 4836000 (min_x min_y max_x max_y)
  -clip_x_below 630000.50 (min_x)
  -clip_y_below 4834500.25 (min_y)
  -clip_x_above 630500.50 (max_x)
  -clip_y_above 4836000.75 (max_y)
  -clip_z 11.125 130.725 (min_z max_z)
  -clip_z_below 11.125 (min_z)
  -clip_z_above 130.725 (max_z)
  -clip_z_between 11.125 130.725 (min_z max_z)
Filter points based on their return number.
  -first_only -keep_first -drop_first
  -last_only -keep_last -drop_last
  -keep_middle -drop_middle
  -keep_return 1 2 3
  -drop_return 3 4
  -keep_single -drop_single
  -keep_double -drop_double
  -keep_triple -drop_triple
  -keep_quadruple -drop_quadruple
  -keep_quintuple -drop_quintuple
Filter points based on the scanline flags.
  -drop_scan_direction 0
  -scan_direction_change_only
  -edge_of_flight_line_only
Filter points based on their intensity.
  -keep_intensity 20 380
  -drop_intensity_below 20
  -drop_intensity_above 380
  -drop_intensity_between 4000 5000
Filter points based on their classification.
  -keep_class 1 3 7
  -drop_class 4 2
  -drop_synthetic -keep_synthetic
  -drop_keypoint -keep_keypoint
  -drop_withheld -keep_withheld
Filter points based on their user data.
  -keep_user_data 1
  -drop_user_data 255
  -keep_user_data_between 10 20
  -drop_user_data_below 1
  -drop_user_data_above 100
  -drop_user_data_between 10 40
Filter points based on their point source ID.
  -keep_point_source 3
  -keep_point_source_between 2 6
  -drop_point_source 27
  -drop_point_source_below 6
  -drop_point_source_above 15
  -drop_point_source_between 17 21
Filter points based on their scan angle.
  -keep_scan_angle -15 15
  -drop_abs_scan_angle_above 15
  -drop_scan_angle_below -15
  -drop_scan_angle_above 15
  -drop_scan_angle_between -25 -23
Filter points based on their gps time.
  -keep_gps_time 11.125 130.725
  -drop_gps_time_below 11.125
  -drop_gps_time_above 130.725
  -drop_gps_time_between 22.0 48.0
Filter points based on their wavepacket.
  -keep_wavepacket 1 2
  -drop_wavepacket 0
Filter points with simple thinning.
  -keep_every_nth 2
  -keep_random_fraction 0.1
  -thin_with_grid 1.0
Transform coordinates.
  -translate_x -2.5
  -scale_z 0.3048
  -rotate_xy 15.0 620000 4100000 (angle + origin)
  -translate_xyz 0.5 0.5 0
  -translate_then_scale_y -0.5 1.001
  -clamp_z_below 70.5
  -clamp_z 70.5 72.5
Transform raw xyz integers.
  -translate_raw_z 20
  -translate_raw_xyz 1 1 0
  -clamp_raw_z 500 800
Transform intensity.
  -scale_intensity 2.5
  -translate_intensity 50
  -translate_then_scale_intensity 0.5 3.1
Transform scan_angle.
  -scale_scan_angle 1.944445
  -translate_scan_angle -5
  -translate_then_scale_scan_angle -0.5 2.1
Change the return number or return count of points.
  -repair_zero_returns
  -set_return_number 1
  -change_return_number_from_to 2 1
  -set_number_of_returns 2
  -change_number_of_returns_from_to 0 2
Modify the classification.
  -set_classification 2
  -change_classification_from_to 2 4
  -classify_z_below_as -5.0 7
  -classify_z_above_as 70.0 7
Modify the user data.
  -set_user_data 0
  -change_user_data_from_to 23 26
Modify the point source ID.
  -set_point_source 500
  -change_point_source_from_to 1023 1024
Transform gps_time.
  -translate_gps_time 40.50
  -adjusted_to_week
  -week_to_adjusted 1671
Transform RGB colors.
  -scale_rgb_down (by 256)
  -scale_rgb_up (by 256)
Supported LAS Inputs
  -i lidar.las
  -i lidar.laz
  -i lidar1.las lidar2.las lidar3.las -merged
  -i *.las - merged
  -i flight0??.laz flight1??.laz
  -i terrasolid.bin
  -i esri.shp
  -i nasa.qi
  -i lidar.txt -iparse xyzti -iskip 2 (on-the-fly from ASCII)
  -i lidar.txt -iparse xyzi -itranslate_intensity 1024
  -lof file_list.txt
  -stdin (pipe from stdin)
  -rescale 0.01 0.01 0.001
  -rescale_xy 0.01 0.01
  -rescale_z 0.01
  -reoffset 600000 4000000 0
Supported LAS Outputs
  -o lidar.las
  -o lidar.laz
  -o xyzta.txt -oparse xyzta (on-the-fly to ASCII)
  -o terrasolid.bin
  -o nasa.qi
  -odir C:\data\ground (specify output directory)
  -odix _classified (specify file name appendix)
  -ocut 2 (cut the last two characters from name)
  -olas -olaz -otxt -obin -oqfit (specify format)
  -stdout (pipe to stdout)
  -nil    (pipe to NULL)
LAStools (by martin@rapidlasso.com) version 130718 (academic)
usage:
lasnoise -i in.laz -o out.laz
lasnoise -i raw\*.laz -step 2 -isolated 3 -odir denoised\ -olaz
lasnoise -i *.laz -step 4 -isolated 5 -olaz -remove_noise
lasnoise -i tiles\*.laz -step 3 -isolated 3 -classify_as 31 -odir denoised\ -olaz
lasnoise -h

-------------
if you find bugs let me (martin@rapidlasso.com) know.
