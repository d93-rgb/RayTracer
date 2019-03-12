#pragma once
#include "rt.h"

constexpr auto NUM = 0.2f;
constexpr auto GAP = NUM / 2.f;

namespace rt
{
inline glm::vec3 operator*(int n, const glm::vec3 &v)
{
	return glm::vec3(n * v.x, n * v.y, n * v.z);
}

class Texture
{
public:
	virtual glm::vec3 getTexel(glm::vec2 uv, glm::vec3 color) = 0;

};

class CheckerBoardTexture : public Texture
{
public:
	glm::vec3 getTexel(glm::vec2 uv, glm::vec3 color)
	{
		return ((fmodf(uv.x, NUM) < GAP) ^ (fmodf(uv.y, NUM) < GAP)) * color;
	}

	glm::vec3 getTexel(glm::vec2 uv, glm::vec3 color_1, glm::vec3 color_2)
	{
		return ((fmodf(uv.x, NUM) < GAP) ^ (fmodf(uv.y, NUM) < GAP)) ? color_1 : color_2;
	}

};

} // namespace rt