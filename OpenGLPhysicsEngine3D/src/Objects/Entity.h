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

	ObjectProperties& GetProperties() { return m_Properties; }
	unsigned int GetId() { return m_Id; }

	IEntity* operator ->() { return m_Entity.get(); }

private:
	unsigned int m_Id;
	std::unique_ptr<IEntity> m_Entity;
	ObjectProperties m_Properties;
};

