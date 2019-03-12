#pragma once
#include "rt.h"
#include "material.h"
#include "ray.h"
#include "interaction.h"
#include "texture.h"

//#define DEBUG

namespace rt
{

template <typename T> inline int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

struct Object
{
	glm::mat4 obj_to_world, world_to_obj;
	std::shared_ptr<Material> mat;

	virtual float intersect(const Ray &ray, SurfaceInteraction *isect) = 0;

	virtual glm::vec3 get_normal(glm::vec3 p) const = 0;
};

class Bounds3
{
	glm::vec3 normal;
	glm::vec3 boundaries[2];

public:
	Bounds3(glm::vec3 min_bounds, glm::vec3 max_bounds)
	{
		boundaries[0] = (min_bounds);
		boundaries[1] = (max_bounds);
	}

	bool intersect(const Ray &ray)
	{
		assert(abs(length(ray.rd)) > 0);

		glm::vec3 t[2] = { glm::vec3(INFINITY), glm::vec3(INFINITY) };

		// calculate the 6 ray-plane intersections
		t[0] = boundaries[0] - ray.ro;
		t[1] = boundaries[1] - ray.ro;

		if (ray.rd.x != 0)
		{
			t[0].x /= ray.rd.x;
			t[1].x /= ray.rd.x;
		}
		else
		{
			t[0].x = (t[0].x == 0 ? 0.f : INFINITY);
			t[1].x = (t[1].x == 0 ? 0.f : INFINITY);
		}

		if (ray.rd.y != 0)
		{
			t[0].y /= ray.rd.y;
			t[1].y /= ray.rd.y;
		}
		else
		{
			t[0].y = (t[0].y == 0 ? 0.f : INFINITY);
			t[1].y = (t[1].y == 0 ? 0.f : INFINITY);
		}

		if (ray.rd.z != 0)
		{
			t[0].z /= ray.rd.z;
			t[1].z /= ray.rd.z;
		}
		else
		{
			t[0].z = (t[0].z == 0 ? 0.f : INFINITY);
			t[1].z = (t[1].z == 0 ? 0.f : INFINITY);
		}

		if (t[0].x > t[1].x)
		{
			std::swap(t[0].x, t[1].x);
		}

		if (t[0].y > t[1].y)
		{
			std::swap(t[0].y, t[1].y);
		}

		// no overlap found => no intersection
		if (t[1].x < t[0].y || t[0].x > t[1].y)
		{
			return false;
		}

		if (t[0].z > t[1].z)
		{
			std::swap(t[0].z, t[1].z);
		}

		// no overlap found => no intersection
		if (t[1].x < t[0].z || t[0].x > t[1].z || t[1].y < t[0].z || t[0].y > t[1].z)
		{
			return false;
		}

		return true;
	}

	glm::vec3 get_normal(glm::vec3 p) const
	{
		return glm::vec3(0.f);
	}
};

struct Sphere : public Object
{
	float r;
	glm::vec3 origin;
	glm::vec3 color;
	std::shared_ptr<Texture> tex;


	Sphere(glm::vec3 origin, float radius, glm::vec3 color, std::shared_ptr<Material> m)
	{
		this->origin = origin;
		this->r = radius;
		this->color = color;
		this->mat = m;
		tex = nullptr;
	}

	Sphere(glm::vec3 origin, 
		float radius, 
		glm::vec3 color, 
		std::shared_ptr<Material> m,
		std::shared_ptr<Texture> texture)
	{
		this->origin = origin;
		this->r = radius;
		this->color = color;
		this->mat = m;
		this->tex = texture;
	}

	glm::vec3 get_normal(glm::vec3 p) const
	{
		return glm::normalize(p - origin);
	}

	float intersect(const Ray &ray, SurfaceInteraction *isect)
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

		tmp = std::fmin(t1, t2);
		tmp = tmp >= 0 ? tmp : fmax(t1, t2);
		tmp = tmp >= 0 ? tmp : INFINITY;

		if (tmp >= 0 && tmp < INFINITY)
		{
			if (tmp < ray.tNearest)
			{
				ray.tNearest = tmp;
				isect->p = ray.ro + ray.rd * tmp;
				isect->normal = get_normal(isect->p);
				isect->uv = getTextureCoordinates(isect->p);
				isect->mat = mat;
				isect->texture = tex;
			}
		}
		return tmp;
	}

	glm::vec2 getTextureCoordinates(glm::vec3 pos)
	{
		glm::vec3 pos_shift = glm::normalize(pos - origin);
		//float radius = glm::length(pos_shift);
		float u = (1 + atan2f(pos_shift.z, pos_shift.x) / (float)M_PI) * 0.5f;
		float v = acosf(pos_shift.y) / (float)M_PI;
		
		return glm::vec2(u, v);
	}
};

struct Plane : public Object
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
	float k;

	Plane(glm::vec3 p, glm::vec3 n) :
		pos(p), normal(glm::normalize(n))
	{
		k = glm::dot(normal, pos);
	}

	Plane(glm::vec3 p, glm::vec3 n, glm::vec3 color, std::shared_ptr<Material> m) :
		pos(p), normal(glm::normalize(n)), color(color)
	{
		this->mat = m;
		k = glm::dot(normal, pos);
	}

	float intersect(const Ray &ray, SurfaceInteraction * isect)
	{
		float denom = glm::dot(normal, ray.rd);

		if (abs(denom) < 1e-6) return INFINITY;

		float num = k - glm::dot(normal, ray.ro);

		float t = num / denom;

		//if(t >= 0) std::cout << t << std::endl;
		t = t >= 0 ? t : INFINITY;

		if (t >= 0 && t < INFINITY)
		{
			if (t < ray.tNearest)
			{
				ray.tNearest = t;
				isect->p = ray.ro + ray.rd * t;
				isect->normal = get_normal(isect->p);
				isect->mat = mat;
			}
		}

		return t;
	}

	float intersect(const Ray &ray)
	{
		float denom = glm::dot(normal, ray.rd);

		if (abs(denom) < 1e-6) return INFINITY;

		float num = k - glm::dot(normal, ray.ro);

		float t = num / denom;

		//if(t >= 0) std::cout << t << std::endl;
		return t >= 0 ? t : INFINITY;
	}

	/*
		Get the missing coordinate of the point P, so that it lies on the plane.

		coordinate = 0: get the missing x-coordinate => v = y and w = z
		coordinate = 1: get the missing y-coordinate => v = x and w = z
		coordinate = 2: get the missing z-coordinate => v = x and w = y
	*/
	glm::vec4 getPlanePos(float v, float w, int coordinate)
	{
		glm::vec4 c;
		float a;
		float tmp = glm::dot(normal, pos);

		switch (coordinate)
		{
		case 0:
			a = (tmp - (normal.y * v + normal.z * w)) / normal.x;
			c = glm::vec4(a, v, w, 1.f);
			break;
		case 1:
			a = (tmp - (normal.x * v + normal.z * w)) / normal.y;
			c = glm::vec4(v, a, w, 1.f);
			break;
		case 2:
			a = (tmp - (normal.x * v + normal.y * w)) / normal.z;
			c = glm::vec4(v, w, a, 1.f);
			break;
		default:
			return glm::vec4(INFINITY);
		}
		return c;
	}

	glm::vec3 get_normal(glm::vec3 p) const
	{
		return normal;
	}
};

struct Rectangle : public Object
{
	glm::vec3 center;
	glm::vec3 v1;
	glm::vec3 v2;
	// normal of the plane the rectangle resides in
	glm::vec3 normal;

	float v1_dot;
	float v2_dot;

	Rectangle() {};

	Rectangle(glm::vec3 center, glm::vec3 u, glm::vec3 v, std::shared_ptr<Material> m) :
		center(center - 0.5f * (u + v))
	{
		this->mat = m;
		this->v1 = u;
		this->v2 = v;
		this->normal = glm::normalize(glm::cross(u, v));
		v1_dot = glm::dot(v1, v1);
		v2_dot = glm::dot(v2, v2);
	}

	/*
		Intersection routine for the ray-triangle intersection test.
		The nearest intersection parameter of Ray will not be updated
	*/
	float intersect(const Ray &ray, SurfaceInteraction * isect)
	{
		float denom = glm::dot(ray.rd, normal);

		if (abs(denom) < 1e-6) return INFINITY;

		float num = glm::dot(normal, center - ray.ro);

		float t = num / denom;

		if (t < 0) return INFINITY;

		glm::vec3 isec_p = ray.ro + t * ray.rd;

		float inside_1 = glm::dot(isec_p - center, v1) / v1_dot;
		float inside_2 = glm::dot(isec_p - center, v2) / v2_dot;

		bool test = (0 <= inside_1) && (inside_1 <= 1) &&
			(0 <= inside_2) && (inside_2 <= 1);

		if (test)
		{
			if (t < ray.tNearest)
			{
				ray.tNearest = t;
				isect->p = isec_p;
				isect->normal = get_normal(isect->p);
				isect->mat = mat;
			}
		}
		return test ? t : INFINITY;;

	}

	/*
	TODO: Check boundaries
	*/
	glm::vec4 getRectPos(float v, float w, char coordinate)
	{
		glm::vec4 c;
		float a;
		float tmp = glm::dot(normal, center);

		switch (coordinate)
		{
		case 'x':
			a = (tmp - (normal.y * v + normal.z * w)) / normal.x;
			c = glm::vec4(a, v, w, 1.f);
			break;
		case 'y':
			a = (tmp - (normal.x * v + normal.z * w)) / normal.y;
			c = glm::vec4(v, a, w, 1.f);
			break;
		case 'z':
			a = (tmp - (normal.x * v + normal.y * w)) / normal.z;
			c = glm::vec4(v, w, a, 1.f);
			break;
		default:
			return glm::vec4(INFINITY);
		}
		return c;
	}

	glm::vec3 get_normal(glm::vec3 p) const
	{
		return normal;
	}

	glm::vec3 get_normal() const
	{
		return normal;
	}
};

class Cube : public Object
{
	glm::vec3 normal;
	glm::vec3 boundaries;
	glm::vec3 moved_centers[6];
	glm::vec3 v1[3], v2[3];
	float v1_dots[3], v2_dots[3];

public:
	Cube(glm::vec3 side_length, std::shared_ptr<Material> mat) :
		boundaries(side_length / 2.f)
	{
		assert(fmin(fmin(side_length.x, side_length.y), side_length.z) > 0);
		this->mat = mat;

		// sides
		v1[0] = glm::vec3(0.f, 0.f, side_length[2]);
		v2[0] = glm::vec3(0.f, side_length[1], 0.f);

		v1[1] = glm::vec3(side_length[0], 0.f, 0.f);
		v2[1] = glm::vec3(0.f, 0.f, side_length[2]);

		v1[2] = glm::vec3(side_length[0], 0.f, 0.f);
		v2[2] = glm::vec3(0.f, side_length[1], 0.f);

		for (int i = 0; i < 6; ++i)
		{
			// too lazy to write out the six moved_centers
			int i_m = i % 3;
			moved_centers[i] = side_length[i_m] * glm::vec3(1.f) *
				glm::vec3(i_m == 0, i_m == 1, i_m == 2) *
				(-1.f * (i >= 3 ? 1.f : -1.f)) - 0.5f * (v1[i_m] + v2[i_m]);
		}

		for (int i = 0; i < 3; ++i)
		{
			v1_dots[i] = glm::dot(v1[i], v1[i]);
			v2_dots[i] = glm::dot(v2[i], v2[i]);
		}
	}

	float intersect(const Ray &ray, SurfaceInteraction *isect)
	{
		assert(abs(length(ray.rd)) > 0);

		Ray transformed_ray{ world_to_obj * glm::vec4(ray.ro, 1.f),
			world_to_obj * glm::vec4(ray.rd, 0.f) };

		int nearest = 0;
		int tmp = 0;
		float isec_t = INFINITY;
		float inside_1;
		float inside_2;
		bool tests[6] = { false };
		glm::vec3 t[2] = { glm::vec3(INFINITY), glm::vec3(INFINITY) };
		glm::vec3 isec_p;


		// calculate the 6 ray-plane intersections
		t[0] = (boundaries - transformed_ray.ro);
		t[1] = -(boundaries + transformed_ray.ro);


		if (transformed_ray.rd.x != 0)
		{
			t[0].x /= transformed_ray.rd.x;
			t[1].x /= transformed_ray.rd.x;
		}
		else
		{
			t[0].x = (t[0].x == 0 ? 0.f : INFINITY);
			t[1].x = (t[1].x == 0 ? 0.f : INFINITY);
		}

		if (transformed_ray.rd.y != 0)
		{
			t[0].y /= transformed_ray.rd.y;
			t[1].y /= transformed_ray.rd.y;
		}
		else
		{
			t[0].y = (t[0].y == 0 ? 0.f : INFINITY);
			t[1].y = (t[1].y == 0 ? 0.f : INFINITY);
		}

		if (transformed_ray.rd.z != 0)
		{
			t[0].z /= transformed_ray.rd.z;
			t[1].z /= transformed_ray.rd.z;
		}
		else
		{
			t[0].z = (t[0].z == 0 ? 0.f : INFINITY);
			t[1].z = (t[1].z == 0 ? 0.f : INFINITY);
		}

		// check if inside boundaries
		for (int i = 0; i < 6; ++i)
		{
			tmp = i % 3;
			if (t[i >= 3][tmp] >= 0.f)
			{
				isec_t = t[i >= 3][tmp];
				isec_p = transformed_ray.ro + isec_t * transformed_ray.rd;

				inside_1 = glm::dot(isec_p - moved_centers[i], v1[tmp]) /
					v1_dots[tmp];
				inside_2 = glm::dot(isec_p - moved_centers[i], v2[tmp]) /
					v2_dots[tmp];

				tests[i] = (0 <= inside_1) && (inside_1 <= 1)
					&& (0 <= inside_2) && (inside_2 <= 1);
			}
		}

		isec_t = INFINITY;
		for (int i = 0; i < 6; ++i)
		{
			tmp = i % 3;
			if (tests[i])
			{
				if (isec_t > t[i >= 3][tmp])
				{
					isec_t = t[i >= 3][tmp];
				}
			}
		}

		if (isec_t >= 0 && isec_t < INFINITY)
		{
			if (isec_t < ray.tNearest)
			{
				ray.tNearest = isec_t;
				isect->p = ray.ro + ray.rd * isec_t;
				isect->normal = get_normal(isect->p);
				isect->mat = mat;
			}
		}

		return isec_t;
	}


	/*
		Intersection test without updating the nearest intersection parameter for Ray.
		This routine will be used for the ray-bounding box intersection test.
	*/
	float intersect(const Ray &ray)
	{
		assert(abs(length(ray.rd)) > 0);

		Ray transformed_ray{ world_to_obj * glm::vec4(ray.ro, 1.f),
			world_to_obj * glm::vec4(ray.rd, 0.f) };

		int nearest = 0;
		int tmp = 0;
		float isec_t = INFINITY;
		float inside_1;
		float inside_2;
		bool tests[6] = { false };
		glm::vec3 t[2] = { glm::vec3(INFINITY), glm::vec3(INFINITY) };
		glm::vec3 isec_p;


		// calculate the 6 ray-plane intersections
		t[0] = (boundaries - transformed_ray.ro);
		t[1] = -(boundaries + transformed_ray.ro);


		if (transformed_ray.rd.x != 0)
		{
			t[0].x /= transformed_ray.rd.x;
			t[1].x /= transformed_ray.rd.x;
		}
		else
		{
			t[0].x = (t[0].x == 0 ? 0.f : INFINITY);
			t[1].x = (t[1].x == 0 ? 0.f : INFINITY);
		}

		if (transformed_ray.rd.y != 0)
		{
			t[0].y /= transformed_ray.rd.y;
			t[1].y /= transformed_ray.rd.y;
		}
		else
		{
			t[0].y = (t[0].y == 0 ? 0.f : INFINITY);
			t[1].y = (t[1].y == 0 ? 0.f : INFINITY);
		}

		if (transformed_ray.rd.z != 0)
		{
			t[0].z /= transformed_ray.rd.z;
			t[1].z /= transformed_ray.rd.z;
		}
		else
		{
			t[0].z = (t[0].z == 0 ? 0.f : INFINITY);
			t[1].z = (t[1].z == 0 ? 0.f : INFINITY);
		}

		// check if inside boundaries
		for (int i = 0; i < 6; ++i)
		{
			tmp = i % 3;
			if (t[i >= 3][tmp] >= 0.f)
			{
				isec_t = t[i >= 3][tmp];
				isec_p = transformed_ray.ro + isec_t * transformed_ray.rd;

				inside_1 = glm::dot(isec_p - moved_centers[i], v1[tmp]) /
					v1_dots[tmp];
				inside_2 = glm::dot(isec_p - moved_centers[i], v2[tmp]) /
					v2_dots[tmp];

				tests[i] = (0 <= inside_1) && (inside_1 <= 1)
					&& (0 <= inside_2) && (inside_2 <= 1);
			}
		}

		isec_t = INFINITY;
		for (int i = 0; i < 6; ++i)
		{
			tmp = i % 3;
			if (tests[i])
			{
				if (isec_t > t[i >= 3][tmp])
				{
					isec_t = t[i >= 3][tmp];
				}
			}
		}
		return isec_t;
	}

	glm::vec3 get_normal(glm::vec3 p) const
	{
#ifdef DEBUG
		static int iter = 0;
#endif
#ifdef DEBUG
		if ((iter++) % 150 == 0) {
			bool stop = true;
		}
#endif
		p = world_to_obj * glm::vec4(p, 1.f);
		glm::vec3 a_p = glm::abs(p);
		return a_p.x > a_p.y ?
			(a_p.x > a_p.z ? glm::vec3(sgn(p.x), 0.f, 0.f) : glm::vec3(0.f, 0.f, sgn(p.z))) :
			(a_p.y > a_p.z ? glm::vec3(0.f, sgn(p.y), 0.f) : glm::vec3(0.f, 0.f, sgn(p.z)));
	}
};

inline void create_cube(glm::vec3 center,
	glm::vec3 up,
	glm::vec3 front,
	float s_len,
	std::unique_ptr<Object> sides[],
	std::shared_ptr<Material> mat)
{
	float tmp = s_len / 2;

	glm::vec3 n_up = glm::normalize(up);
	glm::vec3 n_front = glm::normalize(front);
	glm::vec3 n_u_cross_f = glm::normalize(glm::cross(n_up, n_front));

	glm::vec3 t_u = tmp * n_up;
	glm::vec3 t_f = tmp * n_front;
	glm::vec3 t_uf = tmp * n_u_cross_f;

	n_up = s_len * n_up;
	n_front = s_len * n_front;
	n_u_cross_f = s_len * n_u_cross_f;

	//top
	sides[0].reset(new Rectangle(center + t_u, -n_u_cross_f, n_front, mat));
	//bottom
	sides[1].reset(new Rectangle(center - t_u, n_front, -n_u_cross_f, mat));
	//front
	sides[2].reset(new Rectangle(center + t_f, n_up, -n_u_cross_f, mat));
	//back
	sides[3].reset(new Rectangle(center - t_f, -n_u_cross_f, n_up, mat));
	//left
	sides[4].reset(new Rectangle(center + t_uf, n_up, n_front, mat));
	//right
	sides[5].reset(new Rectangle(center - t_uf, n_front, n_up, mat));
}

class Triangle : public Object
{
	// vertices
	glm::vec3 p1, p2, p3;
	// normal
	glm::vec3 n;
	glm::mat4 objToWorld;
	glm::mat3 m_inv;

public:
	Triangle(glm::vec3 p1,
		glm::vec3 p2,
		glm::vec3 p3,
		glm::vec3 n,
		glm::mat4 objToWorld,
		std::shared_ptr<Material> mat) :
		objToWorld(objToWorld)
	{
		this->mat = mat;

		this->p1 = objToWorld * glm::vec4(p1, 1.f);
		this->p2 = objToWorld * glm::vec4(p2, 1.f);
		this->p3 = objToWorld * glm::vec4(p3, 1.f);

		this->n = objToWorld * glm::vec4(n, 0.f);

		this->m_inv = glm::inverse(glm::mat3(this->p1, this->p2, this->p3));

	}

	float intersect(const Ray &ray, SurfaceInteraction * isect)
	{
		float t_plane = INFINITY;
		Plane plane{ p1, n };

		// intersect without updating nearest intersection parameter
		t_plane = plane.intersect(ray);

		glm::vec3 t_vec = m_inv * (ray.ro + t_plane * ray.rd);

		if (t_vec.y + t_vec.z <= 1 &&
			t_vec.y >= 0 &&
			t_vec.z >= 0)
		{
			if (t_plane < ray.tNearest)
			{
				ray.tNearest = t_plane;
				isect->p = ray.ro + ray.rd * t_plane;
				isect->normal = get_normal(isect->p);
				isect->mat = mat;
			}

			return t_plane;
		}
		return INFINITY;
	}

	glm::vec3 get_normal(glm::vec3 p) const
	{
		return n;
	}

	glm::vec3 get_normal() const
	{
		return n;
	}
};

class TriangleMesh : public Object
{
public:
	std::unique_ptr<Bounds3> boundary;
	std::vector<std::unique_ptr<Triangle>> tr_mesh;

	TriangleMesh(std::unique_ptr<Bounds3> &bounds) : boundary(std::move(bounds))
	{}

	TriangleMesh() {}

	float intersect(const Ray &ray, SurfaceInteraction *isect)
	{
		float t_int = INFINITY;
		float tmp = INFINITY;
		bool hit = false;

		hit = boundary->intersect(ray);
		if (!hit) {
			return INFINITY;
		}

		// get nearest intersection point
		for (auto &objs : tr_mesh)
		{
			tmp = objs->intersect(ray, isect);

			if (tmp >= 0 && t_int > tmp)
			{
				t_int = tmp;
			}
		}
		return t_int;
	}

	glm::vec3 get_normal(glm::vec3 p) const
	{
		return glm::vec3(0.f);
	}

};

}