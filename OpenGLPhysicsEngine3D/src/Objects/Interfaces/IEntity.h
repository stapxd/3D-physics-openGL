#pragma once
#include "IMesh.h"
#include "IPhysicsBody.h"

#include "Objects/Parts/Mesh.h"
#include "Objects/Parts/PhysicsBody.h"

class IEntity : public IMesh, public IPhysicsBody {
public:
	virtual Mesh* GetMesh() = 0;
	virtual PhysicsBody* GetPhysicsBody() = 0;
};