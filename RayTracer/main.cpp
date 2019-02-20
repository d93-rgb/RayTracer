#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <math.h>

#include <glm.hpp>

constexpr auto WIDTH = 100;
constexpr auto HEIGHT = 100;

struct Object
{
	virtual float intersect(glm::vec3 &, glm::vec3 &) const = 0;
};

struct Ray
{
	glm::vec3 ro;
	glm::vec3 rd;

	Ray(glm::vec3 ro, glm::vec3 rd)
	{
		this->ro = ro;
		this->rd = rd;
	}
};

struct Sphere : public Object
{
	glm::vec3 origin;
	float r;
	glm::vec3 color;

	Sphere(glm::vec3 origin, float radius, glm::vec3 color)
	{
		this->origin = origin;
		this->r = radius;
		this->color = color;
	}

	glm::vec3 get_normal(glm::vec3 p)
	{
		return glm::normalize(p - origin);
	}

	float intersect(glm::vec3 &ro, glm::vec3 &rd) const override
	{
		float t1 = INFINITY, t2 = t1;
		float tmp;

		float term_1 = glm::dot(rd, rd);
		float term_2 = 2 * glm::dot(rd, ro);
		float term_3 = glm::dot(ro, ro) - r * r;

		float disc = term_2 * term_2 - 4 * term_1*term_3;

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
	
	glm::vec3 sph_or = glm::vec3(0, 0, 0);
	float radius = 1;

	Sphere sph_1 = Sphere(sph_or, radius, glm::vec3(127, 0, 0));
	Sphere sph_2 = Sphere(glm::vec3(-0.2, 0, 0), radius, glm::vec3(127, 127, 0));
	
	Sphere spheres[2] = { sph_1, sph_2 };

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

	std::vector<glm::vec3> col(WIDTH * HEIGHT);
	
	for (int y = 0, i = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			u = (2 * (float)(x) - WIDTH) / HEIGHT;
			v = (2 * (float)(y) - HEIGHT)/ HEIGHT;

			s = u * x_dir + v * y_dir - d * z_dir;

			ray.rd = glm::normalize(s);

			for (Sphere sphs : spheres)
			//for(int j = 0; j < sizeof(spheres)/sizeof(*spheres); ++j)
			{
				t_int = sphs.intersect(ray.ro, ray.rd);

				// get intersection point
				glm::vec3 inters_p = ray.ro + t_int * ray.rd;


				if (t_int >= 0 && t_int != INFINITY)
				{
					col[i] = sphs.color * glm::max(0.f, glm::dot(-rd, sphs.get_normal(inters_p)));
					//std::cout << col[i].x << " " << col[i].y << " " << col[i].z << std::endl;
					break;
				}
				else {
					col[i] = def_col;
				}
			}
			// progress to next pixel
			++i;
		}
	}
	
	
	
	
	
	/***************************************/
	// WRITING TO IMAGE FILE
	/***************************************/
	ofs.open("picture.ppm", _IOSbinary);
	// don't use \n as ending white space, because of windows
	ofs << "P6 " << WIDTH << " " << HEIGHT << " 255 ";

	// write to image file
	for (int i = 0; i < WIDTH * HEIGHT; ++i)
	{
		unsigned char r = (unsigned int)round(col[i].x);
		unsigned char g = (unsigned int)round(col[i].y);
		unsigned char b = (unsigned int)round(col[i].z);

		ofs << r <<  g  << b;
	}

	ofs.close();

	std::cout << "Done creating image." << std::endl;

	//std::cout << (int)var << std::endl;
	//getchar();
	
	// wait 3s before closing
	std::this_thread::sleep_for(std::chrono::seconds(3));

	return 0;
}