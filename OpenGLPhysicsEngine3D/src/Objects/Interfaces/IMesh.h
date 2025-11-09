#pragma once
#include <glm/glm.hpp>
#include "OpenGL/Shader.h"

#include "Structures/AABB.h"
#include "Structures/OBB.h"

class IMesh {
public:
	virtual void InitializeMesh() = 0;

	virtual void Scale(glm::vec3 scale) = 0;
	virtual void Rotate(glm::vec3 rotation) = 0;
	virtual void Translate(glm::vec3 translation) = 0;

	virtual void Move(glm::vec3 translation) = 0;

	virtual AABB GetAABB() = 0;
	virtual OBB  GetOBB() = 0;

	virtual void Draw(const Shader& shader) = 0;
};