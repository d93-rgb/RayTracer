#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <math.h>

#include <glm.hpp>

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;


float deg2rad(float deg) { return deg * M_PI / 180; }
float rad2deg(float rad) { return rad * 180 / M_PI; }

void print_vec3(glm::vec3 v)
{
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

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

struct Light
{
	glm::vec3 p;
	glm::vec3 dir;
	glm::vec3 col;

	Light(glm::vec3 p, glm::vec3 dir, glm::vec3 col)
	{
		this->p = p;
		this->dir = glm::normalize(dir);
		this->col = col;
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
		float term_2 = 2 * glm::dot(rd, ro-origin);
		float term_3 = glm::dot(ro-origin, ro-origin) - r * r;

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

/*
*	Calculate reflection vector.
*	N should be normalized.
*/
glm::vec3 reflect(glm::vec3 L, glm::vec3 N)
{
	return L - 2 * glm::dot(N, L) * N;
}


/*
*	Calculate diffuse shading of an object.
*/
glm::vec3 diff_shade(Sphere *obj, 
	glm::vec3 ob_pos, 
	Light &light)
{
	glm::vec3 l_dir = -light.dir;
	glm::vec3 col = obj->color * light.col * glm::max(0.f, glm::dot(obj->get_normal(ob_pos), l_dir));
	return col;
}

/*
*	Calculate specular shading of an object.
*/
glm::vec3 spec_shade(Sphere *obj, 
	glm::vec3 ob_pos, 
	Light &light,
	glm::vec3 view_dir)
{
	glm::vec3 refl = reflect(light.dir, obj->get_normal(ob_pos));
	refl = glm::normalize(refl);

	return glm::max(0.f, glm::dot(refl, -view_dir)) * light.col;
}

int main(void)
{
	char var = 65;
	std::ofstream ofs;

	float fov = deg2rad(80.f);
	float fov_tan = tan(fov / 2);

	glm::vec3 ro = glm::vec3(0, 0, 0);
	glm::vec3 rd = glm::vec3(0, 0, 1);

	Light dist_light = Light(glm::vec3(2, 200, 1), glm::vec3(0, 0, 2), glm::vec3(1));
	Ray ray =  Ray(ro, rd);
	
	glm::vec3 sph_or_1 = glm::vec3(-4, -2, 6);
	glm::vec3 sph_or_2 = glm::vec3(-4, 2, 8);
	glm::vec3 sph_or_3 = glm::vec3(4, 3, 9);
	float radius[] = { 1, 1.5 , 3};

	Sphere sph_1 = Sphere(sph_or_1, radius[0], glm::vec3(127, 0, 0));
	Sphere sph_2 = Sphere(sph_or_2, radius[1], glm::vec3(127, 127, 0));
	Sphere sph_3 = Sphere(sph_or_3, radius[2], glm::vec3(0, 0, 200));
	
	Sphere spheres[] = { sph_1, sph_2, sph_3 };

	/***************************************/
	// LOOPING OVER PIXELS
	/***************************************/
	float u = 0, v = 0;
	float d = 1;
	float t_int;
	float tmp;
	glm::vec3 x_dir = glm::vec3(1, 0, 0);
	glm::vec3 y_dir = glm::vec3(0, 1, 0);
	glm::vec3 z_dir = glm::vec3(0, 0, -1);
	glm::vec3 s;
	glm::vec3 inters_p;
	glm::vec3 def_col = glm::vec3(50, 50, 50);

	std::vector<glm::vec3> col(WIDTH * HEIGHT);
	
	for (int y = 0, i = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			u = (2 * (float)(x + 0.5) - WIDTH) / HEIGHT * fov_tan;
			v = (-2 * (float)(y + 0.5) + HEIGHT)/ HEIGHT * fov_tan;

			s = u * x_dir + v * y_dir - d * z_dir;

			ray.rd = glm::normalize(s);

			t_int = INFINITY;
			for (Sphere sphs : spheres)
			{
				tmp = sphs.intersect(ray.ro, ray.rd);

				if (tmp >= 0 && t_int > tmp)
				{
					t_int = tmp;
					// get intersection point
					inters_p = ray.ro + t_int * ray.rd;
					
					col[i] = 0.1f * sphs.color + diff_shade(&sphs, inters_p, dist_light);
					col[i] += spec_shade(&sphs, inters_p, dist_light, ray.rd);
					//col[i] = sphs.color * glm::max(0.f, glm::dot(-rd, sphs.get_normal(inters_p)));
					//std::cout << col[i].x << " " << col[i].y << " " << col[i].z << std::endl;
				}
			}
			// no intersection found
			if (t_int < 0 || t_int == INFINITY) col[i] = def_col;

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
	
	// wait 1s before closing
	std::this_thread::sleep_for(std::chrono::seconds(1));

	return 0;
}