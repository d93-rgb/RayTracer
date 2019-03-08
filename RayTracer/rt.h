#pragma once

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
#include <cmath>
#include <memory>

#include <math.h>
#include <omp.h>

#include <glm.hpp>

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

namespace rt
{

struct Ray;
class Scene;
struct Object;
struct Material;
struct Light;
class Camera;

struct Plane;
struct Rectangle;
class Triangle;
class Cube;

class SingleCubeScene;
class GatheringScene;

static constexpr bool QUIET = false;

static constexpr float shadowEpsilon = 1e-3f;
}
