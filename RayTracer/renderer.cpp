#pragma once
#include "renderer.h"

/*
	Calculate the reflection vector.
	DIR	: the incident ray
	N	: the normalized normal vector of a surface
*/
glm::vec3 reflect(glm::vec3 DIR, glm::vec3 N)
{
	return DIR - 2 * glm::dot(N, DIR) * N;
}

/*
	Calculate diffuse shading of an object.
*/
glm::vec3 diff_shade(const Object &obj,
	const glm::vec3 &ob_pos,
	const Light &light)
{
	glm::vec3 col = light.getEmission(light.p - ob_pos) * obj.mat->diffuse *
		glm::max(0.f, glm::dot(obj.get_normal(ob_pos), -light.dir));
	return col;
}

/*
	Calculate specular shading of an object.
*/
glm::vec3 spec_shade(const Object &obj,
	const glm::vec3 &ob_pos,
	const Light &light,
	const glm::vec3 &view_dir)
{
	glm::vec3 refl = reflect(light.dir, obj.get_normal(ob_pos));
	refl = glm::normalize(refl);

#ifdef DEBUG
	debug_vec.push_back(glm::max(0.f, glm::dot(refl, -view_dir)));
#endif

	return light.getEmission(view_dir) * obj.mat->specular * glm::max(0.f, glm::dot(refl, -view_dir));
}

bool calc_shadow(glm::vec3 p, const Scene &sc, const Light &light)
{
	float t_int = INFINITY;
	float tmp;

	Ray ray = Ray(p, -light.dir);
	ray.ro += ray.rd * eps;

	for (auto &objs : sc.get_scene())
	{
		tmp = objs->intersect(ray);

		if (tmp >= 0 && t_int > tmp)
		{
			t_int = tmp;
		}
	}
	// no intersection found
	if (t_int < 0 || t_int == INFINITY) return true;

	return false;
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

glm::vec3 handle_transmission(Ray ray)
{
	return glm::vec3(0);
}

glm::vec3 handle_refraction(Ray ray)
{
	return glm::vec3(0);
}

glm::vec3 phong_shade(const Scene &sc,
	const Ray &ray,
	const glm::vec3 &ob_pos,
	const Object *o)
{
	float visible = 1.f;
	glm::vec3 color(0);
	glm::vec3 contribution;

	// accumulate all light contribution
	for (auto &li : sc.lights)
	{
		contribution = o->mat->ambient * li->getEmission(ray.rd) +
			diff_shade(*o, ob_pos, *li) +
			spec_shade(*o, ob_pos, *li, ray.rd);

		visible = calc_shadow(ob_pos, sc, *li) == true ? 1.0f : 0.0f;
		color += visible * contribution;
	}
	return color;
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
	if(1)
	{
		contribution = phong_shade(s, ray, isect_p, *o);
	}

	if (glm::length((*o)->mat->reflective) > 0)
	{
		contribution += handle_reflection(s, ray, isect_p, o, depth);
	}

	return contribution;
}
