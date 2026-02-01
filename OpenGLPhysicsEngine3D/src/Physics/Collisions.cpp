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

// Исправленный клиппинг: теперь он более строгий к направлениям
void Collisions::Clip(std::vector<glm::vec3>& points, const glm::vec3& planeNormal, float planeDist) {
    std::vector<glm::vec3> clipped;
    if (points.empty()) return;

    glm::vec3 p1 = points.back();
    float d1 = glm::dot(p1, planeNormal) - planeDist;

    for (const auto& p2 : points) {
        float d2 = glm::dot(p2, planeNormal) - planeDist;

        // Точка p2 внутри (дистанция <= 0, так как нормаль смотрит НАРУЖУ)
        if (d2 <= 0.0f) {
            if (d1 > 0.0f) {
                // Переход снаружи внутрь
                clipped.push_back(p1 + (p2 - p1) * (d1 / (d1 - d2)));
            }
            clipped.push_back(p2);
        }
        else if (d1 <= 0.0f) {
            // Переход изнутри наружу
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

    // Возвращаем как было, раз 0.5 дает провал
    glm::vec3 hA = obbA.halfSize;
    glm::vec3 hB = obbB.halfSize;

    glm::vec3 tA_world = obbB.center - obbA.center;

    float R[3][3], AbsR[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            R[i][j] = glm::dot(obbA.axes[i], obbB.axes[j]);
            AbsR[i][j] = std::abs(R[i][j]) + 1e-6f;
        }
    }

    auto TestAxis = [&](glm::vec3 axis, float overlap) {
        float len = glm::length(axis);
        if (len < 1e-6f) return true;
        axis /= len;
        overlap /= len;

        if (overlap < 0.0f) return false;
        if (overlap < depth) {
            depth = overlap;
            if (glm::dot(axis, tA_world) < 0.0f) axis = -axis;
            normal = axis;
        }
        return true;
        };

    // SAT Тесты
    for (int i = 0; i < 3; i++) {
        float ra = hA[i];
        float rb = hB[0] * AbsR[i][0] + hB[1] * AbsR[i][1] + hB[2] * AbsR[i][2];
        if (!TestAxis(obbA.axes[i], ra + rb - std::abs(glm::dot(tA_world, obbA.axes[i])))) return false;
    }
    for (int i = 0; i < 3; i++) {
        float ra = hA[0] * AbsR[0][i] + hA[1] * AbsR[1][i] + hA[2] * AbsR[2][i];
        float rb = hB[i];
        if (!TestAxis(obbB.axes[i], ra + rb - std::abs(glm::dot(tA_world, obbB.axes[i])))) return false;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            glm::vec3 axis = glm::cross(obbA.axes[i], obbB.axes[j]);
            if (glm::length(axis) < 1e-6f) continue;
            float ra = hA[(i + 1) % 3] * AbsR[(i + 2) % 3][j] + hA[(i + 2) % 3] * AbsR[(i + 1) % 3][j];
            float rb = hB[(j + 1) % 3] * AbsR[i][(j + 2) % 3] + hB[(j + 2) % 3] * AbsR[i][(j + 1) % 3];
            if (!TestAxis(axis, ra + rb - std::abs(glm::dot(tA_world, axis)))) return false;
        }
    }

    // Поиск манифольда
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

    // Клиппинг
    Clip(points, ref->axes[s1], glm::dot(ref->center + ref->axes[s1] * rH[s1], ref->axes[s1]));
    Clip(points, -ref->axes[s1], glm::dot(ref->center - ref->axes[s1] * rH[s1], -ref->axes[s1]));
    Clip(points, ref->axes[s2], glm::dot(ref->center + ref->axes[s2] * rH[s2], ref->axes[s2]));
    Clip(points, -ref->axes[s2], glm::dot(ref->center - ref->axes[s2] * rH[s2], -ref->axes[s2]));

    glm::vec3 refNormal = ref->axes[refAxis];
    if (glm::dot(refNormal, flipped ? -normal : normal) < 0) refNormal = -refNormal;
    float planeDist = glm::dot(ref->center + refNormal * rH[refAxis], refNormal);

    // Увеличим порог фильтрации до 0.1 для надежности
    for (auto& p : points) {
        float separation = glm::dot(p, refNormal) - planeDist;
        if (separation <= 0.1f) {
            contactPoints.push_back(p);
        }
    }

    // Гвоздь программы: Если после клиппинга пусто, SAT все равно требует точки!
    if (contactPoints.empty()) {
        // Берем оригинальные вершины грани инцидентного объекта
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
