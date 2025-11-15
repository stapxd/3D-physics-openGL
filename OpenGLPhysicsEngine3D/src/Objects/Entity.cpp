#include "Entity.h"

Entity::Entity(std::unique_ptr<IEntity> entity)
{
	m_Entity = std::move(entity);
}

