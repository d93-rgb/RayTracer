#pragma once
#include <glm.hpp>

#include "object.h"

struct Scene;

extern float eps;

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

	virtual glm::vec3 diff_shade(const Object &obj,
		const glm::vec3 &ob_pos) = 0;

	virtual glm::vec3 spec_shade(const Object &obj,
		const glm::vec3 &ob_pos,
		const glm::vec3 &view_dir) = 0;

	virtual bool calc_shadow(glm::vec3 p, const Scene &sc) = 0;

	virtual glm::vec3 phong_shade(const Scene & sc, 
		const Ray & ray, 
		const glm::vec3 & ob_pos, 
		const Object * o) = 0;

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

	glm::vec3 diff_shade(const Object & obj, const glm::vec3 & ob_pos);
	glm::vec3 spec_shade(const Object & obj, const glm::vec3 & ob_pos, const glm::vec3 & view_dir);
	bool calc_shadow(glm::vec3 p, const Scene &sc);
	glm::vec3 phong_shade(const Scene & sc, const Ray & ray, const glm::vec3 & ob_pos, const Object * o);
};

struct PointLightShaped : public Light
{
	PointLightShaped(glm::vec3 p, glm::vec3 dir, glm::vec3 col) :
		Light(p, dir, col) {}

	// equal light emission in all directions
	glm::vec3 getEmission(glm::vec3 dir) const
	{
		return emission;
	}

	glm::vec3 diff_shade(const Object & obj, const glm::vec3 & ob_pos);
	glm::vec3 spec_shade(const Object & obj, const glm::vec3 & ob_pos, const glm::vec3 & view_dir);
	bool calc_shadow(glm::vec3 p, const Scene &sc);
	glm::vec3 phong_shade(const Scene & sc, const Ray & ray, const glm::vec3 & ob_pos, const Object * o);

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
	glm::vec3 diff_shade(const Object & obj, const glm::vec3 & ob_pos);
	glm::vec3 spec_shade(const Object & obj, const glm::vec3 & ob_pos, const glm::vec3 & view_dir);
	bool calc_shadow(glm::vec3 p, const Scene &sc);
	glm::vec3 phong_shade(const Scene & sc, const Ray & ray, const glm::vec3 & ob_pos, const Object * o);
};