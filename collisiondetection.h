
/* 
 * File:   collisiondetection.h
 * Author: chrfi
 *
 * Created on March 23, 2010, 10:38 AM
 */

#ifndef _COLLISIONDETECTION_H
#define	_COLLISIONDETECTION_H

/**
 * a function that checks for collision between an axis orientated rectangle and a none axis orientated rectangle
 *
 * @note the x and y values for the corners of a polygon must be in sequential order
 * (so that drawing a line from 1 to 2 to 3 to 1 forms a polygon)
 * @param minX the minimum x value of the AOrec
 * @param minY the maximum y value of the AOrec
 * @param maxX the minimum x value of the AOrec
 * @param maxY the maximum y value of the AOrec
 * @param Xs a pointer to an array of x values, each for a corner of the rectangle (must be in consecutive order)
 * @param Ys a pointer to an array of y values, each for a corner of the rectangle (must be in consecutive order)
 * @param size the size of the Xs and Ys arrays (should always be 4 in this case)
 *
 * @return true indicates a collision
 */
bool AOrec_NAOrec(double minX, double minY, double maxX, double maxY, double *Xs, double *Ys, int size);

/**
 * a function to compare a check if a point falls within a non axis orientated rectangle
 *
 * @note the x and y values for the corners of a polygon must be in sequential order
 * (so that drawing a line from 1 to 2 to 3 to 1 forms a polygon)
 * @param px x value of the point
 * @param py y value of the point
 * @param Xs a pointer to an array of x values, each for a corner of the rectangle (must be in consecutive order)
 * @param Ys a pointer to an array of y values, each for a corner of the rectangle (must be in consecutive order)
 * @param size the size of the Xs and Ys arrays (should always be 4 in this case)
 *
 * @return true indicates a collision
 */
bool point_NAOrec(double px, double py, double *Xs, double *Ys, int size);

/**
 * a function that checks for collision between two convex polgons
 *
 * @note the x and y values for the corners of a polygon must be in sequential order
 * (so that drawing a line from 1 to 2 to 3 to 1 forms a polygon)
 * @param poly1Xs a pointer to an array of x values each of which is from a corner of the polygon 1
 * @param poly1Ys a pointer to an array of y values each of which is from a corner of the polygon 1
 * @param poly1size the number of corners on polygon 1 (the length of the poly1Xs and poly1Ys arrays)
 * @param poly2Xs a pointer to an array of x values each of which is from a corner of the polygon 2
 * @param poly2Ys a pointer to an array of y values each of which is from a corner of the polygon 2
 * @param poly2size the number of corners on polygon 2 (the length of the poly2Xs and poly2Ys arrays)
 */
bool axis_sep_test(double *poly1Xs, double *poly1Ys, int poly1size, double *poly2Xs, double *poly2Ys, int poly2size);

#endif	/* _COLLISIONDETECTION_H */

