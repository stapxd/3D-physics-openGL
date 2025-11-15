#include "EntityFactory.h"

#include "Objects/Cube.h"

std::unique_ptr<IEntity> EntityFactory::CreateEntity(EntityTypes type, const EntityParameters params)
{
	switch (type)
	{
	case EntityTypes::Cube:
		return std::make_unique<Cube>(params.isStatic);
	case EntityTypes::StaticCube:
		return std::make_unique<Cube>(true);
	default:
		return nullptr;
	}
}
