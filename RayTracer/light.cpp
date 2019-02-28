#pragma once
#include "light.h"
#include "scene.h"

glm::vec3 PointLight::diff_shade(const Object & obj, const glm::vec3 & ob_pos)
{
	glm::vec3 dir = ob_pos - this->p;
	
	//float sq_dist = glm::dot(dir, dir); // attenuation
	

	glm::vec3 col = getEmission(dir) * obj.mat->diffuse *
		glm::max(0.f, glm::dot(obj.get_normal(ob_pos), -glm::normalize(dir)));

	return col; // sq_dist;
}

/*
	Calculate specular shading of an object.
*/
glm::vec3 PointLight::spec_shade(const Object &obj,
	const glm::vec3 &ob_pos,
	const glm::vec3 &view_dir)
{
	glm::vec3 dir = ob_pos - this->p;
	glm::vec3 refl = reflect(dir, obj.get_normal(ob_pos));
	
	//float sq_dist = glm::dot(dir, dir);

	refl = glm::normalize(refl);

#ifdef DEBUG
	debug_vec.push_back(glm::max(0.f, glm::dot(refl, -view_dir)));
#endif

	return getEmission(view_dir) *
		obj.mat->specular *
		glm::max(0.f, glm::dot(refl, -view_dir));
}

bool PointLight::calc_shadow(glm::vec3 p, const Scene &sc)
{
	return true;
	float t_int = INFINITY;
	float tmp;

	Ray ray = Ray(p, glm::normalize(this->p - p));
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

glm::vec3 DistantLight::diff_shade(const Object &obj,
	const glm::vec3 &ob_pos)
{
	glm::vec3 col = getEmission(this->p - ob_pos) * obj.mat->diffuse *
		glm::max(0.f, glm::dot(obj.get_normal(ob_pos), -this->dir));
	return col;
}

glm::vec3 DistantLight::spec_shade(const Object &obj,
	const glm::vec3 &ob_pos,
	const glm::vec3 &view_dir)
{
	glm::vec3 refl = reflect(this->dir, obj.get_normal(ob_pos));
	refl = glm::normalize(refl);

	return getEmission(view_dir) * obj.mat->specular * glm::max(0.f, glm::dot(refl, -view_dir));

}

bool DistantLight::calc_shadow(glm::vec3 p, const Scene &sc)
{
	float t_int = INFINITY;
	float tmp;

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