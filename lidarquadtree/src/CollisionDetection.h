/* 
 * File:   collisiondetection.h
 * Author: chrfi
 *
 * Created on March 23, 2010, 10:38 AM
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

#ifndef _COLLISIONDETECTION_H
#define	_COLLISIONDETECTION_H

#include <vector>


/**
 * a function that checks for collision between an axis orientated 
 * rectangle(ao) and a none axis orientated(nao) rectangle
 *
 * @note the x and y values for the corners of a polygon must be in 
 * sequential order
 * (so that drawing a line from 1 to 2 to 3 to 1 forms a polygon)
 * @param minX the minimum x value of the AOrec
 * @param minY the maximum y value of the AOrec
 * @param maxX the minimum x value of the AOrec
 * @param maxY the maximum y value of the AOrec
 * @param Xs an array of doubles each of which is the x componant of a
 * point of the polygon (in sequence)
 * @param Ys an array of doubles each of which is the y componant of a 
 * point of the polygon (in sequence)
 * @param size the number of corners that make up the polygon (the length 
 * of the Xs and Ys arrays)
 *
 * @return true indicates a collision
 */
bool aoRectangleNaoRectangle(double minX, double minY, double maxX, 
                             double maxY, std::vector<double> Xs, std::vector<double> Ys,
                             int size);

/**
 * a function to test if a point falls within a convex polygon
 *
 * @note the x and y values for the corners of a polygon must be in 
 * sequential order
 * (so that drawing a line from 1 to 2 to 3 to 1 forms a polygon)
 * @param Xs an array of doubles each of which is the x componant 
 * of a point of the polygon (in sequence)
 * @param Ys an array of doubles each of which is the y componant 
 * of a point of the polygon (in sequence)
 * @param size the number of corners that make up the polygon (the 
 * length of the Xs and Ys arrays)
 * @param px x value of the point
 * @param py y value of the point
 *
 * @return true indicates that the point falls inside the polygon
 */
bool vectorTest(std::vector<double> Xs, std::vector<double> Ys, int size, double px, double py);

/**
 * a function that checks for collision between two convex polgons
 *
 * @note the x and y values for the corners of a polygon must be in 
 * sequential order
 * (so that drawing a line from 1 to 2 to 3 to 1 forms a polygon)
 * @param Xs1 an array of doubles each of which is the x componant of a
 * point of the polygon (in sequence)
 * @param Ys1 an array of doubles each of which is the y componant of a 
 * point of the polygon (in sequence)
 * @param size1 the number of corners that make up the polygon (the 
 * length of the Xs and Ys arrays)
 * @param Xs2 an array of doubles each of which is the x componant of a 
 * point of the polygon (in sequence)
 * @param Ys2 an array of doubles each of which is the y componant of a 
 * point of the polygon (in sequence)
 * @param size2 the number of corners that make up the polygon (the length 
 * of the Xs and Ys arrays)
 */
bool axisSeperationTest(std::vector<double> Xs1, std::vector<double> Ys1, int size1,
                        std::vector<double> Xs2, std::vector<double> Ys2, int size2);

#endif	/* _COLLISIONDETECTION_H */

