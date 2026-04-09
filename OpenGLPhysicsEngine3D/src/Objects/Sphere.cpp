#include "Sphere.h"

#include <glm/gtc/constants.hpp>

Sphere::Sphere(float radius, int stackCount, int sectorCount)
{
	m_Radius = radius;
	m_StackCount = stackCount;
	m_SectorCount = sectorCount;

	InitializeMesh();
}

void Sphere::ApplyTransform(const Transform& transform)
{
    m_Mesh.SetTransform(transform);
}

void Sphere::SetRadius(float radius)
{
	if (m_Radius == radius)
		return;

	if (radius <= 0.0f) {
		m_Radius = 0.5f;
		return;
	}

	m_Radius = radius;

}

void Sphere::Draw(const Shader& shader)
{
	m_Mesh.Draw(shader);
}

AABB Sphere::GetAABB()
{
	AABB box;

	box.min = glm::vec3(FLT_MAX);
	box.max = glm::vec3(-FLT_MAX);

	for (const auto& vertex : m_Mesh.GetTransformedVertices()) {
		box.min = glm::min(box.min, vertex.position);
		box.max = glm::max(box.max, vertex.position);
	}

	return box;
}

OBB Sphere::GetOBB()
{
	OBB obb;
	glm::mat4 model = m_Mesh.GetModel();

	obb.center = glm::vec3(model[3]);

	obb.axes[0] = glm::normalize(glm::vec3(model[0]));
	obb.axes[1] = glm::normalize(glm::vec3(model[1]));
	obb.axes[2] = glm::normalize(glm::vec3(model[2]));

	obb.halfSize = m_Mesh.GetScale();

	return obb;
}

void Sphere::EstimateInertiaTensor(Rigidbody3D& rigidbody)
{
	float r = m_Radius;
	float m = rigidbody.mass;

	float i = (2.0f / 5.0f) * m * (r * r);

	rigidbody.inertiaTensor = glm::mat3(
		i, 0.0f, 0.0f,
		0.0f, i, 0.0f,
		0.0f, 0.0f, i
	);

	rigidbody.inverseInertiaTensorLocal = glm::inverse(rigidbody.inertiaTensor);
}

void Sphere::BuildSphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
	vertices.clear();
	indices.clear();

	float radius = 1.0f;

	float xy;
	float lengthInv = 1.0f / radius;

	glm::vec3 vertex = glm::vec3(0.0f);
	glm::vec3 normal = glm::vec3(0.0f);
	//glm::vec2 texCoord = glm::vec2(0.0f);

	constexpr float pi = glm::pi<float>();

	float sectorStep = 2 * pi / m_SectorCount;
	float stackStep = pi / m_StackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= m_StackCount; i++)
	{
		stackAngle = pi / 2 - i * stackStep;
		xy = radius * cosf(stackAngle);
		vertex.y = radius * sinf(stackAngle);

		for (int j = 0; j <= m_SectorCount; j++)
		{
			sectorAngle = j * sectorStep;

			vertex.x = xy * cosf(sectorAngle);
			vertex.z = xy * sinf(sectorAngle);

			normal.x = vertex.x * lengthInv;
			normal.y = vertex.y * lengthInv;
			normal.z = vertex.z * lengthInv;

			/*texCoord.x = (float)j / m_SectorCount;
			texCoord.y = (float)i / m_StackCount;*/

			//m_Vertices.emplace_back(vertex, normal, texCoord);
			vertices.emplace_back(vertex, normal);
		}
	}

	int k1, k2;
	for (int i = 0; i < m_StackCount; i++)
	{
		k1 = i * (m_SectorCount + 1);
		k2 = k1 + m_SectorCount + 1;

		for (int j = 0; j < m_SectorCount; j++, k1++, k2++)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (m_StackCount - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}
}

void Sphere::InitializeMesh()
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	BuildSphere(vertices, indices);

	VertexLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(3);

	m_Mesh.Initialize(vertices, indices, layout);
}