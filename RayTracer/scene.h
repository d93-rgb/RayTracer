#pragma once
#include <vector>
#include "object.h"

struct Scene
{
	std::vector<Object*> sc;

	Scene() = default;

	void emplace_back(Object *o)
	{
		sc.emplace_back(o);
	}

	std::vector<Object*> get_scene()
	{
		return sc;
	}
};
