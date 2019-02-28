#pragma once
#include "scene.h"

extern int MAX_DEPTH;
extern float eps;

float shoot_ray(const Scene &s, const Ray &ray, Object **o);

glm::vec3 shoot_recursively(const Scene &s,
	const Ray &ray,
	Object **o,
	int depth);

glm::vec3 phong_shade(const Scene &sc,
	const Ray &ray,
	const glm::vec3 &ob_pos,
	const Object *o);

glm::vec3 handle_transmission(Ray ray);

glm::vec3 handle_reflection(const Scene &s,
	const Ray &ray,
	const glm::vec3 &isect_p,
	Object **o,
	int depth);