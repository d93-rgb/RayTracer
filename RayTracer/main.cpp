#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include <math.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>

#include "object.h"
#include "light.h"
#include "scene.h"
#include "gui.h"

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;

std::vector<float> debug_vec;

float deg2rad(float deg) { return deg * (float)M_PI / 180; }
float rad2deg(float rad) { return rad * 180 / (float)M_PI; }

void print_vec3(glm::vec3 v)
{
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

void transform_camera_ro(glm::vec3 &ro, float rot_angle)
{
	ro = glm::rotate(glm::mat4(1.0f), rot_angle, glm::vec3(0, 1, 0)) * glm::vec4(ro, 0.0f);
	/*return glm::mat3x3(glm::vec3(cos(rot_angle), 0, -sin(rot_angle)),
		glm::vec3(0, 1, 0),
		glm::vec3(sin(rot_angle), 0, cos(rot_angle))) * pos+translation;*/
}

void transform_camera_rd(glm::vec3 &rd, float rot_angle)
{
	rd = glm::rotate(glm::mat4(1.0f), rot_angle, glm::vec3(0, 1, 0)) * glm::vec4(rd, 0.0f);
	/*return glm::mat3x3(glm::vec3(cos(rot_angle), 0, -sin(rot_angle)),
		glm::vec3(0, 1, 0),
		glm::vec3(sin(rot_angle), 0, cos(rot_angle))) * pos+translation;*/
}

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
	glm::vec3 col = obj->mat.kd * glm::max(0.f, glm::dot(obj->get_normal(ob_pos), -light.dir));
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

bool calc_shadow(glm::vec3 p, Scene sc, Light light)
{
	float t_int = INFINITY;
	float tmp;
	float eps = 1e-3f;
	
	Ray ray = Ray(p, -light.dir);
	ray.ro += ray.rd * eps;

	for (Object *objs : sc.get_scene())
	{
		tmp = objs->intersect(ray);

		if (tmp >= 0 && t_int > tmp)
		{
			t_int = tmp;
		}
	}
	// no intersection found
	if (t_int < 0 || t_int == INFINITY) return true;

	return false;
}

int main(void)
{
	char var = 65;
	std::ofstream ofs;

	float fov = deg2rad(65.f);
	float fov_tan = tan(fov / 2);

	glm::vec3 ro = glm::vec3(0, 0, 0);
	glm::vec3 rd = glm::vec3(0, 0, -1);

	DistantLight dist_light = DistantLight(glm::vec3(-2, -4, -1), glm::vec3(0.8f));

	glm::vec3 translation = glm::vec3(0, 0, -15);
	float rot_ang = (float)0;

	Ray ray = Ray(ro, rd);

	// DEBUGGING
	//glm::mat3 mat3 = glm::mat3(translation, glm::vec3(1), glm::vec3(1, 2, 3));

	//for (int i = 0; i < 3; ++i)
	//{
	//	for (int j = 0; j < 3; ++j)
	//	{
	//		std::cout << mat3[i][j] << " ";
	//	}
	//	std::cout << std::endl;
	//}	

	/***************************************/
	// CREATING SCENE
	/***************************************/
	Scene sc;
	
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
	glm::vec3 def_col = glm::vec3(0.2, 0.2, 0.2);

	std::vector<glm::vec3> col(WIDTH * HEIGHT);

	Object *ob = nullptr;
	

/*
	Gui g = Gui();
	g.init();
*/

	// transform camera origin to world coordinates
	transform_camera_ro(ray.ro, rot_ang);
	for (int y = 0, i = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			u = (2 * (float)(x + 0.5) - WIDTH) / HEIGHT * fov_tan;
			v = (-2 * (float)(y + 0.5) + HEIGHT)/ HEIGHT * fov_tan;

			s = u * x_dir + v * y_dir - d * z_dir;

			transform_camera_rd(s, rot_ang);
			ray.rd = glm::normalize(s);

			t_int = INFINITY;
			for (Object *objs : sc.get_scene())
			{
				tmp = objs->intersect(ray);

				if (tmp >= 0 && t_int > tmp)
				{
					t_int = tmp;

					ob = objs;
					//col[i] = sphs.color * glm::max(0.f, glm::dot(-rd, sphs.get_normal(inters_p)));
					//std::cout << col[i].x << " " << col[i].y << " " << col[i].z << std::endl;
				}
			}
			// no intersection found
			if (t_int < 0 || t_int == INFINITY)
			{
				col[i] = def_col;
			}
			else
			{
				// get intersection point
				inters_p = ray.ro + t_int * ray.rd;

				col[i] = ob->mat.ka + diff_shade(ob, inters_p, dist_light);
				col[i] += spec_shade(ob, inters_p, dist_light, ray.rd);

				col[i] *= calc_shadow(inters_p, sc, dist_light);
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