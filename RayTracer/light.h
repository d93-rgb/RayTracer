#pragma once
#include <glm.hpp>

struct Light
{
	glm::vec3 p;
	glm::vec3 dir;
	glm::vec3 col;
	float intensity;

	Light(glm::vec3 p, glm::vec3 dir, glm::vec3 col)
	{
		this->p = p;
		this->dir = glm::normalize(dir);
		this->col = col;
	}
};