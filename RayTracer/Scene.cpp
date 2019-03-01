#include "scene.h"


void Scene::init()
{
	glm::vec3 translation = glm::vec3(-1.f, 2.f, 20.f);
	float rot_y = glm::radians(0.f);

	std::vector<glm::vec3> sph_origins = {
		glm::vec3(-10, -2, -5),
		glm::vec3(-9, 21, -22),
		glm::vec3(9, 3, -15),
		glm::vec3(-11, 7, -15),
		glm::vec3(0, 12, -25),
		glm::vec3(0, -2, -11),
		glm::vec3(-3, -2, -4),
		glm::vec3(-14, -2, -3),
		//glm::vec3(-4, 20, -15.f)
	};
	float radius[] = { 1, 1.5, 3, 2, 4 , 4, 2, 3, 2};

	cam = Camera();

	std::vector<std::shared_ptr<Material>> mats = {
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0, 0), glm::vec3(0.7, 0, 0), glm::vec3(1.0, 0, 0))),
		std::shared_ptr<Material>(new Material(glm::vec3(0, 0.02, 0), glm::vec3(0, 0.7, 0), glm::vec3(0, 1.0, 0))),
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0, 0.02), glm::vec3(0.7, 0, 0.7), glm::vec3(0.7, 0, 0.7))),
		std::shared_ptr<Material>(new Material(glm::vec3(0.013, 0.013, 0.035), glm::vec3(0.3, 0.3, 0.8), glm::vec3(0.7, 0.7, 0.7))),
		std::shared_ptr<Material>(new Material(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0, 0, 0))),
		std::shared_ptr<Material>(new Material(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0, 0, 0))),
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0.02, 0.f), glm::vec3(0.8, 0.8, 0.0), glm::vec3(0.5, 0.5, 0))),
		std::shared_ptr<Material>(new Material(glm::vec3(0, 0.f, 0.f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.f, 0.f, 0))),
		std::shared_ptr<Material>(new Material(glm::vec3(0, 0.f, 0.f), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.f, 0.f, 0)))
	};

	mats[2]->setShininess(10.f);
	mats[3]->setShininess(5.f);

	mats[4]->reflective = glm::vec3(0.8f); 
	mats[5]->reflective = glm::vec3(1.0f); // ideal mirror
	mats[7]->reflective = glm::vec3(0.4f); 

	// material for walls
	std::shared_ptr<Material> wall_bot =
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.1, 0.1, 0.1)));
	std::shared_ptr<Material> wall_front =
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.1, 0.1, 0.1)));
	std::shared_ptr<Material> wall_right =
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0.024, 0.016), glm::vec3(0.5, 0.6, 0.4), glm::vec3(0.1, 0.1, 0.1)));
	std::shared_ptr<Material> wall_left =
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.2, 0.4, 0.6), glm::vec3(0.1, 0.1, 0.1)));
	
	wall_bot->reflective = glm::vec3(0.5f);
	for (size_t i = 0; i < sph_origins.size(); ++i)
	{
		sc.emplace_back(std::unique_ptr<Object>(new Sphere(sph_origins[i], radius[i], glm::vec3(0.f), mats[i])));
	}


	//bottom
	sc.emplace_back(std::unique_ptr<Object>(new Rectangle(glm::vec3(-4, 2, -18),
		glm::vec3(50, 0, 0), glm::vec3(0, 30, -30), wall_bot)));
	//front
	sc.emplace_back(std::unique_ptr<Object>(new Rectangle(glm::vec3(-4, 11, -27),
		glm::vec3(50, 0, 0), glm::vec3(0, 70, 0), wall_right)));
	//right
	sc.emplace_back(std::unique_ptr<Object>(new Rectangle(glm::vec3(21, 2, -18),
		glm::vec3(0, 50, 0), glm::vec3(0, 0, 60), wall_right)));
	//left
	sc.emplace_back(std::unique_ptr<Object>(new Rectangle(glm::vec3(-29, 2, -18),
		glm::vec3(0, 50, 0), glm::vec3(0, 0, 60), wall_left)));

	// add lights to the scene
	//lights.emplace_back(std::unique_ptr<Light>(new DistantLight(glm::vec3(-2, -4, -2), glm::vec3(0.8f))));
	//lights.emplace_back(std::unique_ptr<Light>(new DistantLight(glm::vec3(0, 0, -2), glm::vec3(0.8f))));

	lights.emplace_back(std::unique_ptr<Light>(new PointLight(glm::vec3(-4, 24, -26.f),
		glm::vec3(-2, -4, -2), 
		glm::vec3(40.f))));


	cam.setCamToWorld(glm::rotate(glm::translate(glm::mat4(1.f), translation),
		rot_y, glm::vec3(cam.getUpVec())));
	cam.update();
}