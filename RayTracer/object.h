#pragma once
#include <glm/gtc/matrix_transform.hpp>

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

struct Shape
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
		for (int i = 0; i < 3; ++i)
		{
			assert(min_bounds[i] < max_bounds[i]);
		}

		boundaries[0] = (min_bounds);
		boundaries[1] = (max_bounds);
	}

	bool intersect(const Ray &ray)
	{
		assert(abs(length(ray.rd)) > 0);

		// no need for checking division by zero, floating point arithmetic is helping here
		glm::vec3 inv_rd = 1.f / ray.rd;
		glm::vec3 t[2] = { glm::vec3(INFINITY), glm::vec3(INFINITY) };
		// interval of intersection
		float t0 = 0.f, t1 = INFINITY;

		t[0] = boundaries[0] - ray.ro;
		t[1] = boundaries[1] - ray.ro;

		for (int i = 0; i < 3; ++i)
		{
			t[0][i] *= inv_rd[i];
			t[1][i] *= inv_rd[i];

			if (t[0][i] > t[1][i])
			{
				std::swap(t[0][i], t[1][i]);
			}

			// update interval and check if intersection possible
			t0 = t0 > t[0][i] ? t0 : t[0][i];
			t1 = t1 < t[1][i] ? t1 : t[1][i];

			if (t0 > t1)
			{
				return false;
			}
		}

		return true;
	}

	glm::vec3 get_normal(glm::vec3 p) const
	{
		return glm::vec3(0.f);
	}
};

struct Sphere : public Shape
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

		float disc = term_2 * term_2 - 4 * term_1 * term_3;

		if (disc < 0)
		{
			return INFINITY;
		}

		tmp = sqrt(disc);
		t1 = (-term_2 + tmp) / term_1 * 0.5f;
		t2 = (-term_2 - tmp) / term_1 * 0.5f;

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
struct Plane : public Shape
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

	static glm::vec3 getTangentVector(glm::vec3 normal)
	{
		if (normal.x != 0.f)
		{
			return glm::vec3(-normal.y / normal.x, 1.f, 0.f);
		}
		else if (normal.y != 0.f)
		{
			return glm::vec3(1.f, -normal.x / normal.y, 0.f);
		}
		else if (normal.z != 0.f)
		{
			return glm::vec3(0.f, 1.f, -normal.y / normal.z);
		}

		return glm::vec3(0.f);
	}
};

struct Rectangle : public Shape
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
		Intersection routine for the transformed_ray-triangle intersection test.
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

class Cube : public Shape
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


		// calculate the 6 transformed_ray-plane intersections
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
		This routine will be used for the transformed_ray-bounding box intersection test.
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


		// calculate the 6 transformed_ray-plane intersections
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
	std::unique_ptr<Shape> sides[],
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

class Triangle : public Shape
{
	bool interpolate = false;

	// vertices
	glm::vec3 p1, p2, p3;
	// normal
	glm::vec3 n;
	glm::mat4 objToWorld;
	glm::mat4 worldToObj;
	glm::mat3 m_inv;

public:
	Triangle(glm::vec3 p1,
		glm::vec3 p2,
		glm::vec3 p3,
		glm::vec3 n,
		glm::mat4 objToWorld,
		std::shared_ptr<Material> mat) :
		objToWorld(objToWorld),
		worldToObj(glm::inverse(objToWorld))
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

				if (interpolate)
				{
					isect->mat.reset(new Material());
					//TODO: NOT THREADSAFE => CHANGE
					// interpolate with barycentric coordinates
					glm::vec3 c = glm::vec3((1 - (t_vec.y + t_vec.z)), t_vec.y, t_vec.z);
					isect->mat->ambient = c;
					isect->mat->diffuse = glm::vec3(0.f);
					isect->mat->specular = glm::vec3(0.f);
				}
				else
				{
					isect->mat = mat;
				}
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

	void setInterpolate(bool interpolate)
	{
		this->interpolate = interpolate;
	}
};

class TriangleMesh : public Shape
{
public:
	std::unique_ptr<Bounds3> boundary;
	std::vector<std::unique_ptr<Triangle>> tr_mesh;

	TriangleMesh(std::unique_ptr<Bounds3> &bounds) : boundary(std::move(bounds))
	{
	}

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

struct Cylinder : public Shape
{
	float height, radius;
	glm::vec3 pos, dir;
	glm::mat4 objToWorld;
	glm::mat4 worldToObj;

	Cylinder(glm::vec3 pos,
		glm::vec3 dir,
		float radius,
		float height,
		std::shared_ptr<Material> mat) :
		pos(pos), dir(glm::normalize(dir)), radius(radius), height(height)
	{
		this->mat = mat;

		glm::vec3 tangent_v = glm::normalize(Plane::getTangentVector(dir));

		//objToWorld = glm::lookAt(pos, pos + tangent_v, dir);
		// transform axis of the cylinder to the axis given by dir
		objToWorld[0] = glm::vec4(glm::cross(dir, tangent_v), 0.f);
		objToWorld[1] = glm::vec4(dir, 0.f);
		objToWorld[2] = glm::vec4(tangent_v, 0.f);
		objToWorld[3] = glm::vec4(pos, 1.f);

		worldToObj = glm::inverse(objToWorld);
	}

	float intersect(const Ray &ray, SurfaceInteraction *isect)
	{
		Ray transformed_ray{ worldToObj * glm::vec4(ray.ro, 1.f),
			worldToObj * glm::vec4(ray.rd, 0.f) };
		glm::vec2 t_ro = glm::vec2(transformed_ray.ro.x, transformed_ray.ro.z);
		glm::vec2 t_rd = glm::vec2(transformed_ray.rd.x, transformed_ray.rd.z);

		glm::vec3 isect_p1 = glm::vec3(INFINITY), isect_p2 = glm::vec3(INFINITY);
		float x1, x2, tmp1 = INFINITY, tmp2 = INFINITY;
		float a = glm::dot(t_rd, t_rd);
		float b = 2 * glm::dot(t_ro, t_rd);
		float c = glm::dot(t_ro, t_ro) - radius * radius;

		float discr = b * b - 4 * a * c;

		if (discr < 0)
		{
			return INFINITY;
		}

		discr = sqrt(discr);
		x1 = (-b + discr) / a * 0.5f;
		x2 = (-b - discr) / a * 0.5f;

		// get intersection points
		if (x1 > 0)
		{
			isect_p1 = transformed_ray.ro + x1 * transformed_ray.rd;
		}

		if (x2 > 0)
		{
			isect_p2 = transformed_ray.ro + x2 * transformed_ray.rd;
		}

		// no intersection found check
		if ((isect_p1 == glm::vec3(INFINITY)) && (isect_p2 == glm::vec3(INFINITY)))
		{
			return INFINITY;
		}

		// check if inside the given height boundary
		if (isect_p1.y >= 0 && isect_p1.y <= height)
		{
			tmp1 = x1;
		}

		if (isect_p2.y >= 0 && isect_p2.y <= height)
		{
			tmp2 = x2;
		}

		tmp2 = std::min(tmp1, tmp2);

		if (tmp2 < ray.tNearest)
		{
			ray.tNearest = tmp2;
			isect->p = ray.ro + ray.rd * tmp2;
			isect->normal = get_normal(transformed_ray.ro + tmp2 * transformed_ray.rd);
			isect->mat = mat;
		}

		return tmp2;
	}

	glm::vec3 get_normal(glm::vec3 p) const
	{
		// TODO: Inside or outside? How to figure out which side the ray hit?
		return glm::vec3(p.x, 0.f, p.z);
	}
};
}