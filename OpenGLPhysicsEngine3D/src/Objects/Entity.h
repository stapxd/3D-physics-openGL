#pragma once
#include <iostream>

#include "Objects/Interfaces/IEntity.h"

#include "Structures/ObjectProperties.h"

#include "Enumerators/EntityTypes.h"

class Entity
{
public:
	Entity();
	Entity(unsigned int id, std::unique_ptr<IEntity> entity);

	unsigned int GetId() { return m_Id; }
	IEntity* GetEntity() { return m_Entity.get(); }
	ObjectProperties& GetProperties() { return m_Properties; }

	IEntity* operator ->() { return m_Entity.get(); }

private:
	unsigned int m_Id;
	std::unique_ptr<IEntity> m_Entity;
	ObjectProperties m_Properties;
};

