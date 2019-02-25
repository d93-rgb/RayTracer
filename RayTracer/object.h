#pragma once
#include <glm.hpp>

#include "material.h"
#include "ray.h"

struct Object
{
	Material mat;

	Object() = default;

	virtual float intersect(Ray &ray) const = 0;

	virtual glm::vec3 get_normal(glm::vec3 p) = 0;
};

struct Sphere : public Object
{
	glm::vec3 origin;
	float r;
	glm::vec3 color;

	Sphere(glm::vec3 origin, float radius, glm::vec3 color, Material m)
	{
		this->origin = origin;
		this->r = radius;
		this->color = color;
		this->mat = m;
	}

	glm::vec3 get_normal(glm::vec3 p)
	{
		return glm::normalize(p - origin);
	}

	float intersect(Ray &ray) const
	{
		float t1 = INFINITY, t2 = t1;
		float tmp;

		float term_1 = glm::dot(ray.rd, ray.rd);
		float term_2 = 2 * glm::dot(ray.rd, ray.ro - origin);
		float term_3 = glm::dot(ray.ro - origin, ray.ro - origin) - r * r;

		float disc = term_2 * term_2 - 4 * term_1*term_3;

		if (disc < 0)
		{
			return INFINITY;
		}

		t1 = (-term_2 + sqrt(disc)) / (2 * term_1);
		t2 = (-term_2 - sqrt(disc)) / (2 * term_1);

		tmp = fmin(t1, t2);
		tmp = tmp >= 0 ? tmp : fmax(t1, t2);
		return tmp >= 0 ? tmp : INFINITY;
	}
};

struct Plane : public Object
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
	float k;

	Plane(glm::vec3 p, glm::vec3 n, glm::vec3 c, Material m) :
		pos(p), normal(glm::normalize(n)), color(c)
	{
		this->mat = m;
		k = glm::dot(normal, pos);
	}

	float intersect(Ray &ray) const
	{
		float denom = glm::dot(normal, ray.rd);

		if (abs(denom) < 1e-6) return INFINITY;

		float num = k - glm::dot(normal, ray.ro);

		float t = num / denom;

		//if(t >= 0) std::cout << t << std::endl;
		return t >= 0 ? t : INFINITY;
	}

	glm::vec3 get_normal(glm::vec3 p)
	{
		return normal;
	}
};

struct Rectangle : public Object
{
	// position of the lower left corner
	glm::vec3 center;
	glm::vec3 v1;
	glm::vec3 v2;
	// normal of the plane the rectangle resides in
	glm::vec3 normal;

	float v1_dot;
	float v2_dot;

	Rectangle(glm::vec3 center, glm::vec3 u, glm::vec3 v, Material m) :
		center(center - 0.5f * (u + v))
	{
		this->mat = m;
		this->v1 = u;
		this->v2 = v;
		this->normal = glm::normalize(glm::cross(u, v));
		v1_dot = glm::dot(v1, v1);
		v2_dot = glm::dot(v2, v2);
	}

	float intersect(Ray &ray) const
	{
		float denom = glm::dot(ray.rd, normal);

		if (abs(denom) < 1e-6) return INFINITY;

		float num = glm::dot(normal, center - ray.ro);

		float t = num / denom;

		if (t < 0) return INFINITY;

		glm::vec3 isec_p = ray.ro + t * ray.rd;

		float inside_1 = glm::dot(isec_p - center, v1) / v1_dot;
		float inside_2 = glm::dot(isec_p - center, v2) / v2_dot;

		bool test = (0 <= inside_1) && (inside_1 <= 1) && (0 <= inside_2) && (inside_2 <= 1);

		return test == true ? t : INFINITY;
	}

	glm::vec3 get_normal(glm::vec3 p)
	{
		return normal;
	}

	glm::vec3 get_normal()
	{
		return normal;
	}
};
