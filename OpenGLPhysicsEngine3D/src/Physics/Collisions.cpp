#include "Collisions.h"
#include "Structures/OBB.h"

#include <array>

bool Collisions::CheckAABBCollision(Entity& bodyA, Entity& bodyB)
{
    AABB a = bodyA->GetAABB();
    AABB b = bodyB->GetAABB();
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

std::vector<glm::vec3> Collisions::GetOBBVertices(const OBB& obb) {
    std::vector<glm::vec3> vertices(8);

    for (int i = 0; i < 8; i++) {
        glm::vec3 sign(
            (i & 1) ? 1.0f : -1.0f,
            (i & 2) ? 1.0f : -1.0f,
            (i & 4) ? 1.0f : -1.0f
        );
        vertices[i] = obb.center +
            (obb.axes[0] * obb.halfSize.x * sign.x) +
            (obb.axes[1] * obb.halfSize.y * sign.y) +
            (obb.axes[2] * obb.halfSize.z * sign.z);
    }
    return vertices;
}

Face Collisions::GetContactFace(const OBB& obb, const glm::vec3& hitNormal) {
    Face face;

    float maxDot = -1.0f;
    int axisIndex = 0;

    for (int i = 0; i < 3; i++) {
        float d = glm::abs(glm::dot(hitNormal, obb.axes[i]));
        if (d > maxDot) {
            maxDot = d;
            axisIndex = i;
        }
    }

    glm::vec3 normal = obb.axes[axisIndex];
    if (glm::dot(hitNormal, normal) > 0.0f)
        normal = -normal;

    int i1 = (axisIndex + 1) % 3;
    int i2 = (axisIndex + 2) % 3;

    face.vertices[0] = obb.center + normal * obb.halfSize[axisIndex] + obb.axes[i1] * obb.halfSize[i1] + obb.axes[i2] * obb.halfSize[i2];
    face.vertices[1] = obb.center + normal * obb.halfSize[axisIndex] - obb.axes[i1] * obb.halfSize[i1] + obb.axes[i2] * obb.halfSize[i2];
    face.vertices[2] = obb.center + normal * obb.halfSize[axisIndex] - obb.axes[i1] * obb.halfSize[i1] - obb.axes[i2] * obb.halfSize[i2];
    face.vertices[3] = obb.center + normal * obb.halfSize[axisIndex] + obb.axes[i1] * obb.halfSize[i1] - obb.axes[i2] * obb.halfSize[i2];
    face.normal = normal;

    return face;
}

void Collisions::Clip(std::vector<glm::vec3>& points, const glm::vec3& planeNormal, float planeDistance) {
    std::vector<glm::vec3> clipped;
    if (points.empty()) return;

    glm::vec3 p1 = points.back();
    float d1 = glm::dot(planeNormal, p1) - planeDistance;

    for (const auto& p2 : points) {
        float d2 = glm::dot(planeNormal, p2) - planeDistance;

        if (d1 >= 0.0f && d2 >= 0.0f) {
            clipped.push_back(p2);
        }
        else if (d1 >= 0.0f && d2 < 0.0f) {
            float t = d1 / (d1 - d2);
            clipped.push_back(p1 + t * (p2 - p1));
        }
        else if (d1 < 0.0f && d2 >= 0.0f) {
            float t = d1 / (d1 - d2);
            clipped.push_back(p1 + t * (p2 - p1));
            clipped.push_back(p2);
        }

        p1 = p2;
        d1 = d2;
    }
    points = clipped;
}

CollisionManifold Collisions::FindOBBContactPoints(Entity& bodyA, Entity& bodyB, glm::vec3 normal, float depth)
{
    CollisionManifold collisionManifold;

    collisionManifold.depth = depth;
    collisionManifold.normal = normal;

    Face faceA = GetContactFace(bodyA->GetOBB(), -normal);
    Face faceB = GetContactFace(bodyB->GetOBB(), normal);

    for (int i = 0; i < 4; i++) {
        collisionManifold.pointsOfFaces.push_back(faceA.vertices[i]);
        collisionManifold.pointsOfFaces.push_back(faceB.vertices[i]);
    }

    std::shared_ptr<Face> reference;
    std::shared_ptr<Face> incident;

    bool flipped = false;

    float dotA = glm::abs(glm::dot(faceA.normal, normal));
    float dotB = glm::abs(glm::dot(faceB.normal, normal));

    OBB referenceOBB = bodyA->GetOBB();

    const float bias = 1.05f;
    if (dotA * bias >= dotB) {
        reference = std::make_shared<Face>(faceA);
        incident = std::make_shared<Face>(faceB);
        referenceOBB = bodyA->GetOBB();
    }
    else {
        reference = std::make_shared<Face>(faceB);
        incident = std::make_shared<Face>(faceA);
        referenceOBB = bodyB->GetOBB();
        flipped = true;
    }

    collisionManifold.reference = reference;
    collisionManifold.incident = incident;

    int refAxisIdx = 0;
    float maxDot = 0.0f;
    for (int i = 0; i < 3; i++) {
        float dot = glm::abs(glm::dot(reference->normal, referenceOBB.axes[i]));
        if (dot > maxDot) {
            maxDot = dot;
            refAxisIdx = i;
        }
    }

    int i1 = (refAxisIdx + 1) % 3;
    int i2 = (refAxisIdx + 2) % 3;

    std::vector<glm::vec3> contactPoints = {
        incident->vertices[0], incident->vertices[1],
        incident->vertices[2], incident->vertices[3]
    };

   /* std::vector<glm::vec3> contactPoints = {
        incident->vertices[0]
    };*/

    float d1 = glm::dot(referenceOBB.axes[i1], referenceOBB.center);
    float d2 = glm::dot(referenceOBB.axes[i2], referenceOBB.center);

    Clip(contactPoints, -referenceOBB.axes[i1], -(d1 + referenceOBB.halfSize[i1]));
    Clip(contactPoints, referenceOBB.axes[i1], d1 - referenceOBB.halfSize[i1]);

    Clip(contactPoints, -referenceOBB.axes[i2], -(d2 + referenceOBB.halfSize[i2]));
    Clip(contactPoints, referenceOBB.axes[i2], d2 - referenceOBB.halfSize[i2]);

    std::vector<glm::vec3> finalPoints; // ---
    float refPlaneDist = glm::dot(reference->normal, reference->vertices[0]);

    for (const auto& p : contactPoints) {
        float separation = glm::dot(reference->normal, p) - refPlaneDist;

        if (separation <= 0.01f) {
            collisionManifold.contactPoints.push_back(p);
        }
    }

    return collisionManifold;
}

bool Collisions::CheckOBBCollision(Entity& bodyA, Entity& bodyB, glm::vec3& normal, float& depth)
{
    const float eps = 1e-4f;
    depth = FLT_MAX;

    OBB obbA = bodyA->GetOBB();
    OBB obbB = bodyB->GetOBB();

    // SAT collision
    float rotationMatrix[3][3];
    float absRotationMatrix[3][3];

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            rotationMatrix[i][j] = glm::dot(obbA.axes[i], obbB.axes[j]);

    glm::vec3 tAWorld = obbB.center - obbA.center;
    glm::vec3 tA = glm::vec3(glm::dot(tAWorld, obbA.axes[0]), glm::dot(tAWorld, obbA.axes[1]), glm::dot(tAWorld, obbA.axes[2]));

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            absRotationMatrix[i][j] = std::abs(rotationMatrix[i][j]) + eps;

    float rA, rB, overlap;
    glm::vec3 candidateNormal;

    for (int i = 0; i < 3; i++)
    {
        rA = obbA.halfSize[i];
        rB = obbB.halfSize.x * absRotationMatrix[i][0] +
            obbB.halfSize.y * absRotationMatrix[i][1] +
            obbB.halfSize.z * absRotationMatrix[i][2];

        overlap = rA + rB - std::abs(tA[i]);

        if (overlap < 0.0f)
            return false;

        if (overlap < depth)
        {
            depth = overlap;
            candidateNormal = obbA.axes[i];
        }
    }

    for (int i = 0; i < 3; i++)
    {
        rA = obbA.halfSize.x * absRotationMatrix[0][i] +
            obbA.halfSize.y * absRotationMatrix[1][i] +
            obbA.halfSize.z * absRotationMatrix[2][i];

        rB = obbB.halfSize[i];

        float proj = std::abs(tA.x * rotationMatrix[0][i] +
            tA.y * rotationMatrix[1][i] +
            tA.z * rotationMatrix[2][i]);

        overlap = rA + rB - proj;

        if (overlap < 0.0f)
            return false;

        if (overlap < depth)
        {
            depth = overlap;
            candidateNormal = obbB.axes[i];
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            glm::vec3 axis = glm::cross(obbA.axes[i], obbB.axes[j]);

            float len = glm::length(axis);

            if (len < 1e-6f) continue;

            axis /= len;

            rA = std::abs(glm::dot(obbA.axes[0] * obbA.halfSize.x, axis)) +
                std::abs(glm::dot(obbA.axes[1] * obbA.halfSize.y, axis)) +
                std::abs(glm::dot(obbA.axes[2] * obbA.halfSize.z, axis));

            rB = std::abs(glm::dot(obbB.axes[0] * obbB.halfSize.x, axis)) +
                std::abs(glm::dot(obbB.axes[1] * obbB.halfSize.y, axis)) +
                std::abs(glm::dot(obbB.axes[2] * obbB.halfSize.z, axis));

            float proj = std::abs(glm::dot(tAWorld, axis));

            float overlap = rA + rB - proj;

            if (overlap < 0.0f)
                return false;

            if (overlap < depth) {
                depth = overlap;
                candidateNormal = axis;
            }
        }
    }

    normal = glm::normalize(candidateNormal);

    if (glm::dot(tA, normal) < 0)
        normal = -normal;
    
    return true;
}

bool Collisions::CheckRayOBBCollision(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const OBB& obb, float& distance, glm::vec3* hitPoint)
{
    distance = FLT_MAX;

    const float eps = 1e-6f;

    glm::vec3 delta = obb.center - rayOrigin;

    float tMin = -FLT_MAX;
    float tMax = FLT_MAX;

    for (int i = 0; i < 3; i++) {
        float e = glm::dot(obb.axes[i], delta);
        float f = glm::dot(obb.axes[i], rayDir);

        if (std::abs(f) > eps) {
            float t1 = (e + obb.halfSize[i]) / f;
            float t2 = (e - obb.halfSize[i]) / f;

            if (t1 > t2) {
                float w = t1;
                t1 = t2;
                t2 = w;
            }

            if (t2 < tMax) tMax = t2;
            if (t1 > tMin) tMin = t1;

            if (tMin > tMax)
                return false;
        }
        else {
            if (-e - obb.halfSize[i] > 0.0f || -e + obb.halfSize[i] < 0.0f)
                return false;
        }
    }

    if (tMin > 0.0f)
        distance = tMin;
    else
        distance = tMax;

    if (distance < 0.0f)
        return false;

    if (hitPoint != 0) {
        *hitPoint = rayOrigin + rayDir * tMin;
    }

    return true;

}
