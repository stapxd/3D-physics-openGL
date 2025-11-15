#pragma once
#include <iostream>

#include "Objects/Interfaces/IEntity.h"
#include "Enumerators/EntityTypes.h"
#include "Structures/EntityParameters.h"

class EntityFactory
{
public:
	static std::unique_ptr<IEntity> CreateEntity(EntityTypes type, const EntityParameters params);
};

