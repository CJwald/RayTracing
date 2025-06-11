#pragma once

#include <glm/glm.hpp>
#include <vector>

class Response {
public:
	Response(float rotRate, float sensitivity);
	void OnUpdate(float dt, float input);

private:
	float m_RotRate = 0.0f; // deg per second
	float m_Sensitivity = 0.3f;
	float m_RotRateLimit = 1000.0f; // deg per second
	float m_RotAccTime = 0.25f; // seconds
	//float m_RotAcc = 

};
