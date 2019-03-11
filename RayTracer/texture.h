#pragma once
#include "rt.h"

constexpr auto NUM = 2;
constexpr auto GAP = 1.f / NUM;

glm::vec3 operator*(int n, const glm::vec3 &v)
{
	return glm::vec3(n * v.x, n * v.y, n * v.z);
}

class Texture
{

};

class CheckerBoardTexture : public Texture
{
	glm::vec3 getTextureCoord(glm::vec2 uv, glm::vec3 color)
	{
		return ((fmodf(uv.x, NUM) < GAP) ^ (fmodf(uv.y, NUM) < GAP)) * color;
	}
};