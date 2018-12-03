//////////////////////////////////////////////////////
//	Physics Engine Demonstration
//	Jonathon Booker 
//	2018
//

#include "PhysicsEngineApp.h"
#include "Camera.h"

#include "physics\Object.h"
#include "physics\Scene.h"
#include "physics\Sphere.h"
#include "physics\Cube.h"
#include "physics\Plane.h"

#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <imgui.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;



PhysicsEngineApp::PhysicsEngineApp() {

}

PhysicsEngineApp::~PhysicsEngineApp() {

}

bool PhysicsEngineApp::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(100000, 100000, 100000, 100000);

	// create simple camera transforms
	m_camera = new Camera();
	m_camera->SetProjection(glm::radians(45.0f), (float)getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.0f);
	m_camera->SetPosition(glm::vec3(40.f, 40.f, 40.f));
	m_camera->Lookat(glm::vec3());

	// create an object
	m_scene = new Physics::Scene();
	m_scene->setGlobalForce(vec3(0, 0, 0));
	m_scene->setGravity(vec3(0, -20, 0));


	Physics::Cube *m_border = new Physics::Cube(vec3(55, 100, 0), vec3(5, 100, 50), 50, vec4(1, 0, 0, 0.5f), true);
	m_scene->AddObject(m_border);

	Physics::Cube *m_border2 = new Physics::Cube(vec3(-55, 100, 0), vec3(5, 100, 50), 50, vec4(1, 0, 0, 0.5f), true);
	m_scene->AddObject(m_border2);

	Physics::Cube *m_border3 = new Physics::Cube(vec3(0, 100, 55), vec3(50, 100, 5), 50, vec4(1, 0, 0, 0.5f), true);
	m_scene->AddObject(m_border3);

	Physics::Cube *m_border4 = new Physics::Cube(vec3(0, 100, -55), vec3(50, 100, 5), 50, vec4(1, 0, 0, 0.5f), true);
	m_scene->AddObject(m_border4);

	//plane
	Physics::Plane *m_plane = new Physics::Plane(vec3(50, 0, 50), vec3(2, 10, 2), 50, vec4(1, 0, 0, 1), true);
	m_scene->AddObject(m_plane);




	return true; 
}

void PhysicsEngineApp::shutdown() {
	delete m_scene;
	delete m_camera;
	Gizmos::destroy();
}

void PhysicsEngineApp::update(float deltaTime) {
	
	m_camera->Update(deltaTime);

	ImGui::SetNextWindowSize(ImVec2(300,300), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(5,5),ImGuiSetCond_Once);
	ImGui::Begin("Psychic Demo");
	//ImGui::Text("xPos: %f", m_sphere->getPosition().x);
	//ImGui::Text("yPos: %f", m_sphere->getPosition().y);
	//ImGui::Text("zPos: %f", m_sphere->getPosition().z);
	

	static float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	static float position[3] = { 0.0f, 0.0f, 0.0f };
	static float size[3] = { 1.0f, 1.0f, 1.0f };
	static float velocity = 50.0f;
	static float radius = 1.0f;
	static float mass = 1.0f;
	static bool isStatic = false;
	



		ImGui::InputFloat4("Color", color, 1);
		ImGui::InputFloat3("Position", position, 2);
		ImGui::InputFloat("Mass", &mass, 1);
		ImGui::Checkbox("Static", &isStatic);


		if (ImGui::CollapsingHeader("Sphere"))
		{
			ImGui::InputFloat("Sphere Radius", &radius, 1);
			if (ImGui::Button("Add Sphere"))
			{
				Physics::Sphere *m_dynamicSphere = new Physics::Sphere(vec3(position[0], position[1], position[2]), radius, mass, vec4(color[0], color[1], color[2], color[3]), isStatic);
				m_scene->AddObject(m_dynamicSphere);
			}
		}

		if (ImGui::CollapsingHeader("Cube"))
		{
			ImGui::InputFloat3("Cube Size", size, 2);

			if (ImGui::Button("Add Cube"))
			{
				Physics::Cube *m_staticCube = new Physics::Cube(vec3(position[0], position[1], position[2]), vec3(size[0], size[1], size[2]), mass, vec4(color[0], color[1], color[2], color[3]), true);
				m_scene->AddObject(m_staticCube);
			}
		}

		if (ImGui::CollapsingHeader("Shoot a Sphere"))
		{

			

			ImGui::InputFloat("Velocity", &velocity, 10);

			if (ImGui::Button("Shoot Sphere"))
			{

				Physics::Sphere *m_shootSphere = new Physics::Sphere(vec3(m_camera->GetPosition()), 2, 20, vec4((rand() % 255) / 255.f, (rand() % 255) / 255.f, (rand() % 255) / 255.f, 1.f), false);
				m_shootSphere->setVelocity(m_camera->GetFront() * velocity);
				m_scene->AddObject(m_shootSphere);
			}
		}
	
	ImGui::End();



	// wipe the gizmos clean for this frame
	Gizmos::clear();

	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
						vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
						vec3(-10, 0, -10 + i),
						i == 10 ? white : black);
	}

	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();


	m_scene->applyGlobalForce();

	m_scene->Update(deltaTime);
	//aie::Gizmos::addSphere(m_object->getPosition(), 5, 8, 8, vec4(1, 1, 0, 1));
}

void PhysicsEngineApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// ask the scene to add all gizmos
	m_scene->draw();

	// update perspective based on screen size
	Gizmos::draw(m_camera->GetProjectionView());
}