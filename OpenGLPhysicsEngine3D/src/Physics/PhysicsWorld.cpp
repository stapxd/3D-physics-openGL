#include "PhysicsWorld.h"
#include <iostream>

#include "Collisions.h"
#include "Enumerators/EntityTypes.h"

#include "Application/Globals.h"


#include "GJK.h"
#include "EPA.h"

glm::vec3 PhysicsWorld::m_Gravity = glm::vec3(0.0f, -9.81f, 0.0f);
float PhysicsWorld::m_DragCoeff = 0.1f;
bool PhysicsWorld::m_IsVacuum = true;

PhysicsWorld::PhysicsWorld()
{
}

Entity* PhysicsWorld::SelectEntityWithScreenPosition(double xPos, double yPos, int windowWidth, int windowHeight, Camera* camera, glm::vec3& rayDir, glm::vec3* hitPoint)
{
	Entity* selected = nullptr;
	float minDist = FLT_MAX;
	float distance;

	float x = (2.0f * xPos) / windowWidth - 1.0f;
	float y = 1.0f - (2.0f * yPos) / windowHeight;
	float z = 1.0f;

	glm::vec3 rayNds = glm::vec3(x, y, z);

	glm::vec4 rayClip = glm::vec4(rayNds.x, rayNds.y, -1.0f, 1.0f);

	glm::mat4 proj = camera->GetProjection();
	glm::mat4 view = camera->GetView();

	glm::vec4 rayEye = glm::inverse(proj) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

	rayDir = glm::normalize(glm::vec3(glm::inverse(view) * rayEye)); // rayWorld

	for (auto& entity : m_Manager.GetEntities())
	{
		if (Collisions::CheckRayOBBCollision(camera->GetPosition(), rayDir, entity.second->GetOBB(), distance, hitPoint))
		{
			if (distance < minDist) {
				minDist = distance;
				selected = &entity.second;
			}
		}
	}

	return selected;
}

void PhysicsWorld::UpdateInertiaTensors()
{
	for (auto& entity : m_Manager.GetEntities()) {
		entity.second.UpdateInertiaTensor();
	}
}

void PhysicsWorld::Update(float deltaTime, int iterations)
{
	if (!m_Paused) {
		iterations = glm::clamp(iterations, 1, 128);
		float deltaTimePerIteration = deltaTime / iterations;

		for (int i = 0; i < iterations; i++) {
			UpdateInertiaTensors();

			ApplyForceAtPoint();

			MovementEntitiesStep(deltaTimePerIteration);

			BroadPhase();
			NarrowPhase();
		}
	}
	else {
		MovementEntitiesStep(deltaTime);
	}
}

void PhysicsWorld::AddForceAtPoint(Entity& entity, glm::vec3 hitPoint, glm::vec3 forceDir, float forceMagnitude)
{
	if (entity.GetProperties().rigidbody.isStatic)
		return;
	m_ForcesAtPoints.emplace_back(entity, hitPoint, forceDir, forceMagnitude);
}

void PhysicsWorld::ApplyForceAtPoint()
{
	if (m_ForcesAtPoints.empty()) return;

	for (auto& hit : m_ForcesAtPoints) {
		Rigidbody3D& rb = hit.entity.GetProperties().rigidbody;
		const Transform& transform = hit.entity.GetProperties().transform;

		if (rb.isStatic) continue;

		glm::vec3 r = hit.hitPoint - transform.translation;
		glm::vec3 force = hit.forceDir * hit.forceMagnitude;

		glm::vec3 torque = glm::cross(r, force);
		rb.angularVelocity += rb.inverseInertiaTensorWorld * torque;

		glm::vec3 linearAcceleration = force / rb.mass;
		rb.linearVelocity += linearAcceleration;
	}
	m_ForcesAtPoints.clear();
}

void PhysicsWorld::ChangeState(ApplicationStates newState)
{
	if (newState == ApplicationStates::Play)
		m_Paused = false;
	else if (newState == ApplicationStates::Paused) {
		m_Paused = true;
	}
	else if (newState == ApplicationStates::Stop) {
		m_Paused = true;
	}
}

void PhysicsWorld::NarrowPhase()
{
#ifdef GJK_DEBUG
	m_MinkowsiDiff.clear();
	m_MinkowsiDiff.reserve(64);
#endif

	m_CollisionManifold.contactPoints.clear();

	for (size_t i = 0; i < m_CollisionPairs.size(); i++) {
		Entity& bodyA = m_CollisionPairs[i].bodyA;
		Entity& bodyB = m_CollisionPairs[i].bodyB;

		std::vector<glm::vec3> contactPoints;
		glm::vec3 normal;
		float depth;

		bool isStatic_A = bodyA.GetProperties().rigidbody.isStatic;
		bool isStatic_B = bodyB.GetProperties().rigidbody.isStatic;

#ifndef GJK_DEBUG
		bool collided = false;

		EntityTypes typeA = bodyA.GetType();
		EntityTypes typeB = bodyB.GetType();

		if (typeA == EntityTypes::Cube && typeB == EntityTypes::Cube) {
			collided = Collisions::CheckOBBCollision(bodyA, bodyB, normal, depth);
			m_CollisionManifold = Collisions::FindOBBContactPoints(bodyA->GetOBB(), bodyB->GetOBB(), normal, depth);
		}
		else if (typeA == EntityTypes::Sphere && typeB == EntityTypes::Sphere) {
			collided = Collisions::CheckSphereSphereCollision(bodyA, bodyB, normal, depth, m_CollisionManifold.contactPoints);
		}
		else {
			if (typeA == EntityTypes::Sphere) {
				collided = Collisions::CheckSphereOBBCollision(bodyA, bodyB, normal, depth, m_CollisionManifold.contactPoints);
			}
			else {
				collided = Collisions::CheckSphereOBBCollision(bodyB, bodyA, normal, depth, m_CollisionManifold.contactPoints);
				normal = -normal;
			}
		}

		if (collided) {
			//std::printf("x: %f    y: %f    z: %f\n", contactPoint.x, contactPoint.y, contactPoint.z);

	#ifndef ROTATIONAL_PHYSICS_TEST
			SeparateBodies(bodyA, isStatic_A, bodyB, isStatic_B, normal, depth);
			ResolveCollision(bodyA, bodyB, normal, depth);
	#endif

	#ifdef ROTATIONAL_PHYSICS_TEST
			//std::cout << "Collides: "<< m_CollisionManifold.contactPoints.size() << "\n";

			SeparateBodies(bodyA, isStatic_A, bodyB, isStatic_B, normal, depth);
			ResolveCollisionWithRotation3D(bodyA, bodyB, normal, depth, m_CollisionManifold.contactPoints);
			//ResolveCollisionWithRotationAndFriction3D(bodyA, bodyB, normal, depth, m_CollisionManifold.contactPoints);
	#endif

		}
#endif


#ifdef GJK_DEBUG
		if (GJK::Solve(bodyA, bodyB, m_Simplex, &m_MinkowsiDiff)) {
			std::cout << "Collision Detected\n";
			//EPAResult r = EPA::Solve(m_Simplex, bodyA, bodyB);

			//SeparateBodies(bodyA, isStatic_A, bodyB, isStatic_B, r.normal, r.depth);
		}
#endif
	}

}

void PhysicsWorld::BroadPhase()
{
	m_CollisionPairs.clear();

	auto& entities = m_Manager.GetEntities();
	for (auto itA = entities.begin(); itA != entities.end(); itA++) {
		for (auto itB = std::next(itA); itB != entities.end(); itB++) {
			Entity& bodyA = itA->second;
			Entity& bodyB = itB->second;

			if (bodyA.GetProperties().rigidbody.isStatic && bodyB.GetProperties().rigidbody.isStatic)
				continue;

			if (Collisions::CheckAABBCollision(bodyA, bodyB))
				m_CollisionPairs.push_back({ bodyA, bodyB });
		}
	}
}

void PhysicsWorld::MovementEntitiesStep(float deltaTime)
{
	for (auto& entity : m_Manager.GetEntities()) {
		
		/*const ObjectProperties& properties = entity.second.GetProperties();
		entity.second->ApplyTransform(properties.transform);*/

		const ObjectProperties& properties = entity.second.GetProperties();
		entity.second->ApplyTransform(properties.transform);

		if (!m_Paused)
			if (!entity.second.GetProperties().rigidbody.isStatic)
				entity.second.Step(deltaTime);
	}
}

// maybe add correction if it brokes
void PhysicsWorld::SeparateBodies(Entity& bodyA, bool isStatic_A, Entity& bodyB, bool isStatic_B, glm::vec3 normal, float depth)
{
	normal = glm::normalize(normal);

	if (isStatic_B) {
		bodyA.Move(-normal * depth);
	}
	else if (isStatic_A) {
		bodyB.Move(normal * depth);
	}
	else {
		bodyA.Move(-normal * (depth * 0.5f));
		bodyB.Move(normal * (depth * 0.5f));
	}
}

void PhysicsWorld::ResolveCollision(Entity& bodyA, Entity& bodyB, glm::vec3 normal, float depth)
{
	ObjectProperties& propertiesA = bodyA.GetProperties();
	ObjectProperties& propertiesB = bodyB.GetProperties();

	glm::vec3 relativeVelocity = propertiesA.rigidbody.linearVelocity - propertiesB.rigidbody.linearVelocity;

	//float e = std::min(propertiesA.rigidbody.restitution, propertiesB.rigidbody.restitution);
	float e = (propertiesA.rigidbody.restitution + propertiesB.rigidbody.restitution) / 2.0f;

	float numerator = -(1 + e) * glm::dot(relativeVelocity, normal);
	
	float massA = propertiesA.rigidbody.mass;
	float invMassA = (!propertiesA.rigidbody.isStatic && massA > 0.0f)
		? 1.0f / massA
		: 0.0f;

	float massB = propertiesB.rigidbody.mass;
	float invMassB = (!propertiesB.rigidbody.isStatic && massB > 0.0f)
		? 1.0f / massB
		: 0.0f;

	float denominator = invMassA + invMassB;
	float j = numerator / denominator;

	propertiesA.rigidbody.linearVelocity += j * invMassA * normal;
	propertiesB.rigidbody.linearVelocity -= j * invMassB * normal;
}

void PhysicsWorld::ResolveCollisionWithRotation3D(
	Entity& bodyA,
	Entity& bodyB,
	const glm::vec3& normal,
	float depth,
	const std::vector<glm::vec3>& contactPoints)
{

	ObjectProperties& propertiesA = bodyA.GetProperties();
	ObjectProperties& propertiesB = bodyB.GetProperties();

	Rigidbody3D& rbA = propertiesA.rigidbody;
	Rigidbody3D& rbB = propertiesB.rigidbody;

	Transform& transformA = propertiesA.transform;
	Transform& transformB = propertiesB.transform;

	size_t contactCount = contactPoints.size();
	if (contactCount < 1)
		return;

	glm::vec3 collisionDir = transformA.translation - transformB.translation;
	glm::vec3 actualNormal = normal;
	if (glm::dot(actualNormal, collisionDir) < 0) {
		actualNormal = -actualNormal;
	}

	//float e = std::min(rbA.restitution, rbB.restitution);
	float e = (rbA.restitution + rbB.restitution) / 2.0f;

	float massA = rbA.mass;
	float invMassA = (!rbA.isStatic && massA > 0.0f)
		? 1.0f / massA
		: 0.0f;

	float massB = rbB.mass;
	float invMassB = (!rbB.isStatic && massB > 0.0f)
		? 1.0f / massB
		: 0.0f;

	glm::vec3 cp(0.0f);

	for (size_t i = 0; i < contactCount; i++) {
		cp = contactPoints[i];

		glm::vec3 rA = cp - transformA.translation;
		glm::vec3 rB = cp - transformB.translation;

		glm::vec3 angularVA = glm::cross(rbA.angularVelocity, rA);
		glm::vec3 angularVB = glm::cross(rbB.angularVelocity, rB);

		glm::vec3 relativeVelocity = (rbA.linearVelocity + angularVA) - (rbB.linearVelocity + angularVB);

		float contactVelocityMag = glm::dot(relativeVelocity, actualNormal);

		if (contactVelocityMag > 0.0f)
			continue;

		glm::vec3 geometricTorqueA = glm::cross(rA, actualNormal);
		glm::vec3 geometricTorqueB = glm::cross(rB, actualNormal);

		float angTermA = glm::dot(geometricTorqueA, rbA.inverseInertiaTensorWorld * geometricTorqueA);
		float angTermB = glm::dot(geometricTorqueB, rbB.inverseInertiaTensorWorld * geometricTorqueB);

		float denominator = invMassA + invMassB + angTermA + angTermB;

		if (denominator < 0.000001f) continue;

		float j = -(1.0f + e) * contactVelocityMag;
		j /= (denominator * (float)contactCount);

		glm::vec3 impulse = j * actualNormal;

		rbA.linearVelocity += impulse * invMassA;
		rbA.angularVelocity += rbA.inverseInertiaTensorWorld * glm::cross(rA, impulse);

		rbB.linearVelocity -= impulse * invMassB;
		rbB.angularVelocity -= rbB.inverseInertiaTensorWorld * glm::cross(rB, impulse);
	}
}

void PhysicsWorld::ResolveCollisionWithRotationAndFriction3D(
	Entity& bodyA, 
	Entity& bodyB, 
	const glm::vec3& normal, 
	float depth, 
	const std::vector<glm::vec3>& contactPoints)
{
	ObjectProperties& propertiesA = bodyA.GetProperties();
	ObjectProperties& propertiesB = bodyB.GetProperties();

	Rigidbody3D& rbA = propertiesA.rigidbody;
	Rigidbody3D& rbB = propertiesB.rigidbody;

	Transform& transformA = propertiesA.transform;
	Transform& transformB = propertiesB.transform;

	size_t contactCount = contactPoints.size();
	if (contactCount < 1)
		return;

	glm::vec3 collisionDir = transformA.translation - transformB.translation;
	glm::vec3 actualNormal = normal;
	if (glm::dot(actualNormal, collisionDir) < 0) {
		actualNormal = -actualNormal;
	}

	//float e = std::min(rbA.restitution, rbB.restitution);
	float e = (rbA.restitution + rbB.restitution) * 0.5f;

	float sf = (rbA.staticFriction + rbB.staticFriction) * 0.5f;
	float df = (rbA.dynamicFriction + rbB.dynamicFriction) * 0.5f;

	float massA = rbA.mass;
	float invMassA = (!rbA.isStatic && massA > 0.0f)
		? 1.0f / massA
		: 0.0f;

	float massB = rbB.mass;
	float invMassB = (!rbB.isStatic && massB > 0.0f)
		? 1.0f / massB
		: 0.0f;

	glm::vec3 cp(0.0f);

	for (size_t i = 0; i < contactCount; i++) {
		cp = contactPoints[i];

		glm::vec3 rA = cp - transformA.translation;
		glm::vec3 rB = cp - transformB.translation;

		glm::vec3 angularVA = glm::cross(rbA.angularVelocity, rA);
		glm::vec3 angularVB = glm::cross(rbB.angularVelocity, rB);

		glm::vec3 relativeVelocity = (rbA.linearVelocity + angularVA) - (rbB.linearVelocity + angularVB);

		float contactVelocityMag = glm::dot(relativeVelocity, actualNormal);

		if (contactVelocityMag > 0.0f)
			continue;

		glm::vec3 geometricTorqueA = glm::cross(rA, actualNormal);
		glm::vec3 geometricTorqueB = glm::cross(rB, actualNormal);

		float angTermA = glm::dot(geometricTorqueA, rbA.inverseInertiaTensorWorld * geometricTorqueA);
		float angTermB = glm::dot(geometricTorqueB, rbB.inverseInertiaTensorWorld * geometricTorqueB);

		float denominator = invMassA + invMassB + angTermA + angTermB;

		if (denominator < 0.000001f) continue;

		float j = -(1.0f + e) * contactVelocityMag;
		j /= (denominator * (float)contactCount);

		glm::vec3 impulse = j * actualNormal;


		if (!rbA.isStatic) {
			rbA.linearVelocity += impulse * invMassA;
			rbA.angularVelocity += rbA.inverseInertiaTensorWorld * glm::cross(rA, impulse);
		}


		if (!rbB.isStatic) {
			rbB.linearVelocity -= impulse * invMassB;
			rbB.angularVelocity -= rbB.inverseInertiaTensorWorld * glm::cross(rB, impulse);
		}

		// friction
		angularVA = glm::cross(rbA.angularVelocity, rA);
		angularVB = glm::cross(rbB.angularVelocity, rB);

		relativeVelocity = (rbA.linearVelocity + angularVA) - (rbB.linearVelocity + angularVB);

		glm::vec3 tangent = relativeVelocity - (glm::dot(relativeVelocity, actualNormal) * actualNormal);

		if (glm::length(tangent) > 0.0001f) {
			tangent = glm::normalize(tangent);

			glm::vec3 crossAT = glm::cross(rA, tangent);
			glm::vec3 crossBT = glm::cross(rB, tangent);

			float angTermA = glm::dot(crossAT, rbA.inverseInertiaTensorWorld * crossAT);
			float angTermB = glm::dot(crossBT, rbB.inverseInertiaTensorWorld * crossBT);

			float denominator = invMassA + invMassB + angTermA + angTermB;

			if (denominator > 0.000001f) {
				float jt = -glm::dot(relativeVelocity, tangent) / (denominator * (float)contactCount);

				float frictionMag;
				float jAbs = std::abs(j);
				if (std::abs(jt) <= jAbs * sf) {
					frictionMag = jt;
				}
				else {
					frictionMag = (jt > 0.0f ? 1.0f : -1.0f) * jAbs * df;
				}

				glm::vec3 frictionImpulse = frictionMag * tangent;

				if (!rbA.isStatic) {
					rbA.linearVelocity += frictionImpulse * invMassA;
					rbA.angularVelocity += rbA.inverseInertiaTensorWorld * glm::cross(rA, frictionImpulse);
				}
				if (!rbB.isStatic) {
					rbB.linearVelocity -= frictionImpulse * invMassB;
					rbB.angularVelocity -= rbB.inverseInertiaTensorWorld * glm::cross(rB, frictionImpulse);
				}
			}
		}
	}
}
