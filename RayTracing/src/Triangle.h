#pragma once

#include <glm/glm.hpp>

struct Triangle {
    Triangle(glm::vec3 Verts);
	glm::vec3 Verts;
	glm::vec3 Normal;
};