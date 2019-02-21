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

std::vector<float> debug_vec;

float deg2rad(float deg) { return (float) deg * M_PI / 180; }
float rad2deg(float rad) { return (float) rad * 180 / M_PI; }

void print_vec3(glm::vec3 v)
{
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}


struct Material
{
	glm::vec3 ka, kd, ks;

	Material() : ka(0), kd(0), ks(0) {}

	Material(glm::vec3 amb, glm::vec3 dif, glm::vec3 spe)
		: ka(amb), kd(dif), ks(spe) {}
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


struct Object
{
	Material mat;

	Object() = default;

	virtual float intersect(Ray &ray) const = 0;

	virtual glm::vec3 get_normal(glm::vec3 p) = 0;
};

struct Light
{
	glm::vec3 p;
	glm::vec3 dir;
	glm::vec3 col;
	float intensity;

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

	Sphere(glm::vec3 origin, float radius, glm::vec3 color, Material m)
	{
		this->origin = origin;
		this->r = radius;
		this->color = color;
		this->mat = m;
	}

	glm::vec3 get_normal(glm::vec3 p)
	{
		return glm::normalize(p - origin);
	}

	float intersect(Ray &ray) const
	{
		float t1 = INFINITY, t2 = t1;
		float tmp;

		float term_1 = glm::dot(ray.rd, ray.rd);
		float term_2 = 2 * glm::dot(ray.rd, ray.ro-origin);
		float term_3 = glm::dot(ray.ro-origin, ray.ro-origin) - r * r;

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

struct Plane : public Object
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
	float k;
	
	Plane(glm::vec3 p, glm::vec3 n, glm::vec3 c, Material m) :
		pos(p), normal(glm::normalize(n)), color(c)
	{
		this->mat = m;
		k = glm::dot(normal, pos);
	}

	float intersect(Ray &ray) const
	{
		float denom = glm::dot(normal, ray.rd);

		if (abs(denom) < 1e-6) return INFINITY;

		float num = k - glm::dot(normal, ray.ro);

		float t = num / denom;

		//if(t >= 0) std::cout << t << std::endl;
		return t >= 0 ? t : INFINITY;
	}

	glm::vec3 get_normal(glm::vec3 p)
	{
		return normal;
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
glm::vec3 diff_shade(Object *obj, 
	glm::vec3 ob_pos, 
	Light &light)
{
	glm::vec3 l_dir = -light.dir;
	glm::vec3 col = obj->mat.kd * glm::max(0.f, glm::dot(obj->get_normal(ob_pos), l_dir));
	return col;
}

/*
*	Calculate specular shading of an object.
*/
glm::vec3 spec_shade(Object *obj, 
	glm::vec3 ob_pos, 
	Light &light,
	glm::vec3 view_dir)
{
	glm::vec3 refl = reflect(light.dir, obj->get_normal(ob_pos));
	refl = glm::normalize(refl);

	debug_vec.push_back(glm::max(0.f, glm::dot(refl, -view_dir)));

	return glm::max(0.f, glm::dot(refl, -view_dir)) * light.col * obj->mat.ks;
}

int main(void)
{
	char var = 65;
	std::ofstream ofs;

	float fov = deg2rad(75.f);
	float fov_tan = tan(fov / 2);

	glm::vec3 ro = glm::vec3(0, 0, 0);
	glm::vec3 rd = glm::vec3(0, 0, -1);

	Light dist_light = Light(glm::vec3(2, 200, 1), glm::vec3(5, 4, 0.5), glm::vec3(0.8f));
	Ray ray =  Ray(ro, rd);
	
	glm::vec3 sph_or_1 = glm::vec3(-4, -2, -10);
	glm::vec3 sph_or_2 = glm::vec3(-4, 2, -10);
	glm::vec3 sph_or_3 = glm::vec3(4, 3, -9);
	float radius[] = { 1, 1.5 , 3};

	Material m1 = Material(glm::vec3(0.1, 0, 0), glm::vec3(0.7, 0, 0), glm::vec3(0.3, 0, 0));
	Material m2 = Material(glm::vec3(0, 0.1, 0), glm::vec3(0, 0.7, 0), glm::vec3(0, 0.3, 0));
	Material m3 = Material(glm::vec3(0.1, 0, 0.1), glm::vec3(0.7, 0, 0.7), glm::vec3(0.7, 0, 0.7));
	Material m4 = Material(glm::vec3(0.1, 0.1, 0.1), glm::vec3(0.3, 0.3, 0), glm::vec3(0, 0, 0));

	Sphere sph_1 = Sphere(sph_or_1, radius[0], glm::vec3(0.5, 0, 0), m1);
	Sphere sph_2 = Sphere(sph_or_2, radius[1], glm::vec3(0.5, 0.5, 0), m2);
	Sphere sph_3 = Sphere(sph_or_3, radius[2], glm::vec3(0, 0, 0.8), m3);
	
	Plane pl_1 = Plane(glm::vec3(0, -25, 15), glm::vec3(0, 10, 5), glm::vec3(0.2), m4);

	Object *objects[] = { &sph_1, &sph_2, &sph_3, &pl_1 };
	
	/***************************************/
	// LOOPING OVER PIXELS
	/***************************************/
	float u = 0, v = 0;
	float d = 1;
	float t_int;
	float tmp;
	glm::vec3 x_dir = glm::vec3(1, 0, 0);
	glm::vec3 y_dir = glm::vec3(0, 1, 0);
	glm::vec3 z_dir = glm::vec3(0, 0, 1);
	glm::vec3 s;
	glm::vec3 inters_p;
	glm::vec3 def_col = glm::vec3(0.8, 0.8, 0.8);

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
			for (Object *objs : objects)
			{
				tmp = objs->intersect(ray);

				if (tmp >= 0 && t_int > tmp)
				{
					t_int = tmp;
					// get intersection point
					inters_p = ray.ro + t_int * ray.rd;
					
					col[i] = objs->mat.ka + diff_shade(objs, inters_p, dist_light);
					col[i] += spec_shade(objs, inters_p, dist_light, ray.rd);
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
		col[i] = glm::min(glm::vec3(1), col[i]) * 255.f;

		unsigned char r = (unsigned int)round(col[i].x);
		unsigned char g = (unsigned int)round(col[i].y);
		unsigned char b = (unsigned int)round(col[i].z);

		ofs << r <<  g  << b;
	}

	ofs.close();

	/*
	ofs.open("debug.txt");
	for (float f : debug_vec)
	{
		ofs << f << std::endl;
	}
	ofs.close();
	*/
	std::cout << "Done creating image." << std::endl;
	
	//std::cout << (int)var << std::endl;
	//getchar();
	
	// wait 1s before closing
	std::this_thread::sleep_for(std::chrono::seconds(1));

	return 0;
}