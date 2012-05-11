/*
==================================
 LoadWorker.cpp

  Created on: 24 Apr 2012
      Author: jaho
==================================
*/

#include "LoadWorker.h"
#include "ui/FileOpener.h"



LoadWorker::LoadWorker(FileOpener* fo, int point_offset, std::vector<std::string> filenames, bool create_new, bool usearea, int resolutiondepth, int resolutionbase,
		int bucketlevels, int bucketlimit, int cachelimit, bool default_scale_factors, double scale_factor[3],
		std::string ascii_code, SelectionBox fence) :
		fileopener			(fo),
		point_offset		(point_offset),
		filenames			(filenames),
		create_new_quadtree	(create_new),
		usearea				(usearea),
		resolutiondepth		(resolutiondepth),
		resolutionbase		(resolutionbase),
		bucketLevels		(bucketlevels),
		bucketlimit			(bucketlimit),
		cachelimit			(cachelimit),
		use_default_scale_factors	(default_scale_factors),
		ascii_code			(ascii_code),
		fence				(fence),
		thread				(0),
		stop				(false)
{
	if (!use_default_scale_factors)
	{
		this->scale_factor[0] = scale_factor[0];
		this->scale_factor[1] = scale_factor[1];
		this->scale_factor[2] = scale_factor[2];
	}
	else
	{
		this->scale_factor[0] = this->scale_factor[1] = this->scale_factor[2] = 0;
	}
}


LoadWorker::~LoadWorker()
{
	{
		Glib::Mutex::Lock lock (mutex);
		stop = true;
	}
	if (thread)
		thread->join();
}

void LoadWorker::run()
{
	// If a new quadtree has been created
	bool newQuadtree = false;

	try
	{
		// Using fence
		if (usearea)
		{
			std::cout << "LoadWorker: using fence." << "\n";
			for (size_t i = 0; i < filenames.size(); ++i)
			{
				std::string filename = filenames[i];

				if (filename != "")
				{
					{
						LasLoader* loader = 0;
						bool validfile = true;

						// for .LAS files
						if (filename.find(".las", filename.length()-4) != std::string::npos || filename.find(".LAS", filename.length()-4) != std::string::npos)
						{
							loader = new LasLoader(filename.c_str());
						}
						// ASCII files
						else if (filename.find(".txt", filename.length()-4) != std::string::npos || filename.find(".TXT", filename.length()-4) != std::string::npos)
						{
							if (use_default_scale_factors)
							{
								loader = new LasLoader(filename.c_str(), ascii_code.c_str());
							}
							else
							{
								loader = new LasLoader(filename.c_str(), ascii_code.c_str(), scale_factor);
							}
						}
						// Incorrect files
						else
						{
							std::cout << "LoadWorker: Files must have an extension .las or .txt." << std::endl;
							validfile = false;
						}

						if (validfile)
						{
							// If refreshing
							if ((i == 0 && (create_new_quadtree || !fileopener->loadedanyfiles)) || fileopener->lidardata == NULL)
							{
								if (fileopener->lidardata != NULL)
								{
									delete fileopener->lidardata;
								}

								fileopener->lidardata = NULL;

								Quadtree* qt = new Quadtree(loader, bucketlimit, point_offset, fence.getXs(), fence.getYs(), 4, cachelimit, 0, resolutionbase, resolutiondepth);
								int numberofpointsloaded = qt->getNumberOfPoints();

								fileopener->setlidardata(qt, bucketlimit);

								if (numberofpointsloaded == 0)
								{
									std::cout << "No points loaded from file, possibly because of fence." << std::endl;
								}
								newQuadtree = true;
								qt = NULL;
							}
							else
							{
								int numberofpointsloaded = fileopener->lidardata->load(loader, point_offset, 0, fence.getXs(), fence.getYs(), 4);

								if (numberofpointsloaded == 0)
								{
									std::cout << "No points loaded from file, possibly because of fence." << std::endl;
								}
							}

							if (loader->getMinZ() < fileopener->minZ || i == 0)
								fileopener->minZ = loader->getMinZ();
							if (loader->getMaxZ() > fileopener->maxZ || i == 0)
								fileopener->maxZ = loader->getMaxZ();

							fileopener->utm_zone = loader->get_utm_zone();
						}

					}
				}
				fileopener->thread_message = filename;
				sig_file_loaded();
				sig_message();
			}
		}
		else
		{
			double minx=0, maxx=0, miny=0, maxy=0;

			if (create_new_quadtree || !fileopener->loadedanyfiles || fileopener->lidardata == NULL)
			{
				for (size_t i = 0; i < filenames.size(); ++i)
				{
					std::string filename = filenames[i];

					if (filename != "")
					{
						LasLoader* loader = 0;

						// for .LAS files
						if (filename.find(".las", filename.length()-4) != std::string::npos || filename.find(".LAS", filename.length()-4) != std::string::npos)
						{
							loader = new LasLoader(filename.c_str());
						}
						// ASCII files
						else if (filename.find(".txt", filename.length()-4) != std::string::npos || filename.find(".TXT", filename.length()-4) != std::string::npos)
						{
							if (use_default_scale_factors)
							{
								loader = new LasLoader(filename.c_str(), ascii_code.c_str());
							}
							else
							{
								loader = new LasLoader(filename.c_str(), ascii_code.c_str(), scale_factor);
							}
						}
						// else invalid extension

						Boundary lidarboundary = loader->getBoundary();
						if (lidarboundary.minX < minx || i == 0)
							minx = lidarboundary.minX;
						if (lidarboundary.maxX > maxx || i == 0)
							maxx = lidarboundary.maxX;
						if (lidarboundary.minY < miny || i == 0)
							miny = lidarboundary.minY;
						if (lidarboundary.maxY > maxy || i == 0)
							maxy = lidarboundary.maxY;

						if (loader->getMinZ() < fileopener->minZ || i == 0)
							fileopener->minZ = loader->getMinZ();
						if (loader->getMaxZ() > fileopener->maxZ || i == 0)
							fileopener->maxZ = loader->getMaxZ();

						fileopener->utm_zone = loader->get_utm_zone();
					}
				}
			}

			for (size_t i = 0; i < filenames.size(); ++i)
			{
				std::string filename = filenames[i];

				if (filename != "")
				{
					LasLoader* loader = 0;

					// for .LAS files
					if (filename.find(".las", filename.length()-4) != std::string::npos || filename.find(".LAS", filename.length()-4) != std::string::npos)
					{
						loader = new LasLoader(filename.c_str());
					}
					// ASCII files
					else if (filename.find(".txt", filename.length()-4) != std::string::npos || filename.find(".TXT", filename.length()-4) != std::string::npos)
					{
						if (use_default_scale_factors)
						{
							loader = new LasLoader(filename.c_str(), ascii_code.c_str());
						}
						else
						{
							loader = new LasLoader(filename.c_str(), ascii_code.c_str(), scale_factor);
						}
					}
					// else invalid extension

					// If refreshing
					if ((i == 0 && (create_new_quadtree || !fileopener->loadedanyfiles)) || fileopener->lidardata == NULL)
					{
						Quadtree* qt = new Quadtree(minx, miny, maxx, maxy, bucketlimit, cachelimit, bucketLevels,
								resolutionbase, resolutiondepth);

						int numberofpointsloaded = qt->load(loader,point_offset, bucketLevels);
						std::cout << "LoadWorker: loaded points: " << numberofpointsloaded << std::endl;

						fileopener->setlidardata(qt, bucketlimit);

						if (numberofpointsloaded == 0)
						{
							std::cout << "No points loaded from file, possibly because of fence." << std::endl;
						}
						newQuadtree = true;
						qt = NULL;

					}
					else
					{
						int numberofpointsloaded = fileopener->lidardata->load(loader, point_offset, bucketLevels);
						if (numberofpointsloaded == 0)
						{
							std::cout << "No points loaded from file, possibly because of fence." << std::endl;
						}

					}
					if (loader->getMinZ() < fileopener->minZ || i == 0)
						fileopener->minZ = loader->getMinZ();
					if (loader->getMaxZ() > fileopener->maxZ || i == 0)
						fileopener->maxZ = loader->getMaxZ();

					fileopener->utm_zone = loader->get_utm_zone();

					if (loader)
						delete loader;
				}
				fileopener->thread_message = filename;
				sig_file_loaded();
				sig_message();
			}
		}
	}
	catch(DescriptiveException e)
	{
		std::cout << "LoadWorker: There has been an exception:\n";
		std::cout << e.what() << "\n" << e.why() << std::endl;
		if (fileopener->lidardata != NULL)
			delete fileopener->lidardata;
		fileopener->loadedanyfiles = false;
		newQuadtree = false;
		return;
	}

	fileopener->newQuadtree = newQuadtree;
	sig_done();
}


