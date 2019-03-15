#pragma once
#include "rt.h"
#include "texturemapping.h"

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
protected:
	std::shared_ptr<TextureMapping> tm;
public:
	Texture() = default;
	virtual glm::vec3 getTexel(glm::vec2 uv) = 0;
	virtual glm::vec2 getTextureCoordinates(glm::vec3 pos) = 0;

};

class CheckerBoardTexture : public Texture
{
	glm::vec3 color;
public:

	CheckerBoardTexture() = default;
	CheckerBoardTexture(std::shared_ptr<TextureMapping> texMap, glm::vec3 color = glm::vec3(1.f))
	{
		this->color = color;
		this->tm = texMap;
	}

	glm::vec2 getTextureCoordinates(glm::vec3 pos)
	{
		return tm->getTextureCoordinates(pos);
	}

	glm::vec3 getTexel(glm::vec2 uv)
	{
		return ((fmodf(uv.x, NUM) < GAP) ^ (fmodf(uv.y, NUM) < GAP)) * color;
	}

	glm::vec3 getTexel(glm::vec2 uv, glm::vec3 color_1, glm::vec3 color_2)
	{
		return ((fmodf(uv.x, NUM) < GAP) ^ (fmodf(uv.y, NUM) < GAP)) ? color_1 : color_2;
	}

};

} // namespace rt