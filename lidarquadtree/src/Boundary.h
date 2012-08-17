/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * A data container class storing a boundary (min and max XY values)
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
 * The GNU General Public License is contained in the file COPYING.
 */

#ifndef _BOUNDARY_H_
#define _BOUNDARY_H_

class Boundary
{
public:
    double minX;
    double minY;
    double maxX;
    double maxY;
};

#endif	/* _BOUNDARY_H */

