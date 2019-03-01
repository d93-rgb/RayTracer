#pragma once
#include "renderer.h"

/*
	Calculate the reflection vector.
	dir	: the incident ray
	N	: the normalized normal vector of a surface
*/
glm::vec3 reflect(glm::vec3 dir, glm::vec3 N)
{
	return dir - 2 * glm::dot(N, dir) * N;
}

/*
	Calculate the refracted vector.
	V	: the view direction
	N	: the normalized normal vector of a surface
*/
bool refract(glm::vec3 V, glm::vec3 N, float refr_idx, glm::vec3 *refracted)
{
	float alpha = glm::dot(-V, N);

	// TODO: calculate refracted ray when coming from a medium other than air
	// refractive index of air = 1.f
	float eta = 1.f / refr_idx;

	if (alpha < 0.f)
	{
		eta = 1.f / eta;
		alpha *= -1;
		N = -N;
	}

	float radicand = 1.f - eta * eta * (1.f - alpha * alpha);

	// check for total internal reflection
	if (radicand < 0.f)
	{
		*refracted = glm::vec3(0.f);
		return false;
	}

	*refracted = eta * V - (eta * alpha + sqrt(radicand)) * N;
	return true;
}

glm::vec3 handle_reflection(const Scene &s,
	const Ray &ray,
	const glm::vec3 &isect_p,
	Object **o,
	int depth)
{
	glm::vec3 refl_rd = glm::normalize(reflect(ray.rd, (*o)->get_normal(isect_p)));

	return shoot_recursively(s, Ray(isect_p + eps * refl_rd, refl_rd), o, ++depth);
}

glm::vec3 handle_transmission(const Scene &s,
	const Ray &ray,
	const glm::vec3 &isect_p,
	Object **o,
	int depth)
{
	glm::vec3 refr_rd;
	if (!refract(ray.rd, (*o)->get_normal(isect_p), (*o)->mat->refr_indx, &refr_rd))
	{
		refr_rd = glm::normalize(reflect(ray.rd, (*o)->get_normal(isect_p)));
		return shoot_recursively(s, Ray(isect_p + eps * refr_rd, refr_rd), o, ++depth);
	}
	refr_rd = glm::normalize(refr_rd);

	return shoot_recursively(s, Ray(isect_p + eps * refr_rd, refr_rd), o, ++depth);
}

/*
	Shoot next ray and obtain the next intersection point.
	Returns the distance to the hit surface and saves hit object
	in the given pointer 'o'.
	s: the scene with its objects
	ray: the next ray to trace
	o: the object that was hit
*/
float shoot_ray(const Scene &s, const Ray &ray, Object **o)
{
	float t_int = INFINITY;
	float tmp = INFINITY;

	// get nearest intersection point
	for (auto &objs : s.get_scene())
	{
		tmp = objs->intersect(ray);

		if (tmp >= 0 && t_int > tmp)
		{
			t_int = tmp;
			*o = objs.get();
			//col[i] = sphs.color * glm::max(0.f, glm::dot(-rd, sphs.get_normal(inters_p)));
			//std::cout << col[i].x << " " << col[i].y << " " << col[i].z << std::endl;
		}
	}
	return t_int;
}

glm::vec3 shoot_recursively(const Scene &s,
	const Ray &ray,
	Object **o,
	int depth)
{
	if (depth == MAX_DEPTH)
	{
		return glm::vec3(0);
	}

	float distance;
	glm::vec3 contribution = glm::vec3(0);
	glm::vec3 isect_p;

	distance = shoot_ray(s, ray, o);

	// check for no intersection
	if (distance < 0 || distance == INFINITY)
	{
		return glm::vec3(0.0f);
	}

	isect_p = ray.ro + distance * ray.rd;

	//if ((glm::length((*o)->mat->ambient) > 0) || (glm::length((*o)->mat->specular) > 0))
		// accumulate all light contribution
	for (auto &l : s.lights)
	{
		contribution += l->phong_shade(s,
			ray/*Ray(ray.ro + eps * ray.rd, ray.rd)*/,
			isect_p,
			*o);
	}

	if (glm::length((*o)->mat->reflective) > 0)
	{
		glm::vec3 reflective = (*o)->mat->reflective;
		contribution += reflective * handle_reflection(s, ray, isect_p, o, depth);
	}

	if (glm::length((*o)->mat->transparent) > 0)
	{
		glm::vec3 transparent = (*o)->mat->transparent;
		contribution += transparent * handle_transmission(s, ray, isect_p, o, depth);
	}

	return contribution;
}
