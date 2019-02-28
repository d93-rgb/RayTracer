#include "camera.h"

Camera::Camera()
{
}


Camera::~Camera()
{
}

void Camera::setCamToWorld(glm::mat4 m)
{
	camToWorld = m;
}

void Camera::update()
{
	up *= camToWorld;
	right *= camToWorld;
	front *= camToWorld;
}
