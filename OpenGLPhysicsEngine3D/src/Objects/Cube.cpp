#include "Cube.h"

#include <glm/gtc/matrix_transform.hpp>

#include <Application/Renderer.h>


Cube::Cube()
	: m_Mesh(24, 36)
{
    InitializeMesh();
}
//
//void Cube::Scale(glm::vec3 scale)
//{
//    m_Mesh.Scale(scale);
//}
//
//void Cube::Rotate(glm::vec3 rotation)
//{
//    m_Mesh.Rotate(rotation);
//}
//
//void Cube::Translate(glm::vec3 translation)
//{
//    m_Mesh.Translate(translation);
//}
//
//void Cube::Move(glm::vec3 translation)
//{
//    m_Mesh.Move(translation);
//}

void Cube::ApplyTransform(const Transform& transform)
{
    m_Mesh.SetTransform(transform);
}

AABB Cube::GetAABB()
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

OBB Cube::GetOBB()
{
    OBB obb{};
    glm::mat4 model = m_Mesh.GetModel();

    obb.center = glm::vec3(model[3]);

    obb.axes[0] = glm::normalize(glm::vec3(model[0]));
    obb.axes[1] = glm::normalize(glm::vec3(model[1]));
    obb.axes[2] = glm::normalize(glm::vec3(model[2]));

    obb.halfSize = m_Mesh.GetScale();

    return obb;
}

void Cube::Draw(const Shader& shader)
{
    m_Mesh.Draw(shader);
}

void Cube::InitializeMesh()
{
    std::vector<Vertex> vertices = {
        //   position                normals
     {{-1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}},
     {{ 1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}},
     {{ 1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}},
     {{-1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}},

     {{-1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
     {{-1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
     {{ 1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
     {{ 1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},

     {{-1.0f, -1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}},
     {{-1.0f, -1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}},
     {{-1.0f,  1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}},
     {{-1.0f,  1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}},

     {{ 1.0f, -1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}},
     {{ 1.0f,  1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}},
     {{ 1.0f,  1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}},
     {{ 1.0f, -1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}},

     {{-1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}},
     {{-1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}},
     {{ 1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}},
     {{ 1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}},

     {{-1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}},
     {{ 1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}},
     {{ 1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}},
     {{-1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}}
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        8, 9,10, 10,11, 8,
       12,13,14, 14,15,12,
       16,17,18, 18,19,16,
       20,21,22, 22,23,20
    };

    VertexLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);

    m_Mesh.SetVertices(vertices);
    m_Mesh.SetIndices(indices);

    m_Mesh.Initialize(layout);
}

