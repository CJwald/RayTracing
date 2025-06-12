#include "Player.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Walnut/Input/Input.h"

using namespace Walnut;

Player::Player(glm::vec3 position, glm::vec3 velocity) : m_Position(position), m_Velocity(velocity) {
	m_Position = glm::vec3{0.0f, 0.0f, 0.0f};
	m_Velocity = glm::vec3{0.0f, 0.0f, 0.0f};
}

bool Player::OnUpdate(float dt) {

    // Input check
    //   Mouse / Key
    InputMove(); // Get the movement inputs
    //InputRot(dt);
    //   Controller

    // Boost status updating
    if (m_boosting) {
        if (m_boostDur >= m_maxBoostDur) {
            m_boosting = false;
            m_boostCooldownTime = m_boostCooldown;
        } else {
            m_boostDur += dt;
        } 
    }
    if (m_boostCooldown > 0.0f) {
        m_boostCooldown -= dt;
    } else {
        m_boostCooldown = 0.0f;
    }

    // Move
    Move(dt);

    // Rotate
    Rotate(dt);
    

    return true;
}

bool Player::Move(float dt) {
    bool moved = false;
    if (m_boosting) {
        m_Velocity += m_inputMoveVec * m_boostAcc * dt; 
        if (glm::length(m_Velocity) >= m_boostSpeed) {
            m_Velocity = glm::normalize(m_Velocity) * m_boostSpeed;
        }
    } else {
        if (glm::length(m_Velocity) > m_movementSpeed) {
            m_Velocity += m_inputMoveVec * dt * (m_moveAcc - m_boostDecel); 
        } else {
            m_Velocity += m_inputMoveVec * m_moveAcc * dt; 
            if (glm::length(m_Velocity) >= m_movementSpeed) {
                m_Velocity = glm::normalize(m_Velocity) * m_movementSpeed;
            }
        }
    }

    m_Position += m_inputMoveVec * glm::length(m_Velocity) * dt;
    moved = true;
    return moved;
}

void Player::Rotate(float dt) {
    
}

void Player::InputMove() {
    m_inputMoveVec = {0.0f, 0.0f, 0.0f};

    if (Input::IsKeyDown(KeyCode::W)) {                // FORWARD
        m_inputMoveVec[0] += 1.0f;
    } else if (Input::IsKeyDown(KeyCode::S)) {         // BACKWARD
        m_inputMoveVec[0] -= 1.0f;
	}
	if (Input::IsKeyDown(KeyCode::Space)) {            // UP
        m_inputMoveVec[1] += 1.0f;
	} else if (Input::IsKeyDown(KeyCode::C)) {         // DOWN
        m_inputMoveVec[2] -= 1.0f;
	}
	if (Input::IsKeyDown(KeyCode::D)) {                // RIGHT
        m_inputMoveVec[2] += 1.0f;
	} else if (Input::IsKeyDown(KeyCode::A)) {         // LEFT
        m_inputMoveVec[2] -= 1.0f;
	}

    glm::normalize(m_inputMoveVec);
}

//void Player::InputRot(float dt) {

    // m_inputRotVec = {1.0f, 0.0f, 0.0f, 0.0f};

    // glm::vec2 mousePos = Input::GetMousePosition();
	// glm::vec2 delta = (mousePos - m_LastMousePosition) * 0.002f;
	// m_LastMousePosition = mousePos;

    // // Roll
    // // Keyboard
	// if (Input::IsKeyDown(KeyCode::E)) {         // ROLL RIGHT
	// 	float rollDelta = dt * -GetRollSpeed();
	// 	glm::quat q = glm::normalize(glm::angleAxis(-rollDelta, m_ForwardDirection));
	// 	m_UpDirection = glm::rotate(q, m_UpDirection);
	// 	m_RightDirection = glm::rotate(q, m_RightDirection);
	// 	moved = true;
	// } else if (Input::IsKeyDown(KeyCode::Q)) {         // ROLL LEFT
	// 	float rollDelta = dt * GetRollSpeed();
	// 	glm::quat q = glm::normalize(glm::angleAxis(-rollDelta, m_ForwardDirection));
	// 	m_UpDirection = glm::rotate(q, m_UpDirection);
	// 	m_RightDirection = glm::rotate(q, m_RightDirection);
	// 	moved = true;
	// }
    // // TODO: Controller Roll

    // // Pitch & Yaw
    // // Mouse
	// if (delta.x != 0.0f || delta.y != 0.0f) {
	// 	float pitchDelta = delta.y * GetRotationSpeed() * m_Settings.mouseSens;
	// 	float yawDelta = delta.x * GetRotationSpeed() * m_Settings.mouseSens;

	// 	glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, m_RightDirection),
	// 		glm::angleAxis(-yawDelta, m_UpDirection)));
	// 	m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

	// 	moved = true;
	// }

//}

