#include "Collisions.h"
#include "Structures/OBB.h"

#include "Objects/Sphere.h"
#include "Objects/Cube.h"

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

CollisionManifold Collisions::FindOBBContactPoints(const OBB& bodyAOBB, const OBB& bodyBOBB, glm::vec3 normal, float depth)
{
    CollisionManifold collisionManifold;

    collisionManifold.depth = depth;
    collisionManifold.normal = normal;

    Face faceA = GetContactFace(bodyAOBB, -normal);
    Face faceB = GetContactFace(bodyBOBB, normal);

    for (int i = 0; i < 4; i++) {
        collisionManifold.pointsOfFaces.push_back(faceA.vertices[i]);
        collisionManifold.pointsOfFaces.push_back(faceB.vertices[i]);
    }

    std::shared_ptr<Face> reference;
    std::shared_ptr<Face> incident;

    bool flipped = false;

    float dotA = glm::abs(glm::dot(faceA.normal, normal));
    float dotB = glm::abs(glm::dot(faceB.normal, normal));

    OBB referenceOBB = bodyAOBB;

    const float bias = 1.05f;
    if (dotA * bias >= dotB) {
        reference = std::make_shared<Face>(faceA);
        incident = std::make_shared<Face>(faceB);
        referenceOBB = bodyAOBB;
    }
    else {
        reference = std::make_shared<Face>(faceB);
        incident = std::make_shared<Face>(faceA);
        referenceOBB = bodyBOBB;
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

    if (glm::dot(tAWorld, normal) < 0.0f) {
        normal = -normal;
    }
    
    return true;
}

bool Collisions::CheckSphereOBBCollision(Entity& bodyA, Entity& bodyB, glm::vec3& normal, float& depth, std::vector<glm::vec3>& contactPoints)
{
    Sphere* sphereEntity = (Sphere*)(bodyA.GetEntity());
    Cube* cubeEntity = (Cube*)(bodyB.GetEntity());

    OBB obb = cubeEntity->GetOBB();
    glm::vec3 sphereCenter = sphereEntity->GetMesh()->GetModel()[3];
    float radius = sphereEntity->GetRadius();

    glm::vec3 relativeCenter = sphereCenter - obb.center;

    glm::vec3 localCenter;
    localCenter.x = glm::dot(relativeCenter, obb.axes[0]);
    localCenter.y = glm::dot(relativeCenter, obb.axes[1]);
    localCenter.z = glm::dot(relativeCenter, obb.axes[2]);

    glm::vec3 localClosestPoint;
    localClosestPoint.x = glm::clamp(localCenter.x, -obb.halfSize.x, obb.halfSize.x);
    localClosestPoint.y = glm::clamp(localCenter.y, -obb.halfSize.y, obb.halfSize.y);
    localClosestPoint.z = glm::clamp(localCenter.z, -obb.halfSize.z, obb.halfSize.z);

    glm::vec3 worldClosestPoint = obb.center; // <--- contact point
    for (int i = 0; i < 3; i++) {
        worldClosestPoint += obb.axes[i] * localClosestPoint[i];
    }

    glm::vec3 collisionVec = worldClosestPoint - sphereCenter;
    float distanceSq = glm::dot(collisionVec, collisionVec);

    if (distanceSq >= radius * radius) {
        return false;
    }

    float distance = std::sqrt(distanceSq);

    if (distance > 0.0001f) {
        normal = collisionVec / distance;
        depth = radius - distance;
    }
    else {
        float faceDistX = obb.halfSize.x - std::abs(localCenter.x);
        float faceDistY = obb.halfSize.y - std::abs(localCenter.y);
        float faceDistZ = obb.halfSize.z - std::abs(localCenter.z);

        if (faceDistX < faceDistY && faceDistX < faceDistZ) {
            normal = (localCenter.x > 0) ? obb.axes[0] : -obb.axes[0];
            depth = faceDistX + radius;
        }
        else if (faceDistY < faceDistZ) {
            normal = (localCenter.y > 0) ? obb.axes[1] : -obb.axes[1];
            depth = faceDistY + radius;
        }
        else {
            normal = (localCenter.z > 0) ? obb.axes[2] : -obb.axes[2];
            depth = faceDistZ + radius;
        }
    }

    contactPoints.push_back(worldClosestPoint);

    return true;
}

bool Collisions::CheckSphereSphereCollision(Entity& bodyA, Entity& bodyB, glm::vec3& normal, float& depth, std::vector<glm::vec3>& contactPoints)
{
    glm::vec3 posA = glm::vec3(bodyA.GetEntity()->GetMesh()->GetModel()[3]);
    glm::vec3 posB = glm::vec3(bodyB.GetEntity()->GetMesh()->GetModel()[3]);

    glm::vec3 relativePos = posB - posA;
    float distanceSq = glm::dot(relativePos, relativePos);

    float radiusA = ((Sphere*)bodyA.GetEntity())->GetRadius();
    float radiusB = ((Sphere*)bodyB.GetEntity())->GetRadius();

    float radiusSum = radiusA + radiusB;

    if (distanceSq >= radiusSum * radiusSum) {
        return false;
    }

    float distance = glm::sqrt(distanceSq);

    if (distance != 0.0f) {
        depth = radiusSum - distance;
        normal = relativePos / distance;
    }
    else {
        depth = radiusSum;
        normal = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    contactPoints.push_back(posA + (normal * radiusA));

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
