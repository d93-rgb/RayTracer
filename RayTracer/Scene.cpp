// obj file loader stuff
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "scene.h"
#include "camera.h"
#include "loader.h"
#include "material.h"
#include "shape.h"
#include "quadrics.h"
#include "light.h"

//#define LOAD_TEAPOT
// DEBUGGING
// divide the triangle mesh of the teapot to reduce rendering time
constexpr auto TEAPOTSIZE = 1;

namespace rt
{

Scene::Scene() = default;

Scene::~Scene() = default;

void GatheringScene::init()
{
	cam.reset(new Camera());

	float rot_y = glm::radians(0.f);
	float radius[] = { 1, 1.5, 3, 2, 4 , 4, 2, 3, 2 };

	glm::vec3 translation = glm::vec3(0.f, 3.f, 20.f);

	std::vector<glm::vec3> sph_origins = {
		glm::vec3(-10, -2, -5),
		glm::vec3(-9, 21, -22),
		glm::vec3(9, 3, -15),
		glm::vec3(-11, 7, -15),
		glm::vec3(0, 12, -25),
		glm::vec3(5, -2, -11),
		glm::vec3(-6, -3, -4),
		glm::vec3(-8, 4, -2),
		//glm::vec3(0.f, 0.f, 0.f)
	};
	std::unique_ptr<Shape> cube_1[6], cube_2[6], cube_3[6];


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
		sc.emplace_back(std::unique_ptr<Shape>(new Sphere(sph_origins[i], radius[i], glm::vec3(0.f), mats[i])));
	}

	//bottom
	sc.emplace_back(std::unique_ptr<Shape>(new Rectangle(glm::vec3(-4, 2, -18),
		glm::vec3(150, 0, 0), glm::vec3(0, 150, -150), wall_bot)));

	// get pointer to the floor
	Rectangle *floor = dynamic_cast<Rectangle*>(sc.back().get());

	//front
	//sc.emplace_back(std::unique_ptr<Shape>(new Rectangle(glm::vec3(-4, 11, -27),
	//	glm::vec3(50, 0, 0), glm::vec3(0, 70, 0), wall_right)));
	////right
	//sc.emplace_back(std::unique_ptr<Shape>(new Rectangle(glm::vec3(21, 2, -18),
	//	glm::vec3(0, 50, 0), glm::vec3(0, 0, -60), wall_right)));
	////left
	//sc.emplace_back(std::unique_ptr<Shape>(new Rectangle(glm::vec3(-29.f, 2, -18),
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


	glm::vec4 cube_position = floor->getRectPos(13.f, -25.f, 'y');
	glm::vec3 cube_normal = floor->get_normal();

	create_cube(cube_position + 2.f * glm::vec4(cube_normal, 0.f),
		cube_normal,
		glm::vec3(1.f, 0.f, 0.f),
		4.f,
		cube_1,
		cube_mat_1);
	create_cube(glm::vec3(14.f, 8.f, -3.f),
		//glm::rotate(glm::mat4(1), -30.f, glm::vec3(1.f, 0.f, 0.f)) *
		//glm::vec4(0.f, 0.f, 1.f, 1.f),
		cube_normal,
		/*glm::rotate(glm::mat4(1), -30.f, glm::vec3(1.f, 0.f, 0.f)) *
		glm::vec4(1.f, 0.f, 0.f, 1.f),*/
		glm::perp(glm::vec3(1.f, 1.f, 0.f), cube_normal),
		6.f,
		cube_2,
		cube_mat_2);
	/*create_cube(glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f),
		5.f,
		cube_3,
		cube_mat_2);
	*/

	////////////////////////////////
	// NEW CUBE
	////////////////////////////////
	// cube material for new cube class object
	auto new_cube_mat = std::shared_ptr<Material>(new Material(glm::vec3(0.01f, 0.02f, 0.005f),
		glm::vec3(0.2f, 0.6f, 0.1f),
		glm::vec3(0.2f, 0.6f, 0.1f)));

	sc.emplace_back(std::unique_ptr<Shape>(new Cube(glm::vec3(3.f), new_cube_mat)));
	sc.back()->obj_to_world = glm::rotate(glm::scale(glm::translate(
		glm::mat4(1.f),
		glm::vec3(0.f, -1.f, 10.f)),
		glm::vec3(1.25f, 0.5f, 1.f)),
		glm::radians(60.f),
		glm::vec3(1.f, 0.f, 0.f));

	sc.back()->world_to_obj = glm::inverse(sc.back()->obj_to_world);
	////////////////////////////////
	// END
	////////////////////////////////

	for (std::unique_ptr<Shape> &p : cube_1)
	{
		sc.emplace_back(std::move(p));
	}
	for (std::unique_ptr<Shape> &p : cube_2)
	{
		sc.emplace_back(std::move(p));
	}
	/*for (std::unique_ptr<Shape> &p : cube_3)
	{
		sc.emplace_back(std::move(p));
	}*/
	// add lights to the scene
	//lights.emplace_back(std::unique_ptr<Light>(new DistantLight(glm::vec3(-2, -4, -2), glm::vec3(0.8f))));
	//lights.emplace_back(std::unique_ptr<Light>(new DistantLight(glm::vec3(0, 0, -1), glm::vec3(0.8f))));

	lights.emplace_back(std::unique_ptr<Light>(new PointLight(glm::vec3(-2.f, 4.f, -17.f),
		glm::vec3(-2, -4, -2),
		glm::vec3(100.f))));


	cam->setCamToWorld(glm::rotate(glm::translate(glm::mat4(1.f), translation),
		rot_y, glm::vec3(cam->getUpVec())));
	cam->update();
}

void SingleCubeScene::init()
{
	float rot_x = glm::radians(0.f);
	float deb;
	glm::vec3 translation = glm::vec3(-2.f, -2.f, 18.f);
	glm::vec4 cube_position;
	glm::vec3 cube_normal;
	glm::vec3 p1, p2, p3, tr_normal;

	std::string teapot =
		"C:\\Users\\Dood\\Documents\\ComputerGraphics\\models\\teapot.obj";
	
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	glm::mat4 teapot_to_world = glm::rotate(
		glm::scale(
			//glm::mat4(1.f),
			glm::translate(glm::mat4(1.f), glm::vec3(-6.f, 0.f, 2.f)),
			glm::vec3(1.5f)),
		glm::radians(30.f),
		glm::vec3(0.f, 1.f, 0.f));
	
	Rectangle *floor;
	std::unique_ptr<Shape> cube_2[6];

	/////////////////////////////////////
	// Triangle mesh
	/////////////////////////////////////
#ifdef LOAD_TEAPOT
	std::unique_ptr<TriangleMesh> t_pot{ new TriangleMesh() };
	glm::vec3 b_min = glm::vec3(INFINITY), b_max = glm::vec3(-INFINITY);
	
	extractMesh(teapot, vertices, indices);

	std::shared_ptr<Material> teapot_mat =
		std::shared_ptr<Material>(
			new Material(glm::vec3(0.02f, 0.f, 0.02f),
				glm::vec3(0.4f, 0.f, 0.4f),
				glm::vec3(0.05f, 0.05f, 0.05f)));
	teapot_mat->setShininess(20.f);

	for (size_t i = 0; i < indices.size() / TEAPOTSIZE; i += 3)
	{
		p1 = vertices[indices[i]];
		p2 = vertices[indices[i + 1]];
		p3 = vertices[indices[i + 2]];
		tr_normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));

		// get boundaries of the triangle mesh
		b_min = glm::min(b_min, glm::min(glm::min(p1, p2), p3));
		b_max = glm::max(b_max, glm::max(glm::max(p1, p2), p3));

		t_pot->tr_mesh.push_back(std::unique_ptr<Triangle>(new Triangle(p1,
			p2,
			p3,
			tr_normal,
			teapot_to_world,
			teapot_mat)));
	}
	/////////////////////////////////////
	// Triangle mesh END
	/////////////////////////////////////


	////////////////////////////////
	// BOUNDARY FOR THE TEAPOT
	////////////////////////////////
	t_pot->boundary.reset(new Bounds3(
		teapot_to_world * glm::vec4(b_min, 1.f), 
		teapot_to_world * glm::vec4(b_max, 1.f)));

	// put triangle mesh into scene
	sc.emplace_back(std::move(t_pot));
	////////////////////////////////
	// END
	////////////////////////////////
#endif

	// material for walls
	std::shared_ptr<Material> wall_bot =
		std::shared_ptr<Material>(
			new Material(glm::vec3(0.02, 0.02, 0.02),
				glm::vec3(0.4, 0.4, 0.4),
				glm::vec3(0.0, 0.0, 0.0)));
	wall_bot->reflective = glm::vec3(0.2f);

	//bottom
	sc.emplace_back(std::unique_ptr<Shape>(new Rectangle(glm::vec3(-4, 2, -18),
		glm::vec3(150, 0, 0), glm::vec3(0, 150, -150), wall_bot)));

	// get pointer to the floor
	floor = dynamic_cast<Rectangle*>(sc.back().get());

	cube_position = floor->getRectPos(1.5f, -27.f, 'y');
	cube_normal = floor->get_normal();

	deb = glm::dot(cube_normal, glm::vec3(-2.f, 4.f, -17.f) - floor->center);

	std::shared_ptr<Material> triangle_mat_1 =
		std::shared_ptr<Material>(new Material(
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f)));
	std::shared_ptr<Material> cube_mat_2 =
		std::shared_ptr<Material>(new Material(
			glm::vec3(0.02, 0.02, 0.0),
			glm::vec3(0.9, 0.2, 0.0f),
			glm::vec3(0.6, 0.0, 0.6)));
	cube_mat_2->setShininess(10.f);

	create_cube(cube_position + glm::vec4((3.f * cube_normal), 1.f),
		//glm::rotate(glm::mat4(1), -30.f, glm::vec3(1.f, 0.f, 0.f)) *
		//glm::vec4(0.f, 0.f, 1.f, 1.f),
		cube_normal,
		/*glm::rotate(glm::mat4(1), -30.f, glm::vec3(1.f, 0.f, 0.f)) *
		glm::vec4(1.f, 0.f, 0.f, 1.f),*/
		glm::perp(glm::vec3(1.f, 1.f, 0.f), cube_normal),
		6.f,
		cube_2,
		cube_mat_2);

	for (std::unique_ptr<Shape> &p : cube_2)
	{
		sc.emplace_back(std::move(p));
	}


	/////////////////////////////////////
	// Single Triangle
	/////////////////////////////////////
	sc.emplace_back(std::unique_ptr<Shape>(new Triangle(glm::vec3(0.f, 0.f, -4.f),
		glm::vec3(4.f, 0.f, -4.f),
		glm::vec3(4.f, 4.f, -4.f),
		glm::vec3(0.f, 0.f, 1.f),
		glm::scale(glm::mat4(1.f), glm::vec3(2.f)),
		triangle_mat_1)));
	dynamic_cast<Triangle*>(sc.back().get())->setInterpolate(true);
	
	/////////////////////////////////////
	// Sphere
	/////////////////////////////////////
	std::shared_ptr<Material> sphere_mat =
		std::shared_ptr<Material>(new Material(
			glm::vec3(0.02, 0.02, 0.02),
			glm::vec3(0.3, 0.3, 0.3f),
			glm::vec3(0.4f, 0.4f, 0.4f)));
	sphere_mat->setShininess(10.f);

	sc.emplace_back(std::unique_ptr<Shape>(new Sphere(
		glm::vec3(-3.f, 0.f, -7.f),
		2.f,
		glm::vec3(1.f),
		sphere_mat,
		std::shared_ptr<Texture>(new CheckerBoardTexture()))));

	// red sphere at origin
	sphere_mat =
		std::shared_ptr<Material>(new Material(
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f)));

	sc.emplace_back(std::unique_ptr<Shape>(new Sphere(
		glm::vec3(0.f, 0.f, 0.f),
		0.1f,
		glm::vec3(1.f),
		sphere_mat)));

	sphere_mat =
		std::shared_ptr<Material>(new Material(
			glm::vec3(0.02, 0.0f, 0.02),
			glm::vec3(0.3, 0.f, 0.4f),
			glm::vec3(0.4f, 0.4f, 0.4f)));
	sphere_mat->setShininess(20.f);

	sc.emplace_back(std::unique_ptr<Shape>(new Sphere(
		glm::vec3(-6.f, 6.f, -3.f),
		1.f,
		glm::vec3(1.f),
		sphere_mat)));

	sphere_mat =
		std::shared_ptr<Material>(new Material());
	sphere_mat->reflective = glm::vec3(1.f);

	sc.emplace_back(std::unique_ptr<Shape>(new Sphere(
		glm::vec3(-5.f, -4.f, 4.f),
		1.5f,
		glm::vec3(1.f),
		sphere_mat)));

	sphere_mat =
		std::shared_ptr<Material>(new Material());
	sphere_mat->transparent = glm::vec3(1.f);

	sc.emplace_back(std::unique_ptr<Shape>(new Sphere(
		glm::vec3(5.f, -3.f, 4.f),
		2.f,
		glm::vec3(1.f),
		sphere_mat)));

	/////////////////////////////////////
	// Cylinders
	/////////////////////////////////////
	float cyl_rad = 0.1f;
	std::shared_ptr<Material> cylinder_mat_1 =
		std::shared_ptr<Material>(new Material(
			glm::vec3(0.009f, 0.002f, 0.f),
			glm::vec3(0.9f, 0.2f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f)));
	cylinder_mat_1->setShininess(20.f);

	sc.emplace_back(std::unique_ptr<Shape> (new Cylinder(
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 0.f, 0.f),
		cyl_rad,
		8.f,
		cylinder_mat_1)));
	sc.emplace_back(std::unique_ptr<Shape>(new Cylinder(
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f),
		cyl_rad,
		8.f,
		cylinder_mat_1)));
	sc.emplace_back(std::unique_ptr<Shape>(new Cylinder(
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 0.f, 1.f),
		cyl_rad,
		8.f,
		cylinder_mat_1)));
	/*dynamic_cast<Cylinder*>(sc.back().get())->worldToObj =
		glm::inverse(
			glm::translate(
			glm::rotate(glm::mat4(1.f), glm::radians(60.f), glm::vec3(1.f, 0.f, 0.f)),
			glm::vec3(5.f, 1.f, 2.f)));*/
	//dynamic_cast<Cylinder*>(sc.back().get())->worldToObj = glm::mat4(1.f);
	
	/////////////////////////////////////
	// Lights
	/////////////////////////////////////
	lights.emplace_back(std::unique_ptr<Light>(new PointLight(glm::vec3(-2.f, 20.f, -5.f),
		glm::vec3(-2, -4, -2),
		glm::vec3(80.f))));
	/*lights.emplace_back(std::unique_ptr<Light>(new PointLight(glm::vec3(0.f, 0.f, 10.f),
		glm::vec3(0.f, 0.f, -1.f),
		glm::vec3(30.f))));
*/
	/////////////////////////////////////
	// Lights END
	/////////////////////////////////////

	/////////////////////////////////////
	// Camera
	/////////////////////////////////////
	cam.reset(new Camera());
	cam->setCamToWorld(glm::rotate(glm::translate(glm::mat4(1.f), translation),
		rot_x, glm::vec3(0.f, 1.f, 0.f)));
	cam->update();
	/////////////////////////////////////
	// Camera END
	/////////////////////////////////////

}
}