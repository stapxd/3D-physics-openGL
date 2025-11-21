#pragma once
#include <unordered_map>

#include "Objects/Entity.h"

#include "Structures/EntityParameters.h"

class EntityManager
{
public:
	Entity& AddEntity(EntityTypes type, const EntityParameters& params);
	Entity& FindEntity(unsigned int id);
	void ClearAll();

	void SetEntityPropertiesFromParameters(Entity& entity, const EntityParameters& params);

	std::unordered_map<unsigned int, Entity>& GetEntities() { return m_Entities; }
	const std::unordered_map<unsigned int, Entity>& GetEntities() const { return m_Entities; }

	unsigned int GetSize() const { return m_Size; }
private:
	unsigned int m_Size = 0;
	std::unordered_map<unsigned int, Entity> m_Entities;
};

