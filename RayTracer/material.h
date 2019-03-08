#pragma once
#include "rt.h"

namespace rt
{
struct Material
{
	glm::vec3 ambient, diffuse, specular, reflective, transparent;
	float refr_indx;

	Material() :
		ambient(0),
		diffuse(0),
		specular(0),
		reflective(0),
		transparent(0),
		n(1.f),
		refr_indx(1.f)
	{
	}

	Material(glm::vec3 amb, glm::vec3 dif, glm::vec3 spe)
		:
		ambient(amb),
		diffuse(dif),
		specular(spe),
		reflective(0),
		transparent(0),
		n(1.f),
		refr_indx(1.f)
	{
	}

	void setShininess(float exp)
	{
		n = exp;
	}

	float getShininess()
	{
		return n;
	}

	void setRefractiveIdx(float f)
	{
		this->refr_indx = f;
	}

protected:
	// specular exponent
	float n;
};
}