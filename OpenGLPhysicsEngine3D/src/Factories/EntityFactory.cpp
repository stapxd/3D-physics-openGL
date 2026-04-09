#include "EntityFactory.h"

#include "Objects/Cube.h"
#include "Objects/Sphere.h"

std::unique_ptr<IEntity> EntityFactory::CreateEntity(EntityTypes type, const ObjectProperties& properties)
{
	switch (type)
	{
	case EntityTypes::Cube:
		return std::make_unique<Cube>();
	case EntityTypes::Sphere:
		return std::make_unique<Sphere>(properties.transform.scale.x);
	default:
		return nullptr;
	}
}
