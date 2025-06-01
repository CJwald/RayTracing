#include "Renderer.h"

#include "Walnut/Random.h"
//#include <iostream>
#include <execution>

namespace Utils {

	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

	static uint32_t PCG_Hash(uint32_t input)
	{
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	static float RandomFloat(uint32_t& seed)
	{
		seed = PCG_Hash(seed);
		return (float)seed / (float)std::numeric_limits<uint32_t>::max();
	}

	static glm::vec3 InUnitSphere(uint32_t& seed)
	{
		return glm::normalize(glm::vec3(
			RandomFloat(seed) * 2.0f - 1.0f, 
			RandomFloat(seed) * 2.0f - 1.0f, 
			RandomFloat(seed) * 2.0f - 1.0f)
		);
	}
}


void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		// No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

	m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);
	for (uint32_t i = 0; i < width; i++)
		m_ImageHorizontalIter[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_ImageVerticalIter[i] = i;
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;
	
	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

#define MT 1
#if MT
	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
		[this](uint32_t y)
		{
			std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
				[this, y](uint32_t x)
				{
					glm::vec4 color = PerPixel(x, y);
					m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

					glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
					accumulatedColor /= (float)m_FrameIndex;

					accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
					m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
				});
		});

#else

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec4 color = PerPixel(x, y);
			m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

			glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
			accumulatedColor /= (float)m_FrameIndex;

			accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
		}
	}
#endif

	m_FinalImage->SetData(m_ImageData);

	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];
	glm::vec3 centerFov = m_ActiveCamera->GetDirection();

	ray.Traveled = 0.0f;
	
	glm::vec3 light(0.0f);
	glm::vec3 contribution(1.0f);

	uint32_t seed = x + y * m_FinalImage->GetWidth();
	seed *= m_FrameIndex;

	int bounces = 5;
	uint32_t recursionDepth = 0;
	for (int i = 0; i < bounces; i++)
	{
		seed += i;

		Renderer::HitPayload payload = TraceRay(ray, recursionDepth, centerFov);
		if (payload.HitDistance < 0.0f) { break; }

		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];

		contribution *= material.Albedo;
		light += (material.GetEmission() + contribution) * (100.0f - ray.Traveled)/100.0f;

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		ray.Direction = glm::normalize(payload.WorldNormal + Utils::InUnitSphere(seed));
	}

	return glm::vec4(light, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(Ray& ray, uint32_t depth, glm::vec3 centerFov)
{
	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	// where
	// a = ray origin
	// b = ray direction
	// r = radius
	// t = hit distance
	uint32_t maxRecursion = 10;
	depth += 1;
	float maxTravel = 100.0f;
	float closestB = 0.0f;
	float farthestB = 0.0f;

	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (size_t j = 0; j < m_ActiveScene->Borders.size(); j++)
	{
		for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
		{
			const Sphere& sphere = m_ActiveScene->Spheres[i];
			glm::vec3 origin = ray.Origin - sphere.Position;

			float a = glm::dot(ray.Direction, ray.Direction);
			float b = 2.0f * glm::dot(origin, ray.Direction);
			float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

			// Quadratic forumula discriminant:
			// b^2 - 4ac

			float discriminant = b * b - 4.0f * a * c;
			if (discriminant < 0.0f)
				continue;

			// Quadratic formula:
			// (-b +- sqrt(discriminant)) / 2a

			// float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)
			float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
			if (closestT > 0.0f && closestT < hitDistance)
			{
				hitDistance = closestT;
				//hitDistance = closestT + ray.Traveled; // TODO: maybe need this
				closestSphere = (int)i;
			}
		}

		if (closestSphere < 0) { // no sphere hit, do border check
			const BoundingSphere& border = m_ActiveScene->Borders[j];
			glm::vec3 originb = ray.Origin - border.Position;

			float a = glm::dot(centerFov, centerFov);
			float b = 2.0f * glm::dot(originb, centerFov);
			float c = glm::dot(originb, originb) - border.Radius * border.Radius;

			// Quadratic forumula discriminant:
			// b^2 - 4ac

			float discriminant = b * b - 4.0f * a * c;
			if (discriminant < 0.0f)
				continue;

			// Quadratic formula:
			// (-b +- sqrt(discriminant)) / 2a

			// float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)
			//closestB = (-b - glm::sqrt(discriminant)) / (2.0f * a);
			farthestB = (-b + glm::sqrt(discriminant)) / (2.0f * a);


			if (farthestB > 0.0f)
			{
				ray.Traveled += farthestB; // TODO: This doesnt work for some reason
				//std::cout << "ray.Traveled: " << ray.Traveled << std::endl;
				//std::cout << depth << std::endl;
			}
		}

		
		
	}

	if (closestSphere < 0)
		if ((ray.Traveled < maxTravel) && (depth < maxRecursion)){
			
			//glm::vec3 originShift = -2.0f * (ray.Direction * farthestB + ray.Origin); // this isnt correct, needs to shift -border hit location
			glm::vec3 originShift = -ray.Origin - 2.0f * (centerFov * farthestB); // this isnt correct, needs to shift -border hit location
			ray.Origin = ray.Origin + originShift;
			
			return TraceRay(ray, depth, centerFov);
		}else {
			return Miss(ray);
		}

	return ClosestHit(ray, hitDistance, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

	glm::vec3 origin = ray.Origin - closestSphere.Position;
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestSphere.Position;

	return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}
