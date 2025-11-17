#include "Mesh.h"

#include <glm/gtc/matrix_transform.hpp>


Mesh::Mesh(unsigned int vertexCount, unsigned int indexCount)
    : m_Scale(1), m_Rotation(0), m_Translation(0), m_Model(1)
{
    m_Vertices = std::vector<Vertex>(vertexCount);
    m_TransformedVertices = std::vector<Vertex>(vertexCount);
    m_Indices = std::vector<unsigned int>(indexCount);

    m_ShouldBeTransformed = true;
}

void Mesh::Initialize(const VertexLayout& layout)
{
    m_VAO = std::make_unique<VertexArray>();

    m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex), GL_STATIC_DRAW);

    m_VAO->AddBufferLayout(*m_VBO, layout);

    m_EBO = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());

    m_VAO->UnBind();
    m_VBO->UnBind();
    m_EBO->UnBind();
}

void Mesh::Scale(glm::vec3 scale)
{
    if (m_Scale == scale) 
        return;
    m_Scale = scale;
    m_ShouldBeTransformed = true;
}

void Mesh::Rotate(glm::vec3 rotation)
{
    if (m_Rotation == rotation) 
        return;
    m_Rotation = rotation;
    m_ShouldBeTransformed = true;
}

void Mesh::Translate(glm::vec3 translation)
{
    if (m_Translation == translation)
        return;
    m_Translation = translation;
    m_ShouldBeTransformed = true;
}

void Mesh::SetTransform(const Transform& transform)
{
    Translate(transform.translation);
    Rotate(transform.rotation);
    Scale(transform.scale);
}

void Mesh::Move(glm::vec3 translation)
{
    m_Translation += translation;
    m_ShouldBeTransformed = true;
}

glm::mat4 Mesh::GetModel()
{
    if (m_ShouldBeTransformed) UpdateTransformedVertices();
    return m_Model;
}

void Mesh::Draw(const Shader& shader)
{
    //shader.Bind();
    if (m_ShouldBeTransformed) UpdateTransformedVertices();
    shader.SetUniformMat4f("uModel", m_Model);

    m_VAO->Bind();
    GLCall(glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, NULL));
    m_VAO->UnBind();

    //shader.UnBind();
}

void Mesh::SetVertices(std::vector<Vertex> vertices)
{
    m_Vertices = std::move(vertices);
}

void Mesh::SetIndices(std::vector<unsigned int> indices)
{
    m_Indices = std::move(indices);
}

const std::vector<Vertex>& Mesh::GetTransformedVertices()
{
    if (m_ShouldBeTransformed) UpdateTransformedVertices();
    return m_TransformedVertices;
}

void Mesh::UpdateTransformedVertices()
{
    m_Model = glm::mat4(1.0f);
    m_Model = glm::translate(m_Model, m_Translation);
    m_Model = glm::rotate(m_Model, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
    m_Model = glm::rotate(m_Model, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
    m_Model = glm::rotate(m_Model, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
    m_Model = glm::scale(m_Model, m_Scale);

    for (size_t i = 0; i < m_Vertices.size(); i++) {
        m_TransformedVertices[i].position = glm::vec3(m_Model * glm::vec4(m_Vertices[i].position, 1.0f));

        glm::mat3 normalModel = glm::transpose(glm::inverse(m_Model));
        m_TransformedVertices[i].normals = glm::normalize(normalModel * m_Vertices[i].normals);
    }

    m_ShouldBeTransformed = false;
}