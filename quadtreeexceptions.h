/* 
 * File:   exceptions.h
 * Author: chrfi
 *
 * Created on February 10, 2010, 4:10 PM
 */

#include <exception>

#ifndef _QUADTREEEXCEPTIONS_H
#define	_QUADTREEEXCEPTIONS_H

class descriptiveexception : public exception
{
    private:
    const char *details;
public:
    descriptiveexception(const char *details)
    {
        this->details = details;
    }

    const char* why()
    {
        return details;
    }

};

class ramallocationexception : public descriptiveexception
{
public:

    virtual const char* what() const throw ()
    {
        return "an exception has occoured during ram allocation";
    }

    ramallocationexception(const char *details)
    : descriptiveexception(details)
    {}


};

class fileexception : public descriptiveexception
{

public:
    virtual const char* what() const throw ()
    {
        return "file io exception";
    }

    fileexception(const char *details)
    : descriptiveexception(details)
    {}

};

class outofboundsexception : public descriptiveexception
{

public:
    virtual const char* what() const throw ()
    {
        return "a varible has fallen outside all possible boundarys or values";
    }

    outofboundsexception(const char *details)
    : descriptiveexception(details)
    {}


};

class nullpointerexception : public descriptiveexception
{

public:
    virtual const char* what() const throw ()
    {
        return "a method or attribute of a pointer which is null has been requested";
    }

    nullpointerexception(const char *details)
    : descriptiveexception(details)
    {}


};


#endif	/* _QUADTREEEXCEPTIONS_H */

