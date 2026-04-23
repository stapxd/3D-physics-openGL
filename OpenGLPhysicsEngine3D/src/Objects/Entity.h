#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Objects/Interfaces/IEntity.h"

#include "Structures/ObjectProperties.h"

#include "Enumerators/EntityTypes.h"

class Entity
{
public:
	Entity();
	Entity(unsigned int id, EntityTypes type, std::unique_ptr<IEntity> entity);

	void SetId(unsigned int id) { m_Id = id; }
	unsigned int GetId() { return m_Id; }
	EntityTypes GetType() { return m_Type; }
	IEntity* GetEntity() { return m_Entity.get(); }
	
	ObjectProperties& GetProperties() { return m_Properties; }
	bool& GetIsSleeping()  { return m_IsSleeping; }

	float& GetLinVThreshold() { return m_LinearVelocityThreshold; }
	float& GetAngVThreshold() { return m_AngularVelocityThreshold; }

	void Step(float deltaTime);
	void Move(glm::vec3 direction);
	void Wake();
	//void AddRotation(glm::vec3 rotation); // depricated

	void UpdateInertiaTensor();

	IEntity* operator ->() { return m_Entity.get(); }

protected:
	void UpdateOrientation(float deltaTime);

private:
	unsigned int m_Id;
	EntityTypes m_Type;
	std::unique_ptr<IEntity> m_Entity;
	ObjectProperties m_Properties;

	float m_LinearVelocityThreshold = 0.015f;
	float m_AngularVelocityThreshold = 0.01f;

	bool m_IsSleeping = false;
	float m_SleepTimerThreshold = 0.5f;
	float m_SleepTimer = m_SleepTimerThreshold;
};

