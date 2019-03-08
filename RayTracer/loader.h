#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

/*
	Read an .obj file and store the vertices and normals into two seperate containers.
	Return true, if everything went succesfully, false if file could not be read.
*/
bool loadObjFile(const std::string &file,
	std::vector<float> *vertices,
	std::vector<int> *indices)
{
	std::ifstream ifs{ file };
	std::istringstream iss;
	std::string line;

	char type;
	// values
	std::string v1, v2, v3;

	if (ifs.is_open())
	{
		while (std::getline(ifs, line))
		{
			if (!line.empty())
			{
				iss.str(line);
				iss.clear();

				iss >> type >> v1 >> v2 >> v3;

				if (type == 'v')
				{
					vertices->push_back(std::stof(v1));
					vertices->push_back(std::stof(v2));
					vertices->push_back(std::stof(v3));
				}
				else if (type == 'f')
				{
					indices->push_back(std::stoi(v1));
					indices->push_back(std::stoi(v2));
					indices->push_back(std::stoi(v3));
				}
			}
		}
	}
}