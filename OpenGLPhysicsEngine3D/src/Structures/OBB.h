#pragma once
#include <glm/glm.hpp>

struct OBB
{
    glm::vec3 center;    
    glm::vec3 axes[3];   
    glm::vec3 halfSize;  
};