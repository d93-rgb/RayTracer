#include "scene.h"


void Scene::init()
{

	glm::vec3 sph_or_1 = glm::vec3(-2, -2, -5);
	glm::vec3 sph_or_2 = glm::vec3(-4, 2, -18);
	glm::vec3 sph_or_3 = glm::vec3(9, 3, -15);
	glm::vec3 sph_or_4 = glm::vec3(-11, 7, -15);
	float radius[] = { 1, 1.5, 3, 2 };

	Material m1 = Material(glm::vec3(0.1, 0, 0), glm::vec3(0.7, 0, 0), glm::vec3(1.0, 0, 0));
	Material m2 = Material(glm::vec3(0, 0.1, 0), glm::vec3(0, 0.7, 0), glm::vec3(0, 1.0, 0));
	Material m3 = Material(glm::vec3(0.1, 0, 0.1), glm::vec3(0.7, 0, 0.7), glm::vec3(0.7, 0, 0.7));
	Material m4 = Material(glm::vec3(0.1, 0.1, 0.1), glm::vec3(0.3, 0.3, 0.8), glm::vec3(0.7, 0.7, 0.7));

	Material m5 = Material(glm::vec3(0.1, 0.1, 0.1), glm::vec3(0.4, 0.4, 0.4), glm::vec3(0, 0, 0));

	Sphere sph_1 = Sphere(sph_or_1, radius[0], glm::vec3(0.5, 0, 0), m1);
	sc.emplace_back(&sph_1);

	Sphere sph_2 = Sphere(sph_or_2, radius[1], glm::vec3(0.5, 0.5, 0), m2);
	sc.emplace_back(&sph_2);

	Sphere sph_3 = Sphere(sph_or_3, radius[2], glm::vec3(0, 0, 0.8), m3);
	sc.emplace_back(&sph_3);

	Sphere sph_4 = Sphere(sph_or_4, radius[3], glm::vec3(0, 0, 0.8), m4);
	sc.emplace_back(&sph_4);

	/*Plane pl_1 = Plane(glm::vec3(0, -3, -12), glm::vec3(0, 10, 5), glm::vec3(0.2f), m4);
	sc.emplace_back(&pl_1);*/

	Rectangle rect_1 = Rectangle(glm::vec3(-4, 2, -18),
		glm::vec3(50, 0, 0), glm::vec3(0, 30, -30), m5);
	sc.emplace_back(&rect_1);
	//print_vec3(rect_1.get_normal());
	//Object *objects[] = { &sph_1, &sph_2, &sph_3, &pl_1, &rect_1 };
}