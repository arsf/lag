/* 
 * File:   QuadtreeExceptions.h
 * Author: chrfi
 *
 * Created on February 10, 2010, 4:10 PM
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

#ifndef _QUADTREEEXCEPTIONS_H
#define	_QUADTREEEXCEPTIONS_H

#include <exception>

using namespace std;


/**
 * @author Christopher Stanley Finerty
 * @version 2.0
 *
 * an extension of the standard c++ exception class which adds
 * a why() method so that additional information can be passed. 
 * the idea is that the existing what() method tells you the type 
 * of exception and the details attribute (which is returned by the why()
 *  method) can be set when the exception is thrown and should explain 
 *  why it was thrown.
 */
class DescriptiveException : public exception
{
private:

public:
	const char *details;

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

    	return details; //"a variable has fallen outside all possible boundaries or values";
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
        return "a method or attribute of a pointer which is \
                null has been requested";
    }

    NullPointerException(const char *details)
    : DescriptiveException(details) { }
};


#endif	/* _QUADTREEEXCEPTIONS_H */

