#pragma once
#include "rt.h"

namespace rt
{
class TextureMapping
{
public:
	virtual glm::vec2 getTextureCoordinates(glm::vec3 pos) = 0;

};

class SphericalMapping : public TextureMapping
{
	glm::vec3 center;
public:
	SphericalMapping(glm::vec3 center) : center(center)
	{
	}

	glm::vec2 getTextureCoordinates(glm::vec3 pos)
	{
		glm::vec3 pos_shift = glm::normalize(pos - center);
		//float radius = glm::length(pos_shift);
		float u = (1 + atan2f(pos_shift.z, pos_shift.x) / (float)M_PI) * 0.5f;
		float v = acosf(pos_shift.y) / (float)M_PI;

		return glm::vec2(u, v);
	}
};

class PlanarMapping : public TextureMapping
{
public:
	/*
		pos: plane position
		vs: spanning vector
		vt: (non parallel to vs) spanning vector
	*/
	PlanarMapping(glm::vec3 pos, glm::vec3 vs, glm::vec3 vt) : pos(pos), vs(s), vt(vt)
	{

	}

	glm::vec2 getTextureCoordinates(glm::vec3 pos)
	{
		float s = glm::dot(pos - this->pos, vs);
		float t = glm::dot(pos - this->pos, vt);

		return glm::vec2(s, t);
	}

private:
	glm::vec3 pos, vs, vt;
};

} // namespace rt