#pragma once
#include <glm.hpp>

struct Ray
{
	glm::vec3 ro;
	glm::vec3 rd;

	Ray() : ro(0), rd(0) {}

	Ray(glm::vec3 ro, glm::vec3 rd) :
		ro(ro), rd(rd) {}
};