#include "Entity.h"

#include "Physics/PhysicsWorld.h"

Entity::Entity()
	: m_Id(-1)
{
}

Entity::Entity(unsigned int id, std::unique_ptr<IEntity> entity)
{
	m_Id = id;
	m_Entity = std::move(entity);
}

void Entity::Step(float deltaTime)
{
	if (m_Properties.rigidbody.isStatic)
		return;

	//m_Properties.rigidBody.linearVelocity += deltaTime * PhysicsWorld::GetGravity();
	
	m_Properties.rigidbody.linearVelocity += m_Properties.rigidbody.force * deltaTime;

	Move(m_Properties.rigidbody.linearVelocity);

	m_Properties.rigidbody.force = glm::vec3(0);

	m_Entity->ApplyTransform(m_Properties.transform);
}

void Entity::Move(glm::vec3 direction)
{
	m_Properties.transform.translation += direction;
}

