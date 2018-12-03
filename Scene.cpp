//////////////////////////////////////////////////////
//	Physics Engine Demonstration
//	Jonathon Booker 
//	2018
//

#include "physics\Scene.h"
#include "physics\Object.h"
#include "physics\Sphere.h"
#include "physics\Plane.h"
#include "physics\Cube.h"
#include <Gizmos.h>
#include <glm\glm.hpp>

using namespace Physics;
using glm::vec3;
using glm::vec4;

Scene::Scene()
{
	// default gravity
	m_gravity = vec3(0.0f, -9.8f, 0.0f);

	// default for fixed time 100fps
	m_fixedTimeStep = 0.01f;
	m_accumulatedTime = 0.0f;
}


Scene::~Scene()
{
	for (auto object : m_objects)
	{
		delete object;
	}
	m_objects.clear();
}

void Scene::Update(float deltaTime)
{
	// using fixed time step for update

	m_accumulatedTime += deltaTime;

	while (m_accumulatedTime >= m_fixedTimeStep)
	{

		applyGravity();
		for (auto object : m_objects)
		{
			object->Update(m_fixedTimeStep);
		}
		m_accumulatedTime -= m_fixedTimeStep;
		detectCollisions();
		resolveCollisions();
	}
}

void Scene::draw()
{
	for (auto object : m_objects)
	{
		object->draw();
	}
}


void Scene::AddObject(Object * object)
{
	m_objects.push_back(object);
}

void Scene::RemoveObject(Object * object)
{
	auto iter = std::find(m_objects.begin(), m_objects.end(), object);
	if (iter != m_objects.end())
	{
		m_objects.erase(iter);
	}
	delete object;
}

void Scene::applyGlobalForce()
{
	for (auto object : m_objects)
	{
		object->ApplyForce(m_globalForce);
	}
}

void Scene::applyGravity()
{
	for (auto object : m_objects)
	{
		// gravity applies force based on mass
		object->ApplyForce(m_gravity * object->getMass());
	}
}

void Scene::detectCollisions()
{
	// find all the collisions and place them in the collisions vector
	for (auto object = m_objects.begin(); object != m_objects.end(); object++)
	{
		for (auto object2 = object + 1; object2 != m_objects.end(); object2++)
		{
			Object* objectA = static_cast<Object*>(*object);
			Object* objectB = static_cast<Object*>(*object2);

			Collision tempCollision;

			if ((*object)->isColliding(*object2, tempCollision))
			{

				tempCollision.objA = *object;
				tempCollision.objB = *object2;
				m_collisions.push_back(tempCollision);
			}
		}
	}
}

void Physics::Scene::resolveCollisions()
{

	for (auto col : m_collisions)
	{

		// create a normalized collision vector

		vec3 colNorm = glm::normalize(col.collisionNormal);

		// calculate the relative velocity

		vec3 relVel = (col.objA->getVelocity() - col.objB->getVelocity());

			
		// find out  how much of the relative velocity goes along the collision vector

		// start putting together the impulse force:
		// elasticity (this will be an object variable, might need to average it from both objects)
		// get the formula from our resources and calculate J
		// J = (-1 + elasticity) * relative velocity) . collision normal / 
		// Collision normal.collision normal * ((1 / massA + 1 / massB))

		if ((col.objA->getIsStatic() == false) && (col.objB->getIsStatic() == false))
		{

			float j = glm::dot(-(1 + ((col.objA->getElasticity() + col.objB->getElasticity()) / 2)) * (relVel), colNorm) /
					  glm::dot(colNorm, colNorm * ((1 / col.objA->getMass()) + (1 / col.objB->getMass())));

			vec3 force = colNorm * j;

			col.objA->ApplyImpulse(force);
			col.objB->ApplyImpulse(-force);

			col.objA->setPosition(col.objA->getPosition() + (colNorm * (col.seperation / 2)));
			col.objB->setPosition(col.objB->getPosition() + (-colNorm * (col.seperation / 2)));

		}
		else if (col.objB->getIsStatic() == true)
		{
			vec3 statVel = col.objA->getVelocity() - (1 + ((col.objA->getElasticity() + col.objB->getElasticity()) / 2)) * (glm::dot(col.objA->getVelocity(), colNorm) * colNorm);

			col.objA->setVelocity(statVel * 0.9f);
			col.objA->setPosition(col.objA->getPosition() + (colNorm * (col.seperation / 2)));
		}
		else if (col.objA->getIsStatic() == true)
		{
			vec3 statVel = col.objB->getVelocity() - (1 + ((col.objB->getElasticity() + col.objA->getElasticity()) / 2)) * (glm::dot(col.objB->getVelocity(), colNorm) * colNorm);

			col.objB->setVelocity(statVel * 0.9f);
			col.objB->setPosition(col.objB->getPosition() + (colNorm * (col.seperation / 2)));
		}
		
	}
	m_collisions.clear();
}
