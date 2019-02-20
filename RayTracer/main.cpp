#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

#include <glm.hpp>

constexpr auto WIDTH = 10;
constexpr auto HEIGHT = 10;


struct Ray
{
	glm::vec3 ro;
	glm::vec3 rd;

	Ray(glm::vec3 ro, glm::vec3 rd)
	{
		this->ro = ro;
		this->rd = rd;
	}


	float intersect_sphere(glm::vec3 sph_origin, float r)
	{
		float t1 = INFINITY, t2 = t1;
		float tmp;

		float term_1 = rd.x*rd.x + rd.y*rd.y + rd.z*rd.z;
		float term_2 = 2 * (rd.x*ro.x + rd.y*ro.y + rd.z*ro.z);
		float term_3 = ro.x*ro.x + ro.y*ro.y + ro.z*ro.z - r * r;

		float disc = term_2*term_2 - 4 * term_1*term_3;
		
		if (disc < 0)
		{
			return INFINITY;
		}

		t1 = (-term_2 + sqrt(disc)) / (2 * term_1);
		t2 = (-term_2 - sqrt(disc)) / (2 * term_1);

		tmp = fmin(t1, t2);
		tmp = tmp >= 0 ? tmp : fmax(t1, t2);
		return tmp >= 0 ? tmp : INFINITY;
	}
};


int main(void)
{
	char var = 65;
	std::ofstream ofs;

	glm::vec3 ro = glm::vec3(0, 0, -5);
	glm::vec3 rd = glm::vec3(0, 0, 1);

	Ray ray =  Ray(ro, rd);
	float radius = 1;

	float intersect = ray.intersect_sphere(glm::vec3(0, 0, 0), radius);
	
	

	std::cout << "intersect = " << intersect << std::endl;

	ofs.open("picture.poppm");
	// don't use \n as ending white space, because of windows
	ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255 ";


	for (int i = 0; i < WIDTH * HEIGHT; ++i)
	{
		char r = 255 * ((i%3) == 0 ? 1 : 0);
		char g = 255 * ((i%3) == 1 ? 1 : 0);
		char b = 255 * ((i%3) == 2 ? 1 : 0);

		ofs << r <<  g  << b;
	}

	ofs.close();

	std::cout << "Done creating image." << std::endl;

	//std::cout << (int)var << std::endl;
	getchar();
	
	return 0;
}