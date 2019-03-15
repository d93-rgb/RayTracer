#pragma once
#include "rt.h"
#include "texture.h"

namespace rt
{
struct Material
{
	glm::vec3 ambient, diffuse, specular, reflective, transparent;
	float refr_indx;

	std::shared_ptr<Texture> tex;

	Material() :
		ambient(0),
		diffuse(0),
		specular(0),
		reflective(0),
		transparent(0),
		n(1.f),
		refr_indx(1.f),
		tex(nullptr)
	{
	}

	Material(glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, std::shared_ptr<Texture> tex = nullptr)
		:
		ambient(amb),
		diffuse(dif),
		specular(spe),
		reflective(0),
		transparent(0),
		n(1.f),
		refr_indx(1.f)
	{
		this->tex = tex;
	}

	glm::vec3 getAmbient(glm::vec3 pos)
	{
		if (tex)
		{
			return 0.001f * tex->getTexel(tex->getTextureCoordinates(pos));
		}
		return ambient;
	}

	glm::vec3 getDiffuse(glm::vec3 pos)
	{
		if (tex)
		{
			return tex->getTexel(tex->getTextureCoordinates(pos));
		}
		return diffuse;
	}

	void setShininess(float exp)
	{
		n = exp;
	}

	float getShininess() const
	{
		return n;
	}

	void setRefractiveIdx(float f)
	{
		this->refr_indx = f;
	}

	void setTexture(std::shared_ptr<Texture> tex)
	{
		this->tex = tex;
	}

protected:
	// specular exponent
	float n;
};
}