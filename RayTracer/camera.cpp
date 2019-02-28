#include "camera.h"

Camera::~Camera()
{
}

void Camera::setCamToWorld(glm::mat4 m)
{
	camToWorld = m;
}

void Camera::update()
{
	origin = camToWorld * origin;
	up = camToWorld * up;
	right = camToWorld * right;
	front = camToWorld * front;
}
