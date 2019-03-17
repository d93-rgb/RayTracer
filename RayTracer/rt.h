#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES
#define GLOG_NO_ABBREVIATED_SEVERITIES

#include <Windows.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <cmath>
#include <memory>

#include <math.h>
//#include <omp.h>

#include <glm/glm.hpp>
#include <glog/logging.h>

//#include "ray.h"
//#include "material.h"
//#include "object.h"
//#include "scene.h"
//#include "renderer.h"
//#include "light.h"
//#include "scene.h"
//#include "gui.h"
//#include "camera.h"
//#include "loader.h"

// from pbrt
#include "progressreporter.h"

#define PBRT_IS_WINDOWS

// uncomment if you want to shade the color according to the direction of surface normals
//#define DEBUG_NORMALS

namespace rt
{

struct Ray;
class Scene;
struct Shape;
struct Material;
class Interaction;
class SurfaceInteraction;
struct Light;
class Camera;

struct Plane;
struct Rectangle;
class Triangle;
class Cube;

struct Sphere;
struct Cylinder;
struct Disk;
struct Cone;
struct Paraboloid;
struct Hyperboloid;

class Bounds3;


class Texture;
class CheckerBoardTexture;
class RGB_TriangleTexture;

class SingleCubeScene;
class GatheringScene;

enum class ImageWrap;

static constexpr bool QUIET = false;

static constexpr float shadowEpsilon = 1e-3f;
}
