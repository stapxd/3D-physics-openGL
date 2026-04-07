#include "GJK.h"

#include <helpers.h>

std::vector<glm::vec3> GJK::GetOBBVertices(const OBB& obb)
{
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

bool GJK::Solve(Entity& bodyA, Entity& bodyB, Simplex& outSimplex, std::vector<glm::vec3>* minkowskiDifference)
{
    std::vector<glm::vec3> vA = GetOBBVertices(bodyA->GetOBB());
    std::vector<glm::vec3> vB = GetOBBVertices(bodyB->GetOBB());

#ifdef GJK_DEBUG
    for (auto itA = vA.begin(); itA != vA.end(); itA++)
        for (auto itB = vB.begin(); itB != vB.end(); itB++)
            minkowskiDifference->push_back(*itB - *itA);
#endif

    Simplex simplex;
    simplex.count = 1;

    glm::vec3 direction = vA[0] - vB[0];
    simplex.a = Support(vB, direction) - Support(vA, -direction);
    direction = -simplex.a;

    while (true) {

        simplex.d = simplex.c;
        simplex.c = simplex.b;
        simplex.b = simplex.a;
        simplex.count++;

        simplex.a = Support(vB, direction) - Support(vA, -direction);

        if (glm::dot(simplex.a, direction) < 0.0f) {
            return false;
        }

        bool collide = false;
        switch (simplex.count)
        {
        case 2:
            DoSimplexLine(simplex, direction);
            break;
        case 3:
            DoSimplexTriangle(simplex, direction);
            break;
        case 4:
            DoSimplexTetrahedron(simplex, direction, collide);
            if (collide) {
                outSimplex = simplex;
                return true;
            }
            break;
        default:
            helpers::logError("GJK::Solve", "invalid simplex dimension");
            return false;
        }
    }

    outSimplex = simplex;
    return false;
}

glm::vec3 GJK::Support(std::vector<glm::vec3> vertices, glm::vec3 direction)
{
    float largestDot = glm::dot(vertices[0], direction);
    int largestVertexIdx = 0;

    float currentDot = 0.0f;
    for (size_t i = 1; i < vertices.size(); i++) {
        currentDot = glm::dot(vertices[i], direction);
        if (currentDot > largestDot) {
            largestDot = currentDot;
            largestVertexIdx = i;
        }
    }

    return vertices[largestVertexIdx];
}

void GJK::DoSimplexLine(Simplex& simplex, glm::vec3& direction)
{
    glm::vec3 ba = simplex.b - simplex.a;
    glm::vec3 ao = -simplex.a;

    if (SameDirection(ba, ao)) {
        direction = glm::cross(ba, ao);
        return;
    }
    else {
        simplex.count = 1;
        direction = ao;
    }
}

void GJK::DoSimplexTriangle(Simplex& simplex, glm::vec3& direction)
{
    glm::vec3 abc = glm::cross(simplex.b - simplex.a, simplex.c - simplex.a);
    glm::vec3 ac = simplex.c - simplex.a;
    glm::vec3 ao = -simplex.a;
    glm::vec3 ab = simplex.b - simplex.a;

    if (SameDirection(glm::cross(abc, ac), ao)) {
        if (SameDirection(ac, ao)) {
            // the origin is nearest to the line ac
            simplex.b = simplex.c;
            simplex.count = 2;
            direction = glm::cross(glm::cross(ac, ao), ac);
            return;
        }
        else {
            if (SameDirection(ab, ao)) {
                // the origin is nearest to the line ab

                simplex.count = 2;
                direction = glm::cross(glm::cross(ab, ao), ab);
                return;
            }
            else {
                // the origin is nearest to the point a
                simplex.count = 1;
                direction = ao;
                return;
            }
        }
    }
    else {
        if (SameDirection(glm::cross(ab, abc), ao)) {
            if (SameDirection(ab, ao)) {
                // the origin is nearest to the line ab

                simplex.count = 2;
                direction = ao;
                return;
            }
            else {
                simplex.count = 1;
                direction = ao;
                return;
            }
        }
        else {
            if (SameDirection(abc, ao)) {
                // the origin is nearest to triangle abc
                direction = abc;
                return;
            }
            else {
                glm::vec3 t = simplex.b;
                simplex.b = simplex.c;
                simplex.c = t;
                direction = -abc;
                return;
            }
        }
    }

}

void GJK::DoSimplexTetrahedron(Simplex& simplex, glm::vec3& direction, bool& collide)
{
    glm::vec3 abc = glm::cross(simplex.b - simplex.a, simplex.c - simplex.a);
    glm::vec3 acd = glm::cross(simplex.c - simplex.a, simplex.d - simplex.a);
    glm::vec3 adb = glm::cross(simplex.d - simplex.a, simplex.b - simplex.a);
    glm::vec3 ao = -simplex.a;

    if (SameDirection(abc, ao)) {
        simplex.count = 3;
        DoSimplexTriangle(simplex, direction);
        return;
    }

    if (SameDirection(acd, ao)) {
        simplex.b = simplex.c;
        simplex.c = simplex.d;
        simplex.count = 3;
        DoSimplexTriangle(simplex, direction);
        return;
    }

    if (SameDirection(abc, ao)) {
        simplex.c = simplex.b;
        simplex.b = simplex.d;
        simplex.count = 3;
        DoSimplexTriangle(simplex, direction);
        return;
    }

    direction = glm::vec3(0.0f);
    collide = true;
}

bool GJK::SameDirection(glm::vec3 v1, glm::vec3 v2)
{
    float dot = glm::dot(v1, v2);

    if (dot >= 0.0f) {
        return true;
    }
    
    return false;
}
