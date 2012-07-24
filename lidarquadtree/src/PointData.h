/*
 * File:   PointData.h
 * Author: jaho
 *
 * Created on 15 March 2012
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

#ifndef POINTDATA_H_
#define POINTDATA_H_


class PointData
{
public:
	uint8_t wfDescriptorIndex;
	U64 wfOffset;
	uint32_t wfPacketSize;
	float wfPointLocation;
	float xt;
	float yt;
	float zt;

	// Assignment operator that gets values from a laslib::LASwavepacket
	PointData& operator=(LASwavepacket const& wavepacket);
};

inline PointData& PointData::operator=(LASwavepacket const& wp)
{
	wfDescriptorIndex = wp.getIndex();
	wfOffset = wp.getOffset();
	wfPacketSize = wp.getSize();
	wfPointLocation = wp.getLocation();
	xt = wp.getXt();
	yt = wp.getYt();
	zt = wp.getZt();

	return *this;
}

#endif /* POINTDATA_H_ */
