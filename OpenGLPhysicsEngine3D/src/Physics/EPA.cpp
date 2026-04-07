#include "EPA.h"
#include "GJK.h"

EPAResult EPA::Solve(const Simplex& simplex, Entity& bodyA, Entity& bodyB)
{
    std::vector<glm::vec3> vA = GJK::GetOBBVertices(bodyA->GetOBB());
    std::vector<glm::vec3> vB = GJK::GetOBBVertices(bodyB->GetOBB());

    std::vector<glm::vec3> polytope = {
        simplex.a, simplex.b, simplex.c, simplex.d
    };

    std::vector<unsigned int> faces = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 1,
        1, 3, 2
    };

    std::vector<glm::vec4> normals;
    size_t minFace = 0;

    std::tie(normals, minFace) = GetFaceNormals(polytope, faces);

    glm::vec3 minNormal;
    float minDistance = FLT_MAX;

    for (int iterations = 0; iterations < 64; iterations++) {
        minNormal = glm::vec3(normals[minFace]);
        minDistance = normals[minFace].w;

        glm::vec3 support = GJK::Support(vA, minNormal) - GJK::Support(vB, -minNormal);
        float sDistance = glm::dot(minNormal, support);

        if (abs(sDistance - minDistance) < 0.001f) {
            break;
        }

        std::vector<std::pair<unsigned int, unsigned int>> uniqueEdges;

        for (size_t i = 0; i < normals.size(); i++) {
            if (glm::dot(glm::vec3(normals[i]), support) > 0) {
                size_t f = i * 3;

                AddIfUniqueEdge(uniqueEdges, faces, f + 0, f + 1);
                AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
                AddIfUniqueEdge(uniqueEdges, faces, f + 2, f + 0);

                faces[f + 2] = faces.back(); faces.pop_back();
                faces[f + 1] = faces.back(); faces.pop_back();
                faces[f + 0] = faces.back(); faces.pop_back();
                normals[i] = normals.back(); normals.pop_back();
                i--;
            }
        }

        unsigned int newVertexIndex = (unsigned int)polytope.size();
        polytope.push_back(support);

        for (auto& edge : uniqueEdges) {
            faces.push_back(edge.first);
            faces.push_back(edge.second);
            faces.push_back(newVertexIndex);
        }

        std::tie(normals, minFace) = GetFaceNormals(polytope, faces);
    }

    EPAResult result;
    result.normal = minNormal;
    result.depth = minDistance + 0.001f;
    return result;
}

std::pair<std::vector<glm::vec4>, size_t> EPA::GetFaceNormals(const std::vector<glm::vec3>& polytope, const std::vector<unsigned int>& faces)
{
    std::vector<glm::vec4> normals;
    size_t minTriangle = 0;
    float minDistance = FLT_MAX;

    for (size_t i = 0; i < faces.size(); i += 3) {
        glm::vec3 a = polytope[faces[i + 0]];
        glm::vec3 b = polytope[faces[i + 1]];
        glm::vec3 c = polytope[faces[i + 2]];

        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
        float distance = glm::dot(normal, a);


        if (distance < 0) {
            normal = -normal;
            distance = -distance;
        }

        normals.emplace_back(normal, distance);

        if (distance < minDistance) {
            minDistance = distance;
            minTriangle = i / 3;
        }
    }

    return { normals, minTriangle };
}

void EPA::AddIfUniqueEdge(std::vector<std::pair<unsigned int, unsigned int>>& edges, const std::vector<unsigned int>& faces, size_t a, size_t b)
{
    auto reverse = std::find(edges.begin(), edges.end(), std::make_pair(faces[b], faces[a]));

    if (reverse != edges.end()) {
        edges.erase(reverse);
    }
    else {
        edges.emplace_back(faces[a], faces[b]);
    }
}