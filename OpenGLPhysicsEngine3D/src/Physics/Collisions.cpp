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

Face Collisions::GetIncidentFace(const OBB& obb, const glm::vec3& hitNormal) {
    Face face;

    float minDot = 1.0f;
    int axisIndex = 0;
    for (int i = 0; i < 3; i++) {
        float d = glm::dot(hitNormal, obb.axes[i]);
        if (d < minDot) {
            minDot = d;
            axisIndex = i;
        }
    }

    glm::vec3 normal = obb.axes[axisIndex];
    if (glm::dot(hitNormal, normal) > 0.0f) normal = -normal;

    int i1 = (axisIndex + 1) % 3;
    int i2 = (axisIndex + 2) % 3;

    face.vertices[0] = obb.center + normal * obb.halfSize[axisIndex] + obb.axes[i1] * obb.halfSize[i1] + obb.axes[i2] * obb.halfSize[i2];
    face.vertices[1] = obb.center + normal * obb.halfSize[axisIndex] - obb.axes[i1] * obb.halfSize[i1] + obb.axes[i2] * obb.halfSize[i2];
    face.vertices[2] = obb.center + normal * obb.halfSize[axisIndex] - obb.axes[i1] * obb.halfSize[i1] - obb.axes[i2] * obb.halfSize[i2];
    face.vertices[3] = obb.center + normal * obb.halfSize[axisIndex] + obb.axes[i1] * obb.halfSize[i1] - obb.axes[i2] * obb.halfSize[i2];
    face.normal = normal;

    return face;
}

void Collisions::Clip(std::vector<glm::vec3>& points, const glm::vec3& planeNormal, float planeDist) {
    std::vector<glm::vec3> clipped;
    if (points.empty()) return;

    glm::vec3 p1 = points.back();
    float d1 = glm::dot(p1, planeNormal) - planeDist;

    for (const auto& p2 : points) {
        float d2 = glm::dot(p2, planeNormal) - planeDist;

        if (d2 <= 0.0f) {
            if (d1 > 0.0f) {
                clipped.push_back(p1 + (p2 - p1) * (d1 / (d1 - d2)));
            }
            clipped.push_back(p2);
        }
        else if (d1 <= 0.0f) {
            clipped.push_back(p1 + (p2 - p1) * (d1 / (d1 - d2)));
        }
        p1 = p2;
        d1 = d2;
    }
    points = clipped;
}
bool Collisions::CheckOBBCollision(Entity& bodyA, Entity& bodyB, glm::vec3& normal, float& depth, std::vector<glm::vec3>& contactPoints)
{
    contactPoints.clear();
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

            float radiusA = std::abs(glm::dot(obbA.axes[0] * obbA.halfSize.x, axis)) +
                std::abs(glm::dot(obbA.axes[1] * obbA.halfSize.y, axis)) +
                std::abs(glm::dot(obbA.axes[2] * obbA.halfSize.z, axis));


            float radiusB = std::abs(glm::dot(obbB.axes[0] * obbB.halfSize.x, axis)) +
                std::abs(glm::dot(obbB.axes[1] * obbB.halfSize.y, axis)) +
                std::abs(glm::dot(obbB.axes[2] * obbB.halfSize.z, axis));

            float proj = std::abs(glm::dot(tAWorld, axis));

            float overlap = radiusA + radiusB - proj;

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

    // Contact Points
    glm::vec3 hA = obbA.halfSize;
    glm::vec3 hB = obbB.halfSize;

    OBB* ref = &obbA; OBB* inc = &obbB;
    glm::vec3 rH = hA, iH = hB;
    bool flipped = false;

    float aMax = 0, bMax = 0;
    for (int i = 0; i < 3; i++) {
        aMax = std::max(aMax, std::abs(glm::dot(normal, obbA.axes[i])));
        bMax = std::max(bMax, std::abs(glm::dot(normal, obbB.axes[i])));
    }
    if (bMax > aMax) { std::swap(ref, inc); std::swap(rH, iH); flipped = true; }

    Face incidentFace = GetIncidentFace(*inc, flipped ? -normal : normal);

    int refAxis = 0; float maxD = 0;
    for (int i = 0; i < 3; i++) {
        float d = std::abs(glm::dot(flipped ? -normal : normal, ref->axes[i]));
        if (d > maxD) { maxD = d; refAxis = i; }
    }

    int s1 = (refAxis + 1) % 3; int s2 = (refAxis + 2) % 3;
    std::vector<glm::vec3> points = { incidentFace.vertices[0], incidentFace.vertices[1], incidentFace.vertices[2], incidentFace.vertices[3] };

    Clip(points, ref->axes[s1], glm::dot(ref->center + ref->axes[s1] * rH[s1], ref->axes[s1]));
    Clip(points, -ref->axes[s1], glm::dot(ref->center - ref->axes[s1] * rH[s1], -ref->axes[s1]));
    Clip(points, ref->axes[s2], glm::dot(ref->center + ref->axes[s2] * rH[s2], ref->axes[s2]));
    Clip(points, -ref->axes[s2], glm::dot(ref->center - ref->axes[s2] * rH[s2], -ref->axes[s2]));

    glm::vec3 refNormal = ref->axes[refAxis];
    if (glm::dot(refNormal, flipped ? -normal : normal) < 0) refNormal = -refNormal;
    float planeDist = glm::dot(ref->center + refNormal * rH[refAxis], refNormal);

    for (auto& p : points) {
        float separation = glm::dot(p, refNormal) - planeDist;
        if (separation <= 0.1f) {
            contactPoints.push_back(p);
        }
    }

    if (contactPoints.empty()) {
        for (int i = 0; i < 4; i++) {
            float s = glm::dot(incidentFace.vertices[i], refNormal) - planeDist;
            if (s <= 0.1f) contactPoints.push_back(incidentFace.vertices[i]);
        }
    }

    return !contactPoints.empty();
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
