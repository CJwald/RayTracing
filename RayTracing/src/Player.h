
#include <glm/glm.hpp>
#include <vector>


class Player: {
public:
    Player(glm::vec3 position);

	bool OnUpdate(float ts);

	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3& GetDirection() const { return m_ForwardDirection; }

	float GetRotationSpeed();
	float GetRollSpeed();

    struct Settings {
        float horSens = 0.3f;
        float verSens = 0.3f;
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
    struct Atributes {
        float movementSpeed = 5.0f; // m/s
        float boostSpeedScale = 1.3f;
        float rotationSpeed = 2.0f; // TODO: may need to alter this, only used for mouse
        float maxHealth = 100.0f;
        float maxShield = 50.0f;
        float healRate = 10.0f; // HP/s
        float shieldRegenRate = 10.0f; // SP/s
    };
    Atributes m_Atributes;
    glm::vec3 m_Position{0.0f, 0.0f, 0.0f};
    glm::vec3 m_CameraPosition{-0.5f, 0.25f, 0.0f}; // Camera location in player frame
};