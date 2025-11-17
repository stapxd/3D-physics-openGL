#include "EntityFactory.h"

#include "Objects/Cube.h"

std::unique_ptr<IEntity> EntityFactory::CreateEntity(EntityTypes type)
{
	switch (type)
	{
	case EntityTypes::Cube:
		return std::make_unique<Cube>();
	default:
		return nullptr;
	}
}
