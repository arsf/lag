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
		std::string ascii_code, SelectionBox fence) : Worker(),
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
		fence				(fence)
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

void LoadWorker::show_message(std::string message)
{
	fileopener->set_thread_message(message);
	sig_message();
}


void LoadWorker::run()
{
	// Has a new quadtree been created?
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
				show_message(filename);

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
							show_message("Incorrect file type.");
							sig_fail();
							return;
						}

						if (validfile)
						{
							// If refreshing
							if ((i == 0 && (create_new_quadtree || !fileopener->get_loadedanyfiles())) || fileopener->get_lidardata() == NULL)
							{
								fileopener->delete_lidardata();

								Quadtree* qt = new Quadtree(loader, bucketlimit, point_offset, fence.getXs(), fence.getYs(), 4, cachelimit, 0, resolutionbase, resolutiondepth);
								int numberofpointsloaded = qt->getNumberOfPoints();

								fileopener->set_lidardata(qt);

								if (numberofpointsloaded == 0)
								{
									std::cout << "No points loaded from file, possibly because of fence." << std::endl;
									show_message("No points loaded from file.");
								}
								newQuadtree = true;
								qt = NULL;
							}
							else
							{
								int numberofpointsloaded = fileopener->get_lidardata()->load(loader, point_offset, 0, fence.getXs(), fence.getYs(), 4);

								if (numberofpointsloaded == 0)
								{
									std::cout << "No points loaded from file, possibly because of fence." << std::endl;
									show_message("No points loaded from file.");
								}
							}

							if (loader->getMinZ() < fileopener->get_minZ() || i == 0)
								fileopener->set_minZ(loader->getMinZ());
							if (loader->getMaxZ() > fileopener->get_maxZ() || i == 0)
								fileopener->set_maxZ(loader->getMaxZ());

							fileopener->set_utm_zone(loader->get_utm_zone());
						}
					}
				}
				sig_file_loaded();
			}
		}
		else
		{
			double minx=0, maxx=0, miny=0, maxy=0;

			if (create_new_quadtree || !fileopener->get_loadedanyfiles() || fileopener->get_lidardata() == NULL)
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
						else
						{
							std::cout << "LoadWorker: Files must have an extension .las or .txt." << std::endl;
							show_message("Incorrect file type.");
							sig_fail();
							return;
						}

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
				show_message(filename);

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
					else
					{
						std::cout << "LoadWorker: Files must have an extension .las or .txt." << std::endl;
						show_message("Incorrect file type.");
						sig_fail();
						return;
					}

					// If refreshing
					if ((i == 0 && (create_new_quadtree || !fileopener->get_loadedanyfiles())) || fileopener->get_lidardata() == NULL)
					{
						Quadtree* qt = new Quadtree(minx, miny, maxx, maxy, bucketlimit, cachelimit, bucketLevels,
								resolutionbase, resolutiondepth);

						int numberofpointsloaded = qt->load(loader,point_offset, bucketLevels);

						fileopener->set_lidardata(qt);

						if (numberofpointsloaded == 0)
						{
							std::cout << "No points loaded from file, possibly because of fence." << std::endl;
							show_message("No points loaded from file.");
						}
						newQuadtree = true;
						qt = NULL;

					}
					else
					{
						int numberofpointsloaded = fileopener->get_lidardata()->load(loader, point_offset, bucketLevels);
						if (numberofpointsloaded == 0)
						{
							std::cout << "No points loaded from file, possibly because of fence." << std::endl;
							show_message("No points loaded from file.");
						}

					}
					if (loader->getMinZ() < fileopener->get_minZ() || i == 0)
						fileopener->minZ = loader->getMinZ();
					if (loader->getMaxZ() > fileopener->get_maxZ() || i == 0)
						fileopener->maxZ = loader->getMaxZ();

					fileopener->set_utm_zone(loader->get_utm_zone());

					if (loader)
						delete loader;
				}
				sig_file_loaded();
			}
		}
	}
	catch(DescriptiveException e)
	{
		std::cout << "LoadWorker: Error loading file.\n";
		std::cout << e.what() << "\n" << e.why() << std::endl;
		show_message(e.why());

		fileopener->delete_lidardata();

		fileopener->set_loadedanyfiles(false);
		newQuadtree = false;

		sig_fail();

		return;
	}

	fileopener->set_newQuadtree(newQuadtree);
	sig_done();
}


