#include "rt.h"
#include "renderer.h"
#include "scene.h"
#include "camera.h"
#include "utility.h"

// use for debugging
#undef DEBUG
#define OPEN_WITH_GIMP

using namespace rt;

constexpr auto SPP = 1;
constexpr auto GRID_DIM = 1;

constexpr auto WIDTH = 533;
constexpr auto HEIGHT = 400;

int MAX_DEPTH = 4;

//std::vector<float> debug_vec;

void write_file(const char *file, std::vector<glm::vec3> &col, int width, int height);

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
	int new_width, new_height;
	float fov = glm::radians(55.f);
	float fov_tan = tan(fov / 2);
	float u = 0.f, v = 0.f;
	// distance to view plane
	float d = 1.f;
	float inv_spp = 1.f / SPP;
	float inv_grid_dim = 1.f / (GRID_DIM * GRID_DIM);

	float crop_min_x = 0.f, crop_max_x = 1.f;
	float crop_min_y = 0.f, crop_max_y = 1.f;

	assert(crop_min_x <= crop_max_x && crop_min_y <= crop_max_y);

	int cropped_width[2];
	int cropped_height[2];
	glm::vec3 radiance = glm::vec3(0.f);

	crop(crop_min_x, crop_max_x, WIDTH, cropped_width);
	crop(crop_min_y, crop_max_y, HEIGHT, cropped_height);

	new_width = cropped_width[1] - cropped_width[0];
	new_height = cropped_height[1] - cropped_height[0];

	std::vector<glm::vec3> col(new_width * new_height, glm::vec3(0.f));

	/***************************************/
	// CREATING SCENE
	/***************************************/
	//GatheringScene sc;
	SingleCubeScene sc;
	// enclose with braces for destructor of ProgressReporter at the end of rendering
	{
		/***************************************/
		// START PROGRESSREPORTER
		/***************************************/
		pbrt::ProgressReporter reporter(HEIGHT, "Rendering:");
		/***************************************/
		// LOOPING OVER PIXELS
		/***************************************/
		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<> dist(0, 1);
		// dynamic schedule for proper I/O progress update
#pragma omp parallel for schedule(dynamic, 1)
		for (int y = cropped_height[0]; y < cropped_height[1]; ++y)
		{
			//fprintf(stderr, "\rRendering %5.2f%%", 100.*y / (HEIGHT - 1));
			reporter.Update();
			for (int x = cropped_width[0]; x < cropped_width[1]; ++x)
			{
				for (int m = 0; m < GRID_DIM; ++m)
				{
					for (int n = 0; n < GRID_DIM; ++n)
					{
						// hackery needed for omp pragma
						// the index i will be distributed among all threads
						// by omp automatically
						for (int k = 0,
							i = (y - cropped_height[0]) * new_width + x - cropped_width[0]; 
							k < SPP; ++k)
						{
							SurfaceInteraction isect;

							// TODO: Consider changing random values to the range [0,1)
							float u_rnd = 2 * float(dist(eng)) - 1;
							float v_rnd = 2 * float(dist(eng)) - 1;
							// map pixel coordinates to[-1, 1]x[-1, 1]
							float u = (2.f * (x + (m + 0.5f + u_rnd) / 2.f) - WIDTH) / HEIGHT * fov_tan;
							float v = (-2.f * (y + (n + 0.5f + v_rnd) / 2.f) + HEIGHT) / HEIGHT * fov_tan;

							// this can not be split up and needs to be in one line, otherwise
							// omp will not take the average
							col[i] += clamp(shoot_recursively(sc, sc.cam->getPrimaryRay(u, v, d), &isect, 0))
								* inv_spp * inv_grid_dim;
						}
					}
				}
			}
		}
		reporter.Done();
	}
	//#pragma omp parallel for
	//	for (int i = 0; i < 10; ++i)
	//	{
	//		std::this_thread::sleep_for(std::chrono::seconds(1));
	//		std::cout << " thread: " << omp_get_thread_num() << std::endl;
	//	}

	write_file("picture.ppm", col, new_width, new_height);

}

void write_file(const char *file, std::vector<glm::vec3> &col, int width, int height)
{
	static int i_debug = 0;
	std::ofstream ofs;

	/***************************************/
	// WRITING TO IMAGE FILE
	/***************************************/
	ofs.open(file, _IOSbinary);
	// don't use \n as ending white space, because of windows
	ofs << "P6 " << width << " " << height << " 255 ";

	// write to image file
	for (size_t i = 0; i < col.size(); ++i)
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

	std::cout << "Writing to file finished." << std::endl;
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