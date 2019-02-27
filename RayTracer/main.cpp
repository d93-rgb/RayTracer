#define _USE_MATH_DEFINES

// use for debugging
#define DEBUG

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
#include "renderer.h"
#include "light.h"
#include "scene.h"
#include "gui.h"

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;

float eps = 1e-3f;
int MAX_DEPTH = 4;

std::vector<float> debug_vec;

void write_file(const char *file, std::vector<glm::vec3> &col);

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

void render()
{
	float fov = deg2rad(65.f);
	float fov_tan = tan(fov / 2);
	float u = 0, v = 0;
	float d = 1;
	float rot_ang = deg2rad(0.f);
	std::vector<glm::vec3> col(WIDTH * HEIGHT);

	glm::vec3 ro = glm::vec3(0, 0, 14);
	glm::vec3 rd = glm::vec3(0, 0, -1);

	glm::vec3 x_dir = glm::vec3(1, 0, 0);
	glm::vec3 y_dir = glm::vec3(0, 1, 0);
	glm::vec3 z_dir = glm::vec3(0, 0, 1);
	glm::vec3 s;

	Ray ray = Ray(ro, rd);
	glm::vec3 def_col = glm::vec3(0.2, 0.2, 0.2);

	std::shared_ptr<Material> ob_mat;
	Object *ob = nullptr;

	/***************************************/
	// CREATING SCENE
	/***************************************/
	Scene sc;

	/***************************************/
	// LOOPING OVER PIXELS
	/***************************************/
	// transform camera origin to world coordinates
	transform_camera_ro(ray.ro, rot_ang);

	for (int y = 0, i = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			u = (2 * (float)(x + 0.5) - WIDTH) / HEIGHT * fov_tan;
			v = (-2 * (float)(y + 0.5) + HEIGHT) / HEIGHT * fov_tan;

			s = u * x_dir + v * y_dir - d * z_dir;

			transform_camera_rd(s, rot_ang);
			ray.rd = glm::normalize(s);
			
			col[i] = shoot_recursively(sc, ray, &ob, 0);
			
			// progress to next pixel
			++i;
		}
	}

	write_file("picture.ppm", col);

}

void write_file(const char *file, std::vector<glm::vec3> &col)
{
	std::ofstream ofs;

	/***************************************/
	// WRITING TO IMAGE FILE
	/***************************************/
	ofs.open(file, _IOSbinary);
	// don't use \n as ending white space, because of windows
	ofs << "P6 " << WIDTH << " " << HEIGHT << " 255 ";

	// write to image file
	for (int i = 0; i < WIDTH * HEIGHT; ++i)
	{
		col[i] = glm::min(glm::vec3(1), col[i]) * 255.f;

		unsigned char r = (unsigned int)round(col[i].x);
		unsigned char g = (unsigned int)round(col[i].y);
		unsigned char b = (unsigned int)round(col[i].z);

		ofs << r << g << b;
	}

	ofs.close();

	std::cout << "Done creating image." << std::endl;
}

int main(int argc, const char **agrv)
{
	//for (int i = 0; i < 3; ++i)
	//{
	//	for (int j = 0; j < 3; ++j)
	//	{
	//		std::cout << mat3[i][j] << " ";
	//	}
	//	std::cout << std::endl;
	//}	

	/*
		Gui g = Gui();
		g.init();
	*/

	/*
	ofs.open("debug.txt");
	for (float f : debug_vec)
	{
		ofs << f << std::endl;
	}
	ofs.close();
	*/
	//getchar();
	render();

	return 0;
}