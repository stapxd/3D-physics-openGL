#include "Mesh.h"

#include <glm/gtc/matrix_transform.hpp>


Mesh::Mesh(unsigned int vertexCount, unsigned int indexCount)
    : m_Scale(1), m_Rotation(0), m_Translation(0), m_Model(1)
{
    m_Vertices = std::vector<Vertex>(vertexCount);
    m_TransformedVertices = std::vector<glm::vec3>(vertexCount);
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
    m_Scale = scale;
    m_ShouldBeTransformed = true;
}

void Mesh::Rotate(glm::vec3 rotation)
{
    m_Rotation = rotation;
    m_ShouldBeTransformed = true;
}

void Mesh::Translate(glm::vec3 translation)
{
    m_Translation = translation;
    m_ShouldBeTransformed = true;
}

void Mesh::Draw(const Shader& shader)
{
    shader.Bind();
    if (m_ShouldBeTransformed) UpdateTransformedVertices();
    shader.SetUniformMat4f("uModel", m_Model);

    m_VAO->Bind();
    GLCall(glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, NULL));
    m_VAO->UnBind();

    shader.UnBind();
}

void Mesh::SetVertices(std::vector<Vertex> vertices)
{
    m_Vertices = std::move(vertices);
}

void Mesh::SetIndices(std::vector<unsigned int> indices)
{
    m_Indices = std::move(indices);
}

const std::vector<glm::vec3>& Mesh::GetTransformedVertices()
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

    for(int i = 0; i < m_Vertices.size(); i++)
        m_TransformedVertices[i] = glm::vec3(m_Model * glm::vec4(m_Vertices[i].position, 1.0f));

    m_ShouldBeTransformed = false;
}