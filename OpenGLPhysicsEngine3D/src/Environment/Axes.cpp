#include "Axes.h"

#include <glm/gtc/matrix_transform.hpp>

Axes::Axes()
    : m_Scale(1), m_Model(1)
{
    m_Vertices = {
           // position         // color
         -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
          1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

         0.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,

         0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
         0.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f 
    };

    m_VAO = std::make_unique<VertexArray>();

    m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(float), GL_STATIC_DRAW);

    VertexLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    m_VAO->AddBufferLayout(*m_VBO, layout);

    m_VAO->UnBind();
    m_VBO->UnBind();
}

void Axes::Scale(glm::vec3 scale)
{
    m_Scale = scale;
    m_ModelShouldBeUpdated = true;
}

void Axes::Draw(const Shader& shader)
{
    shader.Bind();

    if (m_ModelShouldBeUpdated)
        UpdateModel();

    shader.SetUniformMat4f("uModel", m_Model);

    m_VAO->Bind();
    GLCall(glDrawArrays(GL_LINES, 0, 6));
    m_VAO->UnBind();

    shader.UnBind();
}

void Axes::UpdateModel()
{
    m_Model = glm::mat4(1.0f);
    
    m_Model = glm::scale(m_Model, m_Scale);

    m_ModelShouldBeUpdated = false;
}
