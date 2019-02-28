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

#include "object.h"
#include "renderer.h"
#include "light.h"
#include "scene.h"
#include "gui.h"
#include "camera.h"

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;

float eps = 7e-3f;
int MAX_DEPTH = 4;

std::vector<float> debug_vec;

void write_file(const char *file, std::vector<glm::vec3> &col);

//float deg2rad(float deg) { return deg * (float)M_PI / 180; }
//float rad2deg(float rad) { return rad * 180 / (float)M_PI; }

void print_vec3(glm::vec3 v)
{
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

void render()
{
	float fov = glm::radians(65.f);
	float fov_tan = tan(fov / 2);
	float u = 0.f, v = 0.f;
	// distance to view plane
	float d = 1.f;
	std::vector<glm::vec3> col(WIDTH * HEIGHT);
	
	Object *ob = nullptr;

	/***************************************/
	// CREATING SCENE
	/***************************************/
	Scene sc;

	/***************************************/
	// LOOPING OVER PIXELS
	/***************************************/
	for (int y = 0, i = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			u = (2 * (float)(x + 0.5) - WIDTH) / HEIGHT * fov_tan;
			v = (-2 * (float)(y + 0.5) + HEIGHT) / HEIGHT * fov_tan;
			
			col[i++] = shoot_recursively(sc, sc.cam.getPrimaryRay(u, v, d), &ob, 0);
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