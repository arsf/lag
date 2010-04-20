/* 
 * File:   lidarpointsaver.h
 * Author: chrfi
 *
 * Created on April 16, 2010, 3:36 PM
 */

#ifndef _LIDARPOINTSAVER_H
#define	_LIDARPOINTSAVER_H

class lidarpointsaver
{
public:
    virtual void savepoints(int n, point *points) = 0;
    virtual void finalizesave() = 0;
};

#endif	/* _LIDARPOINTSAVER_H */

