#include "Entity.h"

Entity::Entity()
	: m_Id(-1)
{
}

Entity::Entity(unsigned int id, std::unique_ptr<IEntity> entity)
{
	m_Id = id;
	m_Entity = std::move(entity);
}

