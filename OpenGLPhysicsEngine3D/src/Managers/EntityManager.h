#pragma once
#include <unordered_map>

#include "Objects/Entity.h"

#include "Structures/EntityParameters.h"

class EntityManager
{
public:
	void AddEntity(EntityTypes type, const EntityParameters& params);
	Entity& FindEntity(unsigned int id);

	std::unordered_map<unsigned int, Entity>& GetEntities() { return m_Entities; }
private:
	unsigned int m_Size = 0;
	std::unordered_map<unsigned int, Entity> m_Entities;
};

