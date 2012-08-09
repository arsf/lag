/*
==================================

 LagDisplay.cpp

 Created on: June-July 2010
 Authors:   Andy Chambers, Haraldur Tristan Gunnarsson, Jan Holownia,
            Berin Smaldon

 LIDAR Analysis GUI (LAG), viewer for LIDAR files in .LAS or ASCII format
 Copyright (C) 2009-2012 Plymouth Marine Laboratory (PML)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

==================================
*/

#include <gtkmm.h>
#include <gtkglmm.h>
#include <vector>
#include <iostream>
#include "Quadtree.h"
#include "PointBucket.h"
#ifdef __WIN32 // glx.h uses EXTERN everywhere for __WIN32, breaking things
#define EXTERN extern
#endif
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "MathFuncs.h"
#include "LagDisplay.h"


/*
==================================
 LagDisplay::LagDisplay
==================================
*/
LagDisplay::LagDisplay(const Glib::RefPtr<const Gdk::GL::Config>& config, int bucketlimit) :
		Gtk::GL::DrawingArea(config),
		zoomlevel			(1),
		zoompower			(0.5),
		lidardata			(NULL),
		bucketlimit			(bucketlimit),
		maindetailmod		(0),
		pointsize			(1),
		ratio		   		(1.0),
		brightnessBy		(brightnessByIntensity),
		colourBy		   	(colourByFlightline),
		cbmaxz				(0),
		cbminz				(0),
		cbmaxintensity		(0),
		cbminintensity		(0),
		zoffset				(0),
		zfloor				(0),
		intensityoffset	(0),
		intensityfloor		(0),
		rmaxz				   (0),
		rminz				   (0),
		rmaxintensity		(0),
		rminintensity		(0),
		colourheightarray	(NULL),
		brightnessheightarray	(NULL),
		brightnessintensityarray(NULL),
		red					(0.0),
		green 				(0.0),
		blue	   			(0.0),
		alpha		   		(0.0),
      interrupt_drawing (false),
      GL_data_impede    (false),
      GL_control_impede (false),
      global_pointbucket_mutex   (NULL),
      drawing_thread    (NULL)
{
}

/*
==================================
 LagDisplay::~LagDisplay
==================================
*/
LagDisplay::~LagDisplay()
{
	delete[] colourheightarray;
	delete[] brightnessheightarray;
	delete[] brightnessintensityarray;

   delete drawing_thread;
   drawing_thread = NULL;
}

/*
==================================
 LagDisplay::on_expose_event

 Draw on expose.
==================================
*/
bool LagDisplay::on_expose_event(GdkEventExpose* event)
{
	if (event == 0)
	{
	}
	drawviewable(1);
	return true;
}

/*
==================================
 LagDisplay::on_configure_event

 Please note that the Gtk::GL::DrawingArea::on_realize() method calls this
 event and is (by necessity) before the prepare_image() method, so it is
 necessary to be careful about what to put into this method as some things
 might crash if done before prepare_image().

 When the window is resized, the viewport is resized accordingly and so are
 the viewing properties. Please note that the expose event will be emitted
 right after the configure event, so the on_expose_event method will be
 called right after this one.
==================================
*/
bool LagDisplay::on_configure_event(GdkEventConfigure* event)
{
	if (event == 0)
	{
	}

	glViewport(0, 0, get_width(), get_height());
	resetview();
	return true;
}

/*
==================================
 LagDisplay::coloursandshades

 Prepares the arrays for looking up the colours and shades of the points.
==================================
*/
void LagDisplay::coloursandshades(double maxz, double minz, int maxintensity,
		int minintensity)
{
	//For the legend(s):
	cbmaxz = maxz;
	cbminz = minz;
	cbmaxintensity = maxintensity;
	cbminintensity = minintensity;

	if (colourheightarray != NULL)
		delete[] colourheightarray;
	if (brightnessheightarray != NULL)
		delete[] brightnessheightarray;
	if (brightnessintensityarray != NULL)
		delete[] brightnessintensityarray;

	Colour colour;
	double z = 0, intensity = 0;

	// This is at 30, rather than three, for a reason: three, one for each
	// colour, by ten, so that the "resolution" of the colouring by height
	// is 0.1 metres, not one whole metre. It makes colouring by height
	// look better.
	colourheightarray = new double[30 * (int) (rmaxz - rminz + 4)];

	//Fill height colour array (by ten for extra detail):
	for (int i = 0; i < (int) (10 * (rmaxz - rminz) + 3); i++)
	{
		//0.1 for 0.1 metres for extra detail.
		z = 0.1 * (double) i + rminz;
		colour_by(z, maxz, minz, colour);
		colourheightarray[3 * i] = colour.getR();
		colourheightarray[3 * i + 1] = colour.getG();
		colourheightarray[3 * i + 2] = colour.getB();
	}

	brightnessheightarray = new double[10 * (int) (rmaxz - rminz + 4)];
	//Fill height brightness array:
	for (int i = 0; i < 10 * (int) (rmaxz - rminz + 4); i++)
	{
		// Please note that, as elsewhere in this method, the exact value rmaxz
		// will not be reached, so its position in the array will probably not be
		// the first to equal 1, while the first element will be 0 and be for
		// rminz.
		z = 0.1 * (double) i + rminz;
		brightnessheightarray[i] = brightness_by(z, maxz, minz, zoffset,
				zfloor);
	}
	brightnessintensityarray = new double[(int) (rmaxintensity - rminintensity
			+ 4)];
	//Fill intensity brightness array:
	for (int i = 0; i < rmaxintensity - rminintensity + 4; i++)
	{
		intensity = i + rminintensity;
		brightnessintensityarray[i] = brightness_by(intensity, maxintensity,
				minintensity, intensityoffset, intensityfloor);
	}
}

/*
==================================
 LagDisplay::on_realize

 Prepare the image when the widget is first realised.

 Please note that the this method calls the configure event and is (by
 necessity) before the prepare_image() method, so it is necessary to be
 careful about what to put into the on_configure_event method as
 some things might crash if done before prepare_image().
==================================
*/
void LagDisplay::on_realize()
{
	Gtk::GL::DrawingArea::on_realize();
	prepare_image();
}

/*
==================================
 LagDisplay::getColourByFlightline
==================================
*/
Colour LagDisplay::getColourByFlightline(int flightline)
{
	switch (flightline % 6)
	{
	case 0:
		return Colour(0, 1, 0); // Green
	case 1:
		return Colour(1, 0, 0); // Yellow
	case 2:
		return Colour(1, 1, 0); // Red
	case 3:
		return Colour(0, 1, 1); // Cyan
	case 4:
		return Colour(0, 0, 1); // Blue
	case 5:
		return Colour(1, 0, 1); // Purple
	default:
		return Colour(1, 1, 1); // White
	}
}

/*
==================================
 LagDisplay::getColourByReturn
==================================
*/
Colour LagDisplay::getColourByReturn(int returnNumber)
{
	switch (returnNumber)
	{
	case 1:
		return Colour(0, 0, 1); // Blue
	case 2:
		return Colour(0, 1, 1); // Cyan
	case 3:
		return Colour(0, 1, 0); // Green
	case 4:
		return Colour(1, 0, 0); // Red
	case 5:
		return Colour(1, 0, 1); // Purple
	case 6:
		return Colour(1, 1, 0); // Yellow
	case 7:
		return Colour(1, 0.5, 0.5); // Pink
	default:
		return Colour(1, 1, 1); // White
	}
}

/*
==================================
 LagDisplay::getColourByClassification
==================================
*/
Colour LagDisplay::getColourByClassification(int classification)
{
	switch (classification)
	{
	//Yellow for non-classified.
	case 0:
	case 1:
		return Colour(1, 1, 0);
		//Brown for ground.
	case 2:
		return Colour(0.6, 0.3, 0);
		//Dark green for low vegetation.
	case 3:
		return Colour(0, 0.3, 0);
		//Medium green for medium vegetation.
	case 4:
		return Colour(0, 0.6, 0);
		//Bright green for high vegetation.
	case 5:
		return Colour(0, 1, 0);
		//Cyan for buildings.
	case 6:
		return Colour(0, 1, 1);
		//Purple for low point (noise).
	case 7:
		return Colour(1, 0, 1);
		//Grey for model key-point (mass point).
	case 8:
		return Colour(0.5, 0.5, 0.5);
		//Blue for water.
	case 9:
		return Colour(0, 0, 1);
		//White for overlap points.
	case 12:
		return Colour(1, 1, 1);
		//Red for undefined.
	default:
		return Colour(1, 0, 0);
	}
}

/*
==================================
 LagDisplay::getColourByHeight
==================================
*/
Colour LagDisplay::getColourByHeight(float height)
{
	return Colour(colourheightarray[3 * int(10 * (height - rminz))],
			colourheightarray[3 * int(10 * (height - rminz)) + 1],
			colourheightarray[3 * int(10 * (height - rminz)) + 2]);
}

/*
==================================
 LagDisplay::getColourByIntensity
==================================
*/
Colour LagDisplay::getColourByIntensity(int intensity)
{
	double greyValue = double(intensity - cbminintensity)
			/ (cbmaxintensity - cbminintensity);
	return Colour(greyValue, greyValue, greyValue);
}

/*
==================================
 LagDisplay::colour_by

 Given maximum and minimum values, find out the colour a certain value
 should be mapped to.
==================================
*/
void LagDisplay::colour_by(double value, double maxvalue, double minvalue,
		Colour& colour)
{
	double range = maxvalue - minvalue;
	if (value <= minvalue + range / 6) //Green to Yellow:
		colour.setRGB(6 * (value - minvalue) / range, 1.0, 0.0);
	else if (value <= minvalue + range / 3) //Yellow to Red:
		colour.setRGB(1.0, 2.0 - 6 * (value - minvalue) / range, 0.0);
	else if (value <= minvalue + range / 2) //Red to Purple:
		colour.setRGB(1.0, 0.0, 6 * (value - minvalue) / range - 2.0);
	else if (value <= minvalue + range * 2 / 3) //Purple to Blue:
		colour.setRGB(4.0 - 6 * (value - minvalue) / range, 0.0, 1.0);
	else if (value <= minvalue + range * 5 / 6) //Blue to Cyan:
		colour.setRGB(0.0, 6 * (value - minvalue) / range - 4.0, 1.0);
	else //Cyan to White:
		colour.setRGB(6 * (value - minvalue) / range - 5.0, 1.0, 1.0);
}

/*
==================================
 LagDisplay::brightness_by

 Given maximum and minimum values, find out the brightness a certain value
 should be mapped to.
==================================
*/
double LagDisplay::brightness_by(double value, double maxvalue, double minvalue,
		double offsetvalue, double floorvalue)
{
	double multiplier = floorvalue + offsetvalue
			+ (1.0 - floorvalue) * (value - minvalue) / (maxvalue - minvalue);

	// This prevents the situation where two negative values (for colour and
	// brightness respectively) multiply to create a positive value giving a
	// non-black colour.
	if (multiplier < 0)
		multiplier = 0;

	// This prevents the situation where a non-white colour can be made paler
	// (rather than simply brighter) as a result of its RGB values being
	// multiplied by some number greater than 1. Otherwise, non-white colours
	// can eventually be made white when brightness mode is on.
	if (multiplier > 1)
		multiplier = 1;
	return multiplier;
}

/*
==================================
 LagDisplay::advsubsetproc

 Convenience code for getting a subset and checking to see if the data is
 valid/useful.
==================================
*/
bool LagDisplay::advsubsetproc(vector<PointBucket*>*& pointvector, vector<double> xs,
		vector<double> ys, int ps)
{
	if (lidardata == NULL)
		return false;

	try
	{
		pointvector = lidardata->advSubset(xs, ys, ps);
	} catch (DescriptiveException& e)
	{
		cout << "There has been an exception:" << endl;
		cout << "What: " << e.what() << endl;
		cout << "Why: " << e.why() << endl;
		cout << "No points returned." << endl;
		return false;
	}

	if (pointvector == NULL || pointvector->size() == 0)
		return false;
	else
		return true;
}

/*
==================================
 LagDisplay::pixelsToImageUnits
==================================
*/
double LagDisplay::pixelsToImageUnits(double pixels)
{
	return pixels * ratio / zoomlevel;
}

/*
==================================
 LagDisplay::imageUnitsToPixels
==================================
*/
double LagDisplay::imageUnitsToPixels(double imageUnits)
{
	return imageUnits * zoomlevel / ratio;
}

/*
==================================
 LagDisplay::printString
==================================
*/
void LagDisplay::printString(double x, double y, double z)
{
	glRasterPos3d(x, y, z);
}

/*
==================================
 LagDisplay::clearscreen

 Convenience code for clearing the screen.
==================================
*/
bool LagDisplay::clearscreen()
{
	Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
	if (!glwindow->gl_begin(get_gl_context()))
		return false;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (glwindow->is_double_buffered())
		glwindow->swap_buffers();
	else
		glFlush();

	glwindow->gl_end();
	return true;
}

/*
==================================
 LagDisplay::guard_against_interaction_between_GL_areas

 Convenience code called so that graphical artefacts through changes of view
 do not occur. This is because of being a multiwindow application.
==================================
*/
void LagDisplay::guard_against_interaction_between_GL_areas()
{
	// This line MUST come before the other ones for this purpose as otherwise
	// the others might be applied to the wrong context!
	get_gl_window()->make_current(get_gl_context());
	glPointSize(pointsize);
	glViewport(0, 0, get_width(), get_height());
	resetview();
}

/*
==================================
 LagDisplay::prepare_image

 This method prepares the image for drawing and sets up OpenGl. It gets data
 from the quadtree in order to find the maximum and minimum height and
 intensity values and calls the coloursandshades() method to prepare the
 colouring of the points. It also sets up clearing and the initial view.
==================================
*/
void LagDisplay::prepare_image()
{
	//Subsetting:
	Boundary* lidarboundary = lidardata->getBoundary();
	vector<double> xs(4);
	xs[0] = lidarboundary->minX;
	xs[1] = lidarboundary->minX;
	xs[2] = lidarboundary->maxX;
	xs[3] = lidarboundary->maxX;
	vector<double> ys(4);
	ys[0] = lidarboundary->minY;
	ys[1] = lidarboundary->maxY;
	ys[2] = lidarboundary->maxY;
	ys[3] = lidarboundary->minY;
	vector<PointBucket*> *pointvector = NULL;

	//Get ALL data.
	bool gotdata = advsubsetproc(pointvector, xs, ys, 4);
	delete lidarboundary;

	//If there is no data, then clear the screen to show no data.
	if (!gotdata)
	{
		Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
		if (!glwindow->gl_begin(get_gl_context()))
			return;

		glClearColor(red, green, blue, alpha);
		glClearDepth(1.0);
		glwindow->gl_end();
		clearscreen();
		if (pointvector != NULL)
			delete pointvector;
		return;
	}
	//Determining colouring and shading:
	double maxz = (*pointvector)[0]->getmaxZ(), minz =
			(*pointvector)[0]->getminZ();
	int maxintensity = (*pointvector)[0]->getmaxintensity(), minintensity =
			(*pointvector)[0]->getminintensity();

	//Find the maximum and minimum values from the (*pointvector):
	for (unsigned int i = 0; i < pointvector->size(); i++)
	{
		if (maxz < (*pointvector)[i]->getmaxZ())
			maxz = (*pointvector)[i]->getmaxZ();
		if (minz > (*pointvector)[i]->getminZ())
			minz = (*pointvector)[i]->getminZ();
		if (maxintensity < (*pointvector)[i]->getmaxintensity())
			maxintensity = (*pointvector)[i]->getmaxintensity();
		if (minintensity > (*pointvector)[i]->getminintensity())
			minintensity = (*pointvector)[i]->getminintensity();
	}
	if (maxz <= minz)
		maxz = minz + 1;
	if (maxintensity <= minintensity)
		maxintensity = minintensity + 1;

	rmaxz = maxz;
	rminz = minz;
	rmaxintensity = maxintensity;
	rminintensity = minintensity;

	//Prepare colour and shading arrays.
	coloursandshades(maxz, minz, maxintensity, minintensity);

	//OpenGL setup:
	Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
	if (!glwindow->gl_begin(get_gl_context()))
		return;
	glClearColor(red, green, blue, alpha);
	glClearDepth(1.0);
	glPointSize(pointsize);

	// Very important to include this! This allows us to see the things on
	// the top above the things on the bottom!
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, get_width(), get_height());
	glwindow->gl_end();
	resetview();
	delete pointvector;

	//Set up initial view and draw it.
	returntostart();
}

/*
==================================
 LagDisplay::set_background_colour
==================================
*/
void LagDisplay::set_background_colour(float r, float g, float b, float a)
{
	red = r;
	green = g;
	blue = b;
	alpha = a;
}

/*
==================================
 LagDisplay::update_background_colour
==================================
*/
void LagDisplay::update_background_colour()
{
	Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
	if (!glwindow->gl_begin(get_gl_context()))
		return;
	glClearColor(red, green, blue, alpha);
	glClearDepth(1.0);
}

/*
================================================================================
 LagDisplay::abortFrame

 Instructs this object to abort any frames being presently drawn
================================================================================
*/
void LagDisplay::abortFrame(bool forceGL)
{
   Glib::Mutex::Lock lock (GL_action);
   interrupt_drawing = true;

   if (forceGL)
   {
      GL_control_condition.signal();
      GL_data_condition.signal();
   }
}

/*
================================================================================
 LagDisplay::clear_abortFrame

 Notes that the present frame has ended and should no longer be aborted
================================================================================
*/
void LagDisplay::clear_abortFrame()
{
   Glib::Mutex::Lock lock (GL_action);
   interrupt_drawing = false;
}

/*
================================================================================
 LagDisplay::stopDrawingThread

 Stops the drawing thread and blocks until it is finished
================================================================================
*/
void LagDisplay::stopDrawingThread()
{
   if (drawing_thread)
   {
      drawing_thread->stop();
      drawing_thread->join();
   }
}

/*
================================================================================
 LagDisplay::awaitClearGLData

 Blocks until the thread is interrupted, or the GL data arrays become available

 Parameters:
   reserves - whether to reserve exclusive access to the GL data
 Returns: whether it was interrupted or not, interrupted calls do not receive
          exclusive access
================================================================================
*/
bool LagDisplay::awaitClearGLData(bool reserves)
{
   Glib::Mutex::Lock lock (GL_action);

   while (GL_data_impede && !interrupt_drawing)
      GL_data_condition.wait(GL_action);

   if (!interrupt_drawing)
      GL_data_impede = reserves;

   return interrupt_drawing;
}

/*
================================================================================
 LagDisplay::awaitClearGLControl

 Blocks until the thread is interrupted, or GL is available to accept more
 instruction

 Parameters:
   reserves - whether to reserve exclusive access to GL control
 Returns: whether it was interrupted or not, interrupted calls do not receive
          exclusive access
================================================================================
*/
bool LagDisplay::awaitClearGLControl(bool reserves)
{
   Glib::Mutex::Lock lock (GL_action);

   while (GL_control_impede && !interrupt_drawing)
      GL_control_condition.wait(GL_action);

   if (!interrupt_drawing)
      GL_control_impede = reserves;

   return interrupt_drawing;
}
