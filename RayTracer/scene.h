#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <memory>

#include <gtc\matrix_transform.hpp>
#include <gtx\perpendicular.hpp>

#include "object.h"
#include "light.h"
#include "camera.h"
#include "loader.h"

namespace rt
{

struct Scene
{
	std::vector<std::unique_ptr<Object>> sc;
	std::vector<std::unique_ptr<Light>> lights;

	Camera cam;

	Scene()
	{
	}

	/*void emplace_back(std::unique_ptr<Object> o)
	{
		sc.emplace_back(o);
	}
*/
	const std::vector<std::unique_ptr<Object>>& get_scene() const
	{
		return sc;
	}

	virtual void init() = 0;

	~Scene() = default;
};

class GatheringScene : public Scene
{
public:
	GatheringScene()
	{
		init();
	}

	void init();
};

class SingleCubeScene : public Scene
{
public:
	SingleCubeScene()
	{
		init();
	}

	void init();
};
}