
/* 
 * File:   collisiondetection.h
 * Author: chrfi
 *
 * Created on March 23, 2010, 10:38 AM
 */

#ifndef _COLLISIONDETECTION_H
#define	_COLLISIONDETECTION_H

bool point_NAOrec(double px, double py, double *Xs, double *Ys, int size);
bool AOrec_NAOrec(double minX, double minY, double maxX, double maxY, double *Xs, double *Ys, int size);
bool NAOrec_pixel(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double px, double py);
bool axis_sep_test(double *poly1Xs, double *poly1Ys, int poly1size, double *poly2Xs, double *poly2Ys, int poly2size);

#endif	/* _COLLISIONDETECTION_H */

