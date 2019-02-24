#pragma once
#include <glm.hpp>

struct Light
{
	glm::vec3 p;
	glm::vec3 dir;
	glm::vec3 col;
	float power;

	Light(glm::vec3 p, glm::vec3 dir, glm::vec3 col)
	{
		this->p = p;
		this->dir = glm::normalize(dir);
		this->col = col;
	}
};

struct PointLight : public Light
{

};

struct SpotLight : public Light
{

};

struct AreaLight : public Light
{

};

struct DistantLight : public Light
{
	DistantLight(glm::vec3 dir, glm::vec3 col) :
		Light(glm::vec3(INFINITY), dir, col)
	{}
};