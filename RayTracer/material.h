#pragma once
#include <glm.hpp>

struct Material
{
	glm::vec3 ka, kd, ks;

	Material() : ka(0), kd(0), ks(0) {}

	Material(glm::vec3 amb, glm::vec3 dif, glm::vec3 spe)
		: ka(amb), kd(dif), ks(spe) {}
};