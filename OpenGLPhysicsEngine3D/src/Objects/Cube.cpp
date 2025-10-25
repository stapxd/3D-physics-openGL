#include "Cube.h"

#include <glm/gtc/matrix_transform.hpp>

#include <Application/Renderer.h>

Cube::Cube()
	: m_Scale(1), m_Rotation(0), m_Translation(0), m_Model(1)
{
    m_Vertices = {
         // position             // normals
        -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
         1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,   0.0f, 0.0f, -1.0f,
         1.0f,  1.0f, -1.0f,   0.0f, 0.0f, -1.0f,
         1.0f, -1.0f, -1.0f,   0.0f, 0.0f, -1.0f,

         -1.0f, -1.0f, -1.0f,  -1.0f, 0.0f, 0.0f,
         -1.0f, -1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,
         -1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,
         -1.0f,  1.0f, -1.0f,  -1.0f, 0.0f, 0.0f,

          1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,
          1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,
          1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f,
          1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 0.0f,

          -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
          -1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,
           1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,
           1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,

           -1.0f, -1.0f, -1.0f,   0.0f, -1.0f, 0.0f,
            1.0f, -1.0f, -1.0f,   0.0f, -1.0f, 0.0f,
            1.0f, -1.0f,  1.0f,   0.0f, -1.0f, 0.0f,
           -1.0f, -1.0f,  1.0f,   0.0f, -1.0f, 0.0f
    };
    
    m_Indices = {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        8, 9,10, 10,11, 8,
       12,13,14, 14,15,12,
       16,17,18, 18,19,16,
       20,21,22, 22,23,20 
    };


    m_VAO = std::make_unique<VertexArray>();

    m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(float), GL_STATIC_DRAW);

    VertexLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    m_VAO->AddBufferLayout(*m_VBO, layout);

    m_EBO = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());

    m_VAO->UnBind();
    m_VBO->UnBind();
    m_EBO->UnBind();
}

void Cube::Scale(glm::vec3 scale)
{
    m_Scale = scale;
    m_ModelShouldBeUpdated = true;
}

void Cube::Rotate(glm::vec3 rotation)
{
    m_Rotation = rotation;
    m_ModelShouldBeUpdated = true;
}

void Cube::Translate(glm::vec3 translation)
{
    m_Translation = translation;
    m_ModelShouldBeUpdated = true;
}

void Cube::Draw(const Shader& shader)
{
    shader.Bind();
    if(m_ModelShouldBeUpdated) UpdateModel();
    shader.SetUniformMat4f("uModel", m_Model);

    m_VAO->Bind();
    GLCall(glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, NULL));
    m_VAO->UnBind();

    shader.UnBind();
}

void Cube::UpdateModel()
{
    m_Model = glm::mat4(1.0f);
    m_Model = glm::translate(m_Model, m_Translation);
    m_Model = glm::rotate(m_Model, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
    m_Model = glm::rotate(m_Model, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
    m_Model = glm::rotate(m_Model, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
    m_Model = glm::scale(m_Model, m_Scale);

    m_ModelShouldBeUpdated = false;
}
