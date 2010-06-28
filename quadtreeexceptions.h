/* 
 * File:   exceptions.h
 * Author: chrfi
 *
 * Created on February 10, 2010, 4:10 PM
 */



#ifndef _QUADTREEEXCEPTIONS_H
#define	_QUADTREEEXCEPTIONS_H

#include <exception>

using namespace std;

/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * an extension of the standard c++ exception class which adds
 * a why() method so that additional information can be passed. the idea is that the
 * existing what() method tells you the type of exception and the details attribute (which is
 * returned by the why() method) can be set when the exception is thrown and should explain why it was thrown.
 */
class DescriptiveException : public exception
{
private:
    const char *details;
public:

    DescriptiveException(const char *details)
    {
        this->details = details;
    }

    const char* why()
    {
        return details;
    }
};


/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * new exception extended from DescriptiveException
 */
class RamAllocationException : public DescriptiveException
{
public:

    virtual const char* what() const throw ()
    {
        return "an exception has occoured during ram allocation";
    }

    RamAllocationException(const char *details)
    : DescriptiveException(details) { }
};


/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * new exception extended from DescriptiveException
 */
class FileException : public DescriptiveException
{
public:

    virtual const char* what() const throw ()
    {
        return "file io exception";
    }

    FileException(const char *details)
    : DescriptiveException(details) { }
};


/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * new exception extended from DescriptiveException
 */
class OutOfBoundsException : public DescriptiveException
{
public:

    virtual const char* what() const throw ()
    {
        return "a varible has fallen outside all possible boundarys or values";
    }

    OutOfBoundsException(const char *details)
    : DescriptiveException(details) { }
};


/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * new exception extended from DescriptiveException
 */
class NullPointerException : public DescriptiveException
{
public:

    virtual const char* what() const throw ()
    {
        return "a method or attribute of a pointer which is null has been requested";
    }

    NullPointerException(const char *details)
    : DescriptiveException(details) { }
};


#endif	/* _QUADTREEEXCEPTIONS_H */

