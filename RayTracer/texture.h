#pragma once
#include "rt.h"
#include "texturemapping.h"
#include "shape.h"

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
	virtual glm::vec3 getTexel(glm::vec3 pos) = 0;
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

	glm::vec3 getTexel(glm::vec3 pos)
	{
		glm::vec2 uv = tm->getTextureCoordinates(pos);
		return ((fmodf(uv.x, NUM) < GAP) ^ (fmodf(uv.y, NUM) < GAP)) * color;
	}

	glm::vec3 getTexel(glm::vec3 pos, glm::vec3 color_1, glm::vec3 color_2)
	{
		glm::vec2 uv = tm->getTextureCoordinates(pos);
		return ((fmodf(uv.x, NUM) < GAP) ^ (fmodf(uv.y, NUM) < GAP)) ? color_1 : color_2;
	}

};

// friend class texture of triangle
class RGB_TextureTriangle : public Texture
{
public:
	RGB_TextureTriangle(Triangle *tr) : tr(tr)
	{
	}
	
	glm::vec3 getTexel(glm::vec3 pos);

private:
	Triangle *tr;
};

} // namespace rt