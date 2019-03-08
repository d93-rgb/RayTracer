#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

#include <Windows.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <random>

#include <math.h>
#include <omp.h>

#include <glm.hpp>

#include "rt.h"
#include "object.h"
#include "renderer.h"
#include "light.h"
#include "scene.h"
#include "gui.h"
#include "camera.h"

// use for debugging
#undef DEBUG
#define OPEN_WITH_GIMP

using namespace rt;

constexpr auto SPP = 1;
constexpr auto GRID_DIM = 1;

constexpr auto WIDTH = 533;
constexpr auto HEIGHT = 400;

int MAX_DEPTH = 4;

std::vector<float> debug_vec;

float clamp(float f)
{
	return f < 0.f ? 0.f : (f > 1.f ? 1.f : f);
}

glm::vec3 clamp(glm::vec3 v)
{
	return glm::min(glm::vec3(1.f), glm::max(glm::vec3(0.f), v));
}

void write_file(const char *file, std::vector<glm::vec3> &col);

//float deg2rad(float deg) { return deg * (float)M_PI / 180; }
//float rad2deg(float rad) { return rad * 180 / (float)M_PI; }

std::ostream &operator<<(std::ostream &os, glm::vec3 v)
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
	return os;
}

/*
	Starts rendering a scene and writes the pixels to a .ppm file
*/
void render()
{
	int i = 0;
	float fov = glm::radians(55.f);
	float fov_tan = tan(fov / 2);
	float u = 0.f, v = 0.f;
	// distance to view plane
	float d = 1.f;
	float inv_spp = 1.f / SPP;

	glm::vec3 radiance = glm::vec3(0.f);

	std::vector<glm::vec3> col(WIDTH * HEIGHT, glm::vec3(0.f));

	Object *ob = nullptr;

	/***************************************/
	// CREATING SCENE
	/***************************************/
	//GatheringScene sc;
	SingleCubeScene sc;

	/***************************************/
	// LOOPING OVER PIXELS
	/***************************************/
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<> dist(0, 1);
	// dynamic schedule for proper I/O progress update
#pragma omp parallel for schedule(dynamic, 1)
	for (int y = 0; y < HEIGHT; ++y)
	{
		fprintf(stderr, "\rRendering %5.2f%%", 100.*y / (HEIGHT - 1));
		Object *ob = nullptr;
		for (int x = 0; x < WIDTH; ++x)
		{
			for (int m = 0; m < GRID_DIM; ++m)
			{
				for (int n = 0; n < GRID_DIM; ++n)
				{
					// hackery needed for omp pragma
					// the index i will be distributed among all threads
					// by omp automatically
					for (int k = 0, i = y * WIDTH + x; k < SPP; ++k)
					{
						// TODO: Consider changing random values to the range [0,1)
						float u_rnd = 2 * float(dist(eng)) - 1;
						float v_rnd = 2 * float(dist(eng)) - 1;
						// map pixel coordinates to[-1, 1]x[-1, 1]
						float u = (2.f * (x + (m + 0.5f + u_rnd) / 2.f) - WIDTH) / HEIGHT * fov_tan;
						float v = (-2.f * (y + (n + 0.5f + v_rnd) / 2.f) + HEIGHT) / HEIGHT * fov_tan;

						// this can not be split up and needs to be in one line, otherwise
						// omp will not take the average
						col[i] += clamp(shoot_recursively(sc, sc.cam.getPrimaryRay(u, v, d), &ob, 0)) * inv_spp * 0.25f;
					}
				}
			}
		}
	}

	//#pragma omp parallel for
	//	for (int i = 0; i < 10; ++i)
	//	{
	//		std::this_thread::sleep_for(std::chrono::seconds(1));
	//		std::cout << " thread: " << omp_get_thread_num() << std::endl;
	//	}

	write_file("picture.ppm", col);

}

void write_file(const char *file, std::vector<glm::vec3> &col)
{
	static int i_debug = 0;
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
		// gamma correction and mapping to [0;255]
		col[i] = glm::pow(glm::min(glm::vec3(1), col[i]),
			glm::vec3(1 / 2.2f)) * 255.f;

#ifdef DEBUG
		i_debug = (++i_debug) % 3000;
#endif

		// prevent sign extension by casting to unsigned int
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

// launch rendering 
	render();

#ifdef OPEN_WITH_GIMP
	// OPEN FILE IN GIMP
	std::string gimp_path = "C:\\Program Files\\GIMP 2\\bin\\gimp-2.10.exe";
	std::string image_path = "C:\\Users\\Dood\\source\\repos\\RayTracer\\RayTracer\\picture.ppm";
	std::string szCmdline = gimp_path + " " + image_path;

	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	if (!CreateProcess(nullptr,   // the path
		&szCmdline[0],        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi))            // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	{
		printf("CreateProcess failed (%d).\n%s\n",
			GetLastError(), szCmdline.c_str());
}
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
#endif

	return 0;
}