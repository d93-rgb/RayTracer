#pragma once
#include "rt.h"
#include "texture.h"

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
	{ }

	glm::vec2 getTextureCoordinates(glm::vec3 pos)
	{
		glm::vec3 pos_shift = glm::normalize(pos - center);
		//float radius = glm::length(pos_shift);
		float u = (1 + atan2f(pos_shift.z, pos_shift.x) / (float)M_PI) * 0.5f;
		float v = acosf(pos_shift.y) / (float)M_PI;

		return glm::vec2(u, v);
	}
};

} // namespace rt