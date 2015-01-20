/*
 * File:   LidarPoint.h
 * Author: jaho
 *
 * Created on 10 March 2012
 *
 * ----------------------------------------------------------------
 *
 * This file is part of lidarquadtree, a library providing a data
 * structure for storing and indexing LiDAR points.
 *
 * Copyright (C) 2008-2012 Plymouth Marin Laboratory
 *    arsfinternal@pml.ac.uk
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * The GNU General Public License is contained in the file COPYING. *
 */

#ifndef _LIDAR_POINT_H_
#define _LIDAR_POINT_H_

#include "Point.h"
#include <stdint.h>
#include <laslib/lasdefinitions.hpp>
#include "PointData.h"

class LidarPoint: public Point
{

public:
   // Constructors
   LidarPoint();
   LidarPoint(double,double,double,double,uint16_t,uint8_t,uint8_t,uint8_t,uint8_t, uint8_t, uint8_t, uint8_t, uint16_t, int);

   // Getter methods
   double getTime();
   uint16_t getIntensity();
   uint8_t getClassification();
   uint8_t getFlightline();
   uint8_t getReturn();
   uint8_t getNumberOfReturns();
   uint8_t getScanDirection();
   uint8_t getScanEdge();
   uint8_t getScanAngle();
   uint16_t getPointSourceId();
   int getDataindex();

   // Assignment operator that gets values from a laslib::LASpoint
   LidarPoint& operator=(LASpoint const& laspoint);

   void setClassification(int);
   void setDataindex(int);
   void setFlightlineNumber(int);

private:
   double time;
   uint16_t intensity;
   uint8_t classification;
   uint8_t flightline;
   uint8_t packedByte;
   uint8_t scanAngle;
   uint16_t pointSourceId;
   int dataindex;
};

// Set classification
inline void LidarPoint::setClassification(int classification)
{
   this->classification = classification;
}

inline void LidarPoint::setDataindex(int dataindex)
{
	this->dataindex = dataindex;
}

inline void LidarPoint::setFlightlineNumber(int number)
{
	this->flightline = number;
}

// Getter methods
inline double LidarPoint::getTime() { return time; }
inline uint16_t LidarPoint::getIntensity() { return intensity; }
inline uint8_t LidarPoint::getClassification() { return classification; }
inline uint8_t LidarPoint::getFlightline() { return flightline; }
inline uint8_t LidarPoint::getReturn() { return packedByte & 7; }
inline uint8_t LidarPoint::getNumberOfReturns() { return (packedByte & 56) >> 3; }
inline uint8_t LidarPoint::getScanDirection() { return (packedByte & 64) >> 6; }
inline uint8_t LidarPoint::getScanEdge() { return (packedByte & 128) >> 7; }
inline uint8_t LidarPoint::getScanAngle() { return scanAngle; }
inline uint16_t LidarPoint::getPointSourceId() { return pointSourceId; }
inline int LidarPoint::getDataindex() { return dataindex; }

inline LidarPoint& LidarPoint::operator=(LASpoint const& laspoint)
{
	x_ = laspoint.get_x();
	y_ = laspoint.get_y();
	z_ = laspoint.get_z();
	time = laspoint.gps_time;
	intensity = laspoint.intensity;
	classification = laspoint.classification;
	packedByte = laspoint.return_number;
	scanAngle = laspoint.scan_angle_rank;
	pointSourceId = laspoint.point_source_ID;

	packedByte = packedByte | laspoint.number_of_returns << 3;
	packedByte = packedByte | laspoint.scan_direction_flag << 6;
	packedByte = packedByte | laspoint.edge_of_flight_line << 7;

	flightline = 0;

	return *this;
}

#endif
