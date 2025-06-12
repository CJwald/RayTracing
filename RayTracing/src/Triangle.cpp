#include "Triangle.h"
#include <glm/glm.hpp>
#include <vector>

Triangle::Triangle(std::vector<glm::vec3> V) : Verts(V) {
    glm::vec3 e1 = V[1] - V[0];
    glm::vec3 e2 = V[2] - V[0];
    m_Normal = glm::cross(e1, e2); // note, this assumes points are stored in CCW order
}
