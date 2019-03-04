#pragma once
#include <vector>
#include <memory>

#include <gtc\matrix_transform.hpp>

#include "object.h"
#include "light.h"
#include "camera.h"

struct Scene
{
	std::vector<std::unique_ptr<Object>> sc;
	std::vector<std::unique_ptr<Light>> lights;

	Camera cam;

	Scene() 
	{
		init();
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

	void init();

	~Scene() = default;
};

class GatheringScene : public Scene
{

};