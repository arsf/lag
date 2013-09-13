/*
===============================================================================

  FILE:  laszip_dll.h
  
  CONTENTS:
  
    A simple DLL interface to LASzip
  
  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2007-2013, martin isenburg, rapidlasso - tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    8 August 2013 -- added laszip_get_coordinates() and laszip_set_coordinates()
    6 August 2013 -- added laszip_auto_offset() and laszip_check_for_integer_overflow()
    31 July 2013 -- added laszip_get_point_count() for FUSION integration
    29 July 2013 -- reorganized to create an easy to use LASzip DLL 
  
===============================================================================
*/

#ifndef LASZIP_DLL_H
#define LASZIP_DLL_H

#ifdef _WIN32
#   ifdef LASZIP_DYN_LINK
#       ifdef LASZIP_SOURCE
#           define LASZIP_API __declspec(dllexport)
#       else
#           define LASZIP_API __declspec(dllimport)
#       endif
#   else
#       define LASZIP_API
#   endif
#else
#   define LASZIP_API
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/*---------------------------------------------------------------------------*/
/*--------------- DLL variables to pass data to/from LASzip -----------------*/
/*---------------------------------------------------------------------------*/

typedef int                laszip_BOOL;
typedef unsigned char      laszip_U8;
typedef unsigned short     laszip_U16;
typedef unsigned int       laszip_U32;
typedef unsigned __int64   laszip_U64;
typedef char               laszip_I8;
typedef short              laszip_I16;
typedef int                laszip_I32;
typedef __int64            laszip_I64;
typedef char               laszip_CHAR;
typedef float              laszip_F32;
typedef double             laszip_F64;
typedef void*              laszip_POINTER;

typedef struct laszip_geokey
{
  laszip_U16 key_id;
  laszip_U16 tiff_tag_location;
  laszip_U16 count;
  laszip_U16 value_offset;
} laszip_geokey_struct;

typedef struct laszip_vlr
{
  laszip_U16 reserved;
  laszip_CHAR user_id[16]; 
  laszip_U16 record_id;
  laszip_U16 record_length_after_header;
  laszip_CHAR description[32];
  laszip_U8* data;
} laszip_vlr_struct;

typedef struct laszip_header
{
  laszip_U16 file_source_ID;
  laszip_U16 global_encoding;
  laszip_U32 project_ID_GUID_data_1;
  laszip_U16 project_ID_GUID_data_2;
  laszip_U16 project_ID_GUID_data_3;
  laszip_CHAR project_ID_GUID_data_4[8];
  laszip_U8 version_major;
  laszip_U8 version_minor;
  laszip_CHAR system_identifier[32];
  laszip_CHAR generating_software[32];
  laszip_U16 file_creation_day;
  laszip_U16 file_creation_year;
  laszip_U16 header_size;
  laszip_U32 offset_to_point_data;
  laszip_U32 number_of_variable_length_records;
  laszip_U8 point_data_format;
  laszip_U16 point_data_record_length;
  laszip_U32 number_of_point_records;
  laszip_U32 number_of_points_by_return[5];
  laszip_F64 x_scale_factor;
  laszip_F64 y_scale_factor;
  laszip_F64 z_scale_factor;
  laszip_F64 x_offset;
  laszip_F64 y_offset;
  laszip_F64 z_offset;
  laszip_F64 max_x;
  laszip_F64 min_x;
  laszip_F64 max_y;
  laszip_F64 min_y;
  laszip_F64 max_z;
  laszip_F64 min_z;

  // LAS 1.3 and higher only
  laszip_U64 start_of_waveform_data_packet_record;

  // LAS 1.4 and higher only
  laszip_U64 start_of_first_extended_variable_length_record;
  laszip_U32 number_of_extended_variable_length_records;
  laszip_U64 extended_number_of_point_records;
  laszip_U64 extended_number_of_points_by_return[15];

  // optional
  laszip_U32 user_data_in_header_size;
  laszip_U8* user_data_in_header;

  // optional VLRs
  laszip_vlr_struct* vlrs;

  // optional
  laszip_U32 user_data_after_header_size;
  laszip_U8* user_data_after_header;

} laszip_header_struct;

typedef struct laszip_point
{
  laszip_I32 X;
  laszip_I32 Y;
  laszip_I32 Z;
  laszip_U16 intensity;
  laszip_U8 return_number : 3;
  laszip_U8 number_of_returns_of_given_pulse : 3;
  laszip_U8 scan_direction_flag : 1;
  laszip_U8 edge_of_flight_line : 1;
  laszip_U8 classification;
  laszip_I8 scan_angle_rank;
  laszip_U8 user_data;
  laszip_U16 point_source_ID;

  laszip_F64 gps_time;
  laszip_U16 rgb[4];
  laszip_U8 wave_packet[29];

  // LAS 1.4 only
  laszip_U8 extended_point_type : 2;
  laszip_U8 extended_scanner_channel : 2;
  laszip_U8 extended_classification_flags : 4;
  laszip_U8 extended_classification;
  laszip_U8 extended_return_number : 4;
  laszip_U8 extended_number_of_returns_of_given_pulse : 4;
  laszip_I16 extended_scan_angle;

  laszip_I32 num_extra_bytes;
  laszip_U8* extra_bytes;

} laszip_point_struct;

/*---------------------------------------------------------------------------*/
/*---------------- DLL functions to manage the LASzip DLL -------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_get_version
(
    laszip_U8*                         version_major
    , laszip_U8*                       version_minor
    , laszip_U16*                      version_revision
    , laszip_U32*                      version_build
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_create(
    laszip_POINTER*                    pointer
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_get_error
(
    laszip_POINTER                     pointer
    , laszip_CHAR**                    error
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_get_warning
(
    laszip_POINTER                     pointer
    , laszip_CHAR**                    warning
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_clean(
    laszip_POINTER                     pointer
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_destroy(
    laszip_POINTER                     pointer
);

/*---------------------------------------------------------------------------*/
/*---------- DLL functions to write and read LAS and LAZ files --------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_get_header_pointer(
    laszip_POINTER                     pointer
    , laszip_header_struct**           header_pointer
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_get_point_pointer(
    laszip_POINTER                     pointer
    , laszip_point_struct**            point_pointer
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_get_point_count(
    laszip_POINTER                     pointer
    , laszip_I64*                      count
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_set_header(
    laszip_POINTER                     pointer
    , const laszip_header_struct*      header
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_check_for_integer_overflow(
    laszip_POINTER                     pointer
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_auto_offset(
    laszip_POINTER                     pointer
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_set_point(
    laszip_POINTER                     pointer
    , const laszip_point_struct*       point
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_set_coordinates(
    laszip_POINTER                     pointer
    , const laszip_F64*                coordinates
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_get_coordinates(
    laszip_POINTER                     pointer
    , laszip_F64*                      coordinates
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_set_geokeys(
    laszip_POINTER                     pointer
    , laszip_U32                       number
    , const laszip_geokey_struct*      key_entries
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_set_geodouble_params(
    laszip_POINTER                     pointer
    , laszip_U32                       number
    , const laszip_F64*                geodouble_params
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_set_geoascii_params(
    laszip_POINTER                     pointer
    , laszip_U32                       number
    , const laszip_CHAR*               geoascii_params
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_add_vlr(
    laszip_POINTER                     pointer
    , const laszip_vlr_struct*         vlr
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_open_writer(
    laszip_POINTER                     pointer
    , const laszip_CHAR*               file_name
    , laszip_BOOL                      compress
    );

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_write_point(
    laszip_POINTER                     pointer
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_close_writer(
    laszip_POINTER                     pointer
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_open_reader(
    laszip_POINTER                     pointer
    , const laszip_CHAR*               file_name
    , laszip_BOOL*                     is_compressed
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_seek_point(
    laszip_POINTER                     pointer
    , laszip_I64                       index
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_read_point(
    laszip_POINTER                     pointer
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_close_reader(
    laszip_POINTER                     pointer
);

/*---------------------------------------------------------------------------*/
/*---------------- DLL functions to load and unload LASzip ------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_load_dll
(
);

/*---------------------------------------------------------------------------*/
LASZIP_API laszip_I32
laszip_unload_dll
(
);

#ifdef __cplusplus
}
#endif

#endif /* LASZIP_DLL_H */
