#pragma once

#include <glm/glm.hpp>

enum class CollisionType {
    NONE = -1,
    FaceA = 0,
    FaceB = 1,
    EdgeCross = 2
};
