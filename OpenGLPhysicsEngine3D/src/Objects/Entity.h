#pragma once
#include <iostream>
#include <glm/glm.hpp>

#include "Objects/Interfaces/IEntity.h"

#include "Structures/ObjectProperties.h"

#include "Enumerators/EntityTypes.h"

class Entity
{
public:
	Entity();
	Entity(unsigned int id, EntityTypes type, std::unique_ptr<IEntity> entity);

	void SetId(unsigned int id) { m_Id = id; }
	unsigned int GetId() { return m_Id; }
	EntityTypes GetType() { return m_Type; }
	IEntity* GetEntity() { return m_Entity.get(); }
	ObjectProperties& GetProperties() { return m_Properties; }

	void Step(float deltaTime);
	void Move(glm::vec3 direction);
	void AddRotation(glm::vec3 rotation);

	IEntity* operator ->() { return m_Entity.get(); }

private:
	unsigned int m_Id;
	EntityTypes m_Type;
	std::unique_ptr<IEntity> m_Entity;
	ObjectProperties m_Properties;
};

