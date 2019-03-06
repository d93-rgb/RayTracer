#pragma once
#include "light.h"
#include "scene.h"

glm::vec3 PointLight::diff_shade(const Object & obj, const glm::vec3 & ob_pos)
{
	glm::vec3 dir = ob_pos - this->p;

	//float sq_dist = glm::dot(dir, dir); // attenuation

	glm::vec3 col = getEmission(dir) * obj.mat->diffuse *
		glm::max(0.f, 
			glm::dot(obj.get_normal(ob_pos),
			-glm::normalize(dir)));

	return col; // sq_dist;
}

/*
	Calculate specular shading of an object.
*/
glm::vec3 PointLight::spec_shade(const Object &obj,
	const glm::vec3 &ob_pos,
	const glm::vec3 &view_dir)
{
	//glm::vec3 dir = ob_pos - this->p;
	glm::vec3 dir = this->p - ob_pos;
	//glm::vec3 refl = reflect(dir, obj.get_normal(ob_pos));
	glm::vec3 half = (glm::normalize(dir) - view_dir);
	half /= glm::length(half);
	//refl = glm::normalize(refl);

	return getEmission(view_dir) *
		obj.mat->specular *
		powf(glm::max(0.f, glm::dot(half, obj.get_normal(ob_pos))), 
		obj.mat->getShininess());
}

/*
	Return true if object is visible to the light and false otherwise
*/
bool PointLight::calc_shadow(glm::vec3 p, const Scene &sc)
{
	float dist;
	float t_int = INFINITY;
	float tmp = INFINITY;
	
	glm::vec3 dist_v = this->p - p;
	
	Ray ray = Ray(p, glm::normalize(dist_v));
	
	dist = glm::length(dist_v);
	ray.ro += ray.rd * eps;

	// send shadow rays
	for (auto &objs : sc.get_scene())
	{
		tmp = objs->intersect(ray);

		if (tmp >= 0 && t_int > tmp)
		{
			t_int = tmp;
		}
	}
	// no intersection found
	if (t_int < 0 || t_int == INFINITY || t_int > dist)
	{
		return true;
	}

	return false;
}


glm::vec3 PointLight::phong_shade(const Scene &sc,
	const Ray &ray,
	const glm::vec3 &ob_pos,
	const Object *o)
{
	bool visible = true;
	glm::vec3 color(0);

	glm::vec3 dir = p - ob_pos;
	float inv_sqd_dist = 1.f / glm::dot(dir, dir);

	//if (sqd_dist > 1.f) sqd_dist *= 0.1f;

	visible = calc_shadow(ob_pos, sc);
	color = 0.01f * o->mat->ambient * getEmission(ray.rd);

	if (visible) {
		color += (diff_shade(*o, ob_pos) +
			spec_shade(*o, ob_pos, ray.rd)) * inv_sqd_dist;
	}
	return color;
}

glm::vec3 DistantLight::diff_shade(const Object &obj,
	const glm::vec3 &ob_pos)
{
	float angle = glm::max(0.f, 
		glm::dot(obj.get_normal(ob_pos), -this->dir));

	if (angle <= 0)
	{
		return glm::vec3(0.f);
	}

	return getEmission(this->p - ob_pos) * obj.mat->diffuse *
		angle;
}


glm::vec3 DistantLight::spec_shade(const Object &obj,
	const glm::vec3 &ob_pos,
	const glm::vec3 &view_dir)
{
	glm::vec3 refl = reflect(this->dir, obj.get_normal(ob_pos));
	float angle = glm::max(0.f,
		glm::dot(obj.get_normal(ob_pos), -this->dir));

	if (angle <= 0)
	{
		return glm::vec3(0.f);
	}

	refl = glm::normalize(refl);

	return getEmission(view_dir) * obj.mat->specular * 
		powf(angle, obj.mat->getShininess());
}

bool DistantLight::calc_shadow(glm::vec3 p, const Scene &sc)
{
	float t_int = INFINITY;
	float tmp = INFINITY;

	Ray ray = Ray(p, -this->dir);
	ray.ro += ray.rd * eps;

	// send shadow rays
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


glm::vec3 DistantLight::phong_shade(const Scene &sc,
	const Ray &ray,
	const glm::vec3 &ob_pos,
	const Object *o)
{
	bool visible = true;
	glm::vec3 color(0);

	visible = calc_shadow(ob_pos, sc);
	color = o->mat->ambient * getEmission(ray.rd);

	if (visible) {
		color += diff_shade(*o, ob_pos) +
			spec_shade(*o, ob_pos, ray.rd);

	}
	return color;
}