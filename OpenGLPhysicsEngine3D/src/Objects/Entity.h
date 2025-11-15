#pragma once
#include <iostream>

#include "Objects/Interfaces/IEntity.h"

#include "Structures/ObjectProperties.h"

#include "Enumerators/EntityTypes.h"

class Entity
{
public:
	Entity() 
		{}
	Entity(std::unique_ptr<IEntity> entity);

	ObjectProperties& GetProperties() { return m_Properties; }

	IEntity* operator ->() { return m_Entity.get(); }

private:
	std::unique_ptr<IEntity> m_Entity;
	ObjectProperties m_Properties;
};

