

#include "Response.h"

Response::Response(float rotRate, float sensitivity) : m_RotRate(rotRate), m_Sensitivity(sensitivity) {
	m_RotRate = 0.0f;
	
}

void Response::OnUpdate(float dt, float input) {
	m_RotRate = m_RotRate + input*(m_Sensitivity*m_RotRateLimit/m_RotAccTime)*dt;
}