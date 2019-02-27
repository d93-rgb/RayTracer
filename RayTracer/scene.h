#pragma once
#include <vector>
#include <memory>

#include "object.h"
#include "light.h"

struct Scene
{
	std::vector<std::unique_ptr<Object>> sc;
	std::vector<std::unique_ptr<Light>> lights;

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

