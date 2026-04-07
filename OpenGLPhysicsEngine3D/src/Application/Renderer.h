#pragma once

#include "OpenGL/Camera.h"
#include "OpenGL/ShadowMap.h"
#include "OpenGL/Shader.h"

#include "Objects/Parts/Mesh.h"
#include "Structures/LightData.h"

class Renderer {
public:
    static void BeginScene(const Camera& camera, const Shader& shader);
    static void BeginSceneLight(const Camera& camera, const Shader& shader, const LightData& lightData);
    static void SetShadowLigntUniforms(const ShadowMap& shadowMap, const Camera& camera, const Shader& shader);

    static void DrawMesh(const Shader& shader, Mesh& mesh);
    static void DrawMesh(const Shader& shader, Mesh& mesh, glm::vec3 color);

    static void EndScene(const Shader& shader);
};