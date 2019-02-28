#pragma once
#include <glm.hpp>
#include "ray.h"

class Camera
{
public:
	Camera() : origin(glm::vec4(0.f, 0.f, 0.f, 1.f)),
		right(glm::vec4(1.f, 0.f, 0.f, 0.f)),
		up(glm::vec4(0.f, 1.f, 0.f, 0.f)),
		front(glm::vec4(0.f, 0.f, 1.f, 0.f)) {}

	Camera(glm::vec4 o, glm::vec4 up, glm::vec4 right, glm::vec4 front) : 
		origin(o),
		up(glm::normalize(up)),
		right(glm::normalize(right)),
		front(glm::normalize(front)) {}

	~Camera();
	void setCamToWorld(glm::mat4);

	Ray getPrimaryRay(float u, float v, float d)
	{
		return Ray(origin, glm::normalize(u * right + v * up - d * front));
	}

	glm::vec4 getOrigin() 
	{
		return origin;
	}

	void update();

protected:
	glm::vec4 origin;
	glm::vec4 up, right, front;
	glm::mat4 camToWorld;
};