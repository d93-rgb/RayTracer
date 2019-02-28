#pragma once
#include <glm.hpp>
#include "ray.h"

class Camera
{
public:
	Camera() : origin(glm::vec3(0.f)),
		right(glm::vec3(0.f, 1.f, 0.f)),
		up(glm::vec3(0.f, 1.f, 0.f)),
		front(glm::vec3(0.f, 1.f, -1.f)) {}
	Camera(glm::vec3 o, glm::vec3 up, glm::vec3 right, glm::vec3 front) : 
		origin(o), up(up), right(right), front(front) {}

	~Camera();
	void setCamToWorld(glm::mat4);

	void update();

protected:
	glm::vec3 origin;
	glm::vec3 up, right, front;
	glm::mat4 camToWorld;
};

