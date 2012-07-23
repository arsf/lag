/*
 * File:   LidarPoint.cpp
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


#include "LidarPoint.h"
#include <bitset>

LidarPoint::LidarPoint(double x, double y, double z, double time, uint16_t intensity, uint8_t classification, uint8_t flightline,
                       uint8_t returnNumber, uint8_t noOfReturns, uint8_t scanDirection, uint8_t scanEdge, uint8_t scanAngle,
                       uint16_t pointSourceId, int dataindex) : Point(x, y, z),
                       time 			(time),
                       intensity 		(intensity),
                       classification 	(classification),
                       flightline 		(flightline),
                       packedByte 		(returnNumber),
                       scanAngle		(scanAngle),
                       pointSourceId	(pointSourceId),
                       dataindex		(dataindex)
{
	packedByte = packedByte | noOfReturns << 3;
	packedByte = packedByte | scanDirection << 6;
	packedByte = packedByte | scanEdge << 7;
}

LidarPoint::LidarPoint() : Point()
{}



