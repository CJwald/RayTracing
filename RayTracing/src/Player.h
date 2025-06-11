
#include <glm/glm.hpp>
#include <vector>


class Player: {
public:
    Player(glm::vec3 position);

	bool OnUpdate(float dt);

	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3& GetVelocity() const { return m_Velocity; }
	const glm::vec3& GetDirection() const { return m_ForwardDirection; }

	float GetRotationSpeed() { return m_rotationSpeed };
	float GetRollSpeed() { return m_rollSpeed };
    void InputMove();
    bool Move(glm::vec3 direction, float dt);
    void Rotate(float dt);

    struct Settings {
        float horSens = 0.3f;
        float verSens = 0.3f;
        float mouseSens = 1.2f;
        float adsSensMulti = 0.8f; // scale the senitivity when ads
        float adsFOVMulti = 0.8f;

        // Sight Multipliers
        float ads1xMulti = 1.0f;
        float ads1p25xMulti = 1.0f;
        float ads2p5xMulti = 1.0f;
        float ads4xMulti = 1.0f;
    };
private:
    Settings m_Settings;

    float m_movementSpeed = 5.0f; // m/s
    float m_boostSpeed = 7.5f;
    float m_rotationSpeed = 2.0f; // TODO: may need to alter this, only used for mouse
    float m_rollSpeed = 2.0f;

    float m_moveAcc = 1.0f; // m/s^2
    float m_boostAcc = 2.0f; // m/s^2
    float m_boostDecel = 1.5f; // m/s^2

    float m_boostDur = 0.0f;
    float m_maxBoostDur = 5.0f;
    float m_boostCooldown = 5.0f;
    float m_boostCooldownTime = 0.0f;
    bool m_boosting = false;
    
    float m_maxHealth = 100.0f;
    float m_maxShield = 50.0f;
    float m_healRate = 10.0f; // HP/s
    float m_shieldRegenRate = 10.0f; // SP/s
    
    glm::vec3 m_Position{0.0f, 0.0f, 0.0f};
    glm::vec3 m_Velocity{0.0f, 0.0f, 0.0f};
    glm::vec3 m_CameraPosition{-0.5f, 0.25f, 0.0f}; // Camera location in player frame

	m_ForwardDirection = glm::vec3(-1, 0.0, 0.0);
	m_UpDirection = glm::vec3(0.0, 1.0, 0.0);
	m_RightDirection = glm::vec3(0.0, 0.0, -1.0);

    glm::vec3 m_inputMoveVec{0.0f, 0.0f, 0.0f};
    glm::vec4 m_inputRotVec{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec2 m_LastMousePosition{0.0f, 0.0f};
};