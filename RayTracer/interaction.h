#pragma once
#include "rt.h"
#include "material.h"

namespace rt
{
class Interaction
{

};

class SurfaceInteraction : public Interaction
{
public:
	glm::vec3 p;
	glm::vec3 normal;
	Material mat;
};
}