#include "Entity.h"

#include "Physics/PhysicsWorld.h"

Entity::Entity()
	: m_Id(-1)
{
}

Entity::Entity(unsigned int id, EntityTypes type, std::unique_ptr<IEntity> entity)
{
	m_Id = id;
	m_Type = type;
	m_Entity = std::move(entity);
}

void Entity::Step(float deltaTime)
{
	if (m_Properties.rigidbody.isStatic)
		return;

	if(m_Properties.rigidbody.useGravity)
		m_Properties.rigidbody.linearVelocity += PhysicsWorld::GetGravity() * deltaTime;
	else
		m_Properties.rigidbody.linearVelocity += m_Properties.rigidbody.force * deltaTime;

	Move(m_Properties.rigidbody.linearVelocity * deltaTime);

	m_Properties.rigidbody.force = glm::vec3(0);

	AddRotation(m_Properties.rigidbody.angularVelocity * deltaTime);

	m_Entity->ApplyTransform(m_Properties.transform);
}

void Entity::Move(glm::vec3 direction)
{
	m_Properties.transform.translation += direction;
}

void Entity::AddRotation(glm::vec3 rotation)
{
	m_Properties.transform.rotation += rotation;
}

