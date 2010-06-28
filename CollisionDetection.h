
/* 
 * File:   collisiondetection.h
 * Author: chrfi
 *
 * Created on March 23, 2010, 10:38 AM
 */

#ifndef _COLLISIONDETECTION_H
#define	_COLLISIONDETECTION_H

/**
 * a function that checks for collision between an axis orientated rectangle(ao) and a none axis orientated(nao) rectangle
 *
 * @note the x and y values for the corners of a polygon must be in sequential order
 * (so that drawing a line from 1 to 2 to 3 to 1 forms a polygon)
 * @param minX the minimum x value of the AOrec
 * @param minY the maximum y value of the AOrec
 * @param maxX the minimum x value of the AOrec
 * @param maxY the maximum y value of the AOrec
 * @param Xs an array of doubles each of which is the x componant of a point of the polygon (in sequence)
 * @param Ys an array of doubles each of which is the y componant of a point of the polygon (in sequence)
 * @param size the number of corners that make up the polygon (the length of the Xs and Ys arrays)
 *
 * @return true indicates a collision
 */
bool aoRectangleNaoRectangle(double minX, double minY, double maxX, double maxY, double *Xs, double *Ys, int size);

/**
 * a function to compare a check if a point falls within a non axis orientated rectangle(nao)
 *
 * @note the x and y values for the corners of a polygon must be in sequential order
 * (so that drawing a line from 1 to 2 to 3 to 1 forms a polygon)
 * @param px x value of the point
 * @param py y value of the point
 * @param Xs an array of doubles each of which is the x componant of a point of the polygon (in sequence)
 * @param Ys an array of doubles each of which is the y componant of a point of the polygon (in sequence)
 * @param size the number of corners that make up the polygon (the length of the Xs and Ys arrays)
 *
 * @return true indicates a collision
 */
bool pointNaoRectangle(double px, double py, double *Xs, double *Ys, int size);

/**
 * a function that checks for collision between two convex polgons
 *
 * @note the x and y values for the corners of a polygon must be in sequential order
 * (so that drawing a line from 1 to 2 to 3 to 1 forms a polygon)
 * @param Xs1 an array of doubles each of which is the x componant of a point of the polygon (in sequence)
 * @param Ys1 an array of doubles each of which is the y componant of a point of the polygon (in sequence)
 * @param size1 the number of corners that make up the polygon (the length of the Xs and Ys arrays)
 * @param Xs2 an array of doubles each of which is the x componant of a point of the polygon (in sequence)
 * @param Ys2 an array of doubles each of which is the y componant of a point of the polygon (in sequence)
 * @param size2 the number of corners that make up the polygon (the length of the Xs and Ys arrays)
 */
bool axisSeperationTest(double *Xs1, double *Ys1, int size1, double *Xs2, double *Ys2, int size2);

#endif	/* _COLLISIONDETECTION_H */

