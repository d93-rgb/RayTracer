#pragma once
#include "scene.h"

extern int MAX_DEPTH;
extern float eps;

float shoot_ray(const Scene &s, const Ray &ray, Object **o);

glm::vec3 shoot_recursively(const Scene &s,
	const Ray &ray,
	Object **o,
	int depth);

glm::vec3 diff_shade(const Object &obj,
	const glm::vec3 &ob_pos,
	const Light &light);

glm::vec3 spec_shade(const Object &obj,
	const glm::vec3 &ob_pos,
	const Light &light,
	const glm::vec3 &view_dir);

glm::vec3 phong_shade(const Scene &sc,
	const Ray &ray,
	const glm::vec3 &ob_pos,
	const Object *o);

bool calc_shadow(glm::vec3 p, const Scene &sc, const Light &light);

glm::vec3 handle_transmission(Ray ray);

glm::vec3 handle_reflection(const Scene &s,
	const Ray &ray,
	const glm::vec3 &isect_p,
	Object **o,
	int depth);