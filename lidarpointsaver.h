/* 
 * File:   lidarpointsaver.h
 * Author: chrfi
 *
 * Created on April 16, 2010, 3:36 PM
 */

#ifndef _LIDARPOINTSAVER_H
#define	_LIDARPOINTSAVER_H

/**
 * This interface defines how classes which provide saving logic for lidar points must look
 * the interface allows the quadtree to simply take any subclass and call its methods without understanding how the saving is being done
 * this means that the logic for loading different formats is encapsulated in classes independant of the quadtree
 */
class lidarpointsaver
{
public:
    /**
     * This method is called repeatadly by the quadtree and must take the arrays of ponits it is given and save them
     *
     * @param n the number of points contained in the array
     * @param points the array of points to be saved
     */
    virtual void savepoints(int n, point *points) = 0;

};

#endif	/* _LIDARPOINTSAVER_H */

