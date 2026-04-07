#pragma once

#include <glm/glm.hpp>

struct EPAResult {
    glm::vec3 normal = glm::vec3(0.0f);
    float depth = 0.0f;
    glm::vec3 contactPoint = glm::vec3(0.0f);
};