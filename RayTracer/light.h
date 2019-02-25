#pragma once
#include <glm.hpp>

struct Light
{
	glm::vec3 p;
	glm::vec3 dir;
	glm::vec3 emission;
	float power;

	Light(glm::vec3 p, glm::vec3 dir, glm::vec3 col)
	{
		this->p = p;
		this->dir = glm::normalize(dir);
		this->emission = col;
	}

	virtual glm::vec3 getEmission(glm::vec3 dir) const = 0;
};

struct PointLight : public Light
{
	PointLight(glm::vec3 p, glm::vec3 dir, glm::vec3 col) :
		Light(p, dir, col) {}

	// equal light emission in all directions
	glm::vec3 getEmission(glm::vec3 dir) const
	{
		return emission;
	}
};

struct SpotLight : public Light
{

};

//struct AreaLight : public Light
//{
//
//};

struct DistantLight : public Light
{
	DistantLight(glm::vec3 dir, glm::vec3 col) :
		Light(glm::vec3(INFINITY), dir, col)
	{}

	glm::vec3 getEmission(glm::vec3 dir) const
	{
		return emission;
	}
};