#include "Collisions.h"
#include "Structures/OBB.h"

bool Collisions::CheckAABBCollision(IEntity* bodyA, IEntity* bodyB)
{
    AABB a = bodyA->GetAABB();
    AABB b = bodyB->GetAABB();
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool Collisions::CheckOBBCollision(IEntity* bodyA, IEntity* bodyB, glm::vec3& normal, float& depth)
{
    const float EPSILON = 1e-4f;
    depth = FLT_MAX;
    normal = glm::vec3(0.0f);

    OBB obbA = bodyA->GetOBB();
    OBB obbB = bodyB->GetOBB();

    float rotationMatrix[3][3];
    float absRotationMatrix[3][3];

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            rotationMatrix[i][j] = glm::dot(obbA.axes[i], obbB.axes[j]);

    glm::vec3 tA = obbB.center - obbA.center;
    tA = glm::vec3(glm::dot(tA, obbA.axes[0]), glm::dot(tA, obbA.axes[1]), glm::dot(tA, obbA.axes[2]));

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            absRotationMatrix[i][j] = std::abs(rotationMatrix[i][j]) + EPSILON;

    float radiusA, radiusB, overlap;
    glm::vec3 candidateNormal;

    for (int i = 0; i < 3; i++)
    {
        radiusA = obbA.halfSize[i];
        radiusB = obbB.halfSize.x * absRotationMatrix[i][0] +
            obbB.halfSize.y * absRotationMatrix[i][1] +
            obbB.halfSize.z * absRotationMatrix[i][2];

        overlap = radiusA + radiusB - std::abs(tA[i]);
        if (overlap < 0.0f)
            return false;

        if (overlap < depth)
        {
            depth = overlap;
            candidateNormal = obbA.axes[i] * ((tA[i] < 0.0f) ? -1.0f : 1.0f);
        }
    }

    for (int i = 0; i < 3; i++)
    {
        radiusA = obbA.halfSize.x * absRotationMatrix[0][i] +
            obbA.halfSize.y * absRotationMatrix[1][i] +
            obbA.halfSize.z * absRotationMatrix[2][i];
        radiusB = obbB.halfSize[i];

        float proj = std::abs(tA.x * rotationMatrix[0][i] +
            tA.y * rotationMatrix[1][i] +
            tA.z * rotationMatrix[2][i]);
        overlap = radiusA + radiusB - proj;
        if (overlap < 0.0f)
            return false;

        if (overlap < depth)
        {
            depth = overlap;
            candidateNormal = obbB.axes[i] * ((proj < 0.0f) ? -1.0f : 1.0f);
        }
    }
    
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            radiusA = obbA.halfSize[(i + 1) % 3] * absRotationMatrix[(i + 2) % 3][j] +
                obbA.halfSize[(i + 2) % 3] * absRotationMatrix[(i + 1) % 3][j];
            radiusB = obbB.halfSize[(j + 1) % 3] * absRotationMatrix[i][(j + 2) % 3] +
                obbB.halfSize[(j + 2) % 3] * absRotationMatrix[i][(j + 1) % 3];

            float proj = std::abs(tA[(i + 2) % 3] * rotationMatrix[(i + 1) % 3][j] -
                tA[(i + 1) % 3] * rotationMatrix[(i + 2) % 3][j]);

            overlap = radiusA + radiusB - proj;
            if (overlap < 0.0f)
                return false;

            if (overlap < depth)
            {
                depth = overlap;
                candidateNormal = glm::normalize(glm::cross(obbA.axes[i], obbB.axes[j]));
            }
        }
    }

    normal = candidateNormal;
    return true;
}
