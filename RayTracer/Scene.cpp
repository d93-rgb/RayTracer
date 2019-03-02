#include "scene.h"


void Scene::init()
{
	float rot_y = glm::radians(20.f);
	float radius[] = { 1, 1.5, 3, 2, 4 , 4, 2, 3, 2 };

	glm::vec3 translation = glm::vec3(6.f, 0.f, 20.f);
	std::vector<glm::vec3> sph_origins = {
		glm::vec3(-10, -2, -5),
		glm::vec3(-9, 21, -22),
		glm::vec3(9, 3, -15),
		glm::vec3(-11, 7, -15),
		glm::vec3(0, 12, -25),
		glm::vec3(5, -2, -11),
		glm::vec3(-6, -3, -4),
		glm::vec3(-4, 4, -2),
		//glm::vec3(-4, 20, -15.f)
	};
	std::unique_ptr<Object> cube_1[6], cube_2[6];

	cam = Camera();

	std::vector<std::shared_ptr<Material>> mats = {
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0, 0), glm::vec3(0.7, 0, 0), glm::vec3(1.0, 0, 0))),
		std::shared_ptr<Material>(new Material(glm::vec3(0, 0.02, 0), glm::vec3(0, 0.7, 0), glm::vec3(0, 1.0, 0))),
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0, 0.02), glm::vec3(0.7, 0, 0.7), glm::vec3(0.7, 0, 0.7))),
		std::shared_ptr<Material>(new Material(glm::vec3(0.013, 0.013, 0.035), glm::vec3(0.3, 0.3, 0.8), glm::vec3(0.7, 0.7, 0.7))),
		std::shared_ptr<Material>(new Material(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0, 0, 0))),
		std::shared_ptr<Material>(new Material(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0, 0, 0))),
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0.02, 0.f), glm::vec3(0.8, 0.8, 0.0), glm::vec3(0.5, 0.5, 0))),
		std::shared_ptr<Material>(new Material(glm::vec3(0.0, 0.0f, 0.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0f, 0.0f, 0.0f))),
		std::shared_ptr<Material>(new Material(glm::vec3(0, 0.f, 0.f), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.f, 0.f, 0.f)))
	};

	mats[2]->setShininess(10.f);
	mats[3]->setShininess(5.f);

	mats[4]->reflective = glm::vec3(0.8f);
	mats[5]->reflective = glm::vec3(1.0f); // ideal mirror
	
	// glass sphere
	mats[7]->transparent = glm::vec3(1.f);
	mats[7]->setRefractiveIdx(1.5f);

	// material for walls
	std::shared_ptr<Material> wall_bot =
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.0, 0.0, 0.0)));
	std::shared_ptr<Material> wall_front =
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.1, 0.1, 0.1)));
	std::shared_ptr<Material> wall_right =
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0.024, 0.016), glm::vec3(0.5, 0.6, 0.4), glm::vec3(0.1, 0.1, 0.1)));
	std::shared_ptr<Material> wall_left =
		std::shared_ptr<Material>(new Material(glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.2, 0.4, 0.6), glm::vec3(0.1, 0.1, 0.1)));

	wall_bot->reflective = glm::vec3(0.2f);

	for (size_t i = 0; i < sph_origins.size(); ++i)
	{
		sc.emplace_back(std::unique_ptr<Object>(new Sphere(sph_origins[i], radius[i], glm::vec3(0.f), mats[i])));
	}

	//bottom
	sc.emplace_back(std::unique_ptr<Object>(new Rectangle(glm::vec3(-4, 2, -18),
		glm::vec3(150, 0, 0), glm::vec3(0, 150, -150), wall_bot)));
	//front
	//sc.emplace_back(std::unique_ptr<Object>(new Rectangle(glm::vec3(-4, 11, -27),
	//	glm::vec3(50, 0, 0), glm::vec3(0, 70, 0), wall_right)));
	////right
	//sc.emplace_back(std::unique_ptr<Object>(new Rectangle(glm::vec3(21, 2, -18),
	//	glm::vec3(0, 50, 0), glm::vec3(0, 0, -60), wall_right)));
	////left
	//sc.emplace_back(std::unique_ptr<Object>(new Rectangle(glm::vec3(-29.f, 2, -18),
	//	glm::vec3(0, 50, 0), glm::vec3(0, 0, 60), wall_left)));

	// cube material
	std::shared_ptr<Material> cube_mat_1 =
		std::shared_ptr<Material>(new Material(
			glm::vec3(0.02, 0.0, 0.02), 
			glm::vec3(0.1, 0.0, 0.7f), 
			glm::vec3(0.6, 0.0, 0.6)));
	std::shared_ptr<Material> cube_mat_2 =
		std::shared_ptr<Material>(new Material(
			glm::vec3(0.02, 0.02, 0.0),
			glm::vec3(0.9, 0.2, 0.0f),
			glm::vec3(0.6, 0.0, 0.6)));
	cube_mat_1->setShininess(10.f);
	cube_mat_2->setShininess(10.f);

	create_cube(glm::vec3(13.f, 10.f, -20.f),
		glm::vec3(0.f, 0.f, 1.f),
		glm::vec3(1.f, 0.f, 0.f),
		4.f,
		cube_1,
		cube_mat_1);
	create_cube(glm::vec3(14.f, 8.f, -3.f),
		glm::rotate(glm::mat4(1), -30.f, glm::vec3(1.f, 0.f, 0.f)) * 
		glm::vec4(0.f, 0.f, 1.f, 1.f),
		glm::rotate(glm::mat4(1), -30.f, glm::vec3(1.f, 0.f, 0.f)) *
		glm::vec4(1.f, 0.f, 0.f, 1.f),
		6.f,
		cube_2,
		cube_mat_2);

	// cube material for new cube class object
	std::shared_ptr<Material>(new Material(glm::vec3(0, 0.f, 0.f),
		glm::vec3(0.2f, 0.6f, 0.1f),
		glm::vec3(0.2f, 0.6f, 0.1f)));

	sc.emplace_back(std::unique_ptr<Object>(new Cube(glm::vec3(4.f))));

	for (std::unique_ptr<Object> &p : cube_1)
	{
		sc.emplace_back(std::move(p));
	}
	for (std::unique_ptr<Object> &p : cube_2)
	{
		sc.emplace_back(std::move(p));
	}

	// add lights to the scene
	//lights.emplace_back(std::unique_ptr<Light>(new DistantLight(glm::vec3(-2, -4, -2), glm::vec3(0.8f))));
	//lights.emplace_back(std::unique_ptr<Light>(new DistantLight(glm::vec3(0, 0, -1), glm::vec3(0.8f))));

	lights.emplace_back(std::unique_ptr<Light>(new PointLight(glm::vec3(-2.f, 4.f, -17.f),
		glm::vec3(-2, -4, -2),
		glm::vec3(100.f))));


	cam.setCamToWorld(glm::rotate(glm::translate(glm::mat4(1.f), translation),
		rot_y, glm::vec3(cam.getUpVec())));
	cam.update();
}

void Scene::create_cube(glm::vec3 center,
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

	glm::vec3 t_u	= tmp * n_up;
	glm::vec3 t_f	= tmp * n_front;
	glm::vec3 t_uf	= tmp * n_u_cross_f;

	n_up			= s_len * n_up;
	n_front			= s_len * n_front;
	n_u_cross_f		= s_len * n_u_cross_f;

	//top
	sides[0].reset(new Rectangle(center + t_u, -n_u_cross_f , n_front, mat));
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
