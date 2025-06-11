#include "Player.h"

Player::Player(glm::vec3 position) : m_Position(position) {
	m_Position = glm::vec3{0.0f, 0.0f, 0.0f};
}

bool Player::OnUpdate(float ts) {
    return false;
}


float Camera::GetRotationSpeed() {
	return m_attributes.rotationSpeed;
}

float Camera::GetRollSpeed() {
	return m_attributes.rollSpeed;
}