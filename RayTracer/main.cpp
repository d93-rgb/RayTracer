#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

#include <glm.hpp>

constexpr auto WIDTH = 800;
constexpr auto HEIGHT = 450;


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

	glm::vec3 ro = glm::vec3(0, 0, -7);
	glm::vec3 rd = glm::vec3(0, 0, 1);

	Ray ray =  Ray(ro, rd);
	
	glm::vec3 sph_or = glm::vec3(0, 0, 0);
	float radius = 1;

	float intersect = ray.intersect_sphere(sph_or, radius);
	
	

	std::cout << "intersect = " << intersect << std::endl;
	
	/***************************************/
	// LOOPING OVER PIXELS
	/***************************************/
	float u = 0, v = 0;
	float d = 1;
	float t_int;
	glm::vec3 x_dir = glm::vec3(1, 0, 0);
	glm::vec3 y_dir = glm::vec3(0, 1, 0);
	glm::vec3 z_dir = glm::vec3(0, 0, -1);
	glm::vec3 s;
	glm::vec3 def_col = glm::vec3(0, 0, 0);
	glm::vec3 sph_col = glm::vec3(127, 0, 0);

	std::vector<glm::vec3> col(WIDTH * HEIGHT);
	
	for (int y = 0, i = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			u = -1 + 2 * (float)x / WIDTH;
			v = -1 + 2 * (float)y / HEIGHT;

			s = u * x_dir + v * y_dir - d * z_dir;

			ray.rd = glm::normalize(s);

			t_int = ray.intersect_sphere(sph_or, radius);

			if (t_int >= 0 && t_int != INFINITY)
			{
				col[i++] = sph_col;
			}
			else {
				col[i++] = def_col;
			}
		}
	}
	
	
	
	
	
	/***************************************/
	// WRITING TO IMAGE FILE
	/***************************************/
	ofs.open("picture.ppm");
	// don't use \n as ending white space, because of windows
	ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255 ";

	// write to image file
	for (int i = 0; i < WIDTH * HEIGHT; ++i)
	{
		char r = col[i].x;
		char g = col[i].y;
		char b = col[i].z;

		ofs << r <<  g  << b;
	}

	ofs.close();

	std::cout << "Done creating image." << std::endl;

	//std::cout << (int)var << std::endl;
	getchar();
	
	return 0;
}