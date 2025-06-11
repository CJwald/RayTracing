#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Walnut/Random.h"

#include "Renderer.h"
#include "Camera.h"
#include "Response.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(85.0f, 0.1f, 1000000.0f)
		
	{
		Material& pinkSphere = m_Scene.Materials.emplace_back();
		pinkSphere.Albedo = { 1.0f, 0.0f, 1.0f };
		pinkSphere.Roughness = 0.0f;
		pinkSphere.EmissionColor = pinkSphere.Albedo;
		pinkSphere.EmissionPower = 0.5f;

		Material& blueSphere = m_Scene.Materials.emplace_back();
		blueSphere.Albedo = { 0.2f, 0.3f, 1.0f };
		blueSphere.Roughness = 0.1f;
		blueSphere.EmissionColor = blueSphere.Albedo;
		blueSphere.EmissionPower = 0.5f;

		Material& greenSphere = m_Scene.Materials.emplace_back();
		greenSphere.Albedo = { 0.2f, 0.9f, 0.0f };// { 0.8f, 0.5f, 0.2f };
		greenSphere.Roughness = 0.9f;
		greenSphere.EmissionColor = greenSphere.Albedo;
		greenSphere.EmissionPower = 0.5f;

		Material& orangeSphere = m_Scene.Materials.emplace_back();
		orangeSphere.Albedo = { 0.9f, 0.2f, 0.0f };// { 0.8f, 0.5f, 0.2f };
		orangeSphere.Roughness = 0.1f;
		orangeSphere.EmissionColor = orangeSphere.Albedo;
		orangeSphere.EmissionPower = 15.7f;

		Material& blackSphere = m_Scene.Materials.emplace_back();
		blackSphere.Albedo = { 0.7f, 0.7f, 0.7f };// { 0.8f, 0.5f, 0.2f };
		blackSphere.Roughness = 0.9f;
		blackSphere.EmissionColor = blackSphere.Albedo;
		blackSphere.EmissionPower = 0.0f;
		

		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.5f, 0.0f };
			sphere.Radius = 0.5f;
			sphere.MaterialIndex = 4;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 0.0f, -10.0f, 0.0f };
			sphere.Radius = 10.0f;
			sphere.MaterialIndex = 2;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 5.0f, 0.5f, 0.0f };
			sphere.Radius = 0.5f;
			sphere.MaterialIndex = 3;
			m_Scene.Spheres.push_back(sphere);
		}


		{//////
			BoundingSphere border;
			border.Position = { 0.0f, 0.0f, 0.0f };
			border.Radius = 100.0f;
			m_Scene.Borders.push_back(border);
		}//////

		
		#define DOSTARS 1
		#if DOSTARS

		float scale = 100.0f;
		int gridNum = 3;
		for (size_t m = 0; m < gridNum; m++) {
			for (size_t n = 0; n < gridNum; n++) {
				for (size_t k = 0; k < gridNum; k++) {
					Sphere sphere;
					glm::vec3 loc = scale * Walnut::Random::Vec3(-1.0f, 1.0f) / glm::length(Walnut::Random::Vec3(-1.0f, 1.0f));
					sphere.Position = {
						loc[0],
						loc[1],
						loc[2]
					};
					//sphere.Position = { 
					//	float(m) * scale - gridNum / 2.0f, 
					//	float(n) * scale - gridNum / 2.0f,
					//	float(k) * scale - gridNum / 2.0f 
					//};
					sphere.Radius = 1.25f;
					sphere.MaterialIndex = 3;
					m_Scene.Spheres.push_back(sphere);
				}
			}
		}
		#endif	
		

	}

	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.OnUpdate(ts))
			m_Renderer.ResetFrameIndex();
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}

		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		ImGui::DragFloat("Max Ray Distance", &m_Renderer.GetSettings().maxRayTravelDist, 0.1f);
		ImGui::DragInt("Max Recursion Depth", &m_Renderer.GetSettings().maxRecursionDepth, 3);

		if (ImGui::Button("Reset"))
			m_Renderer.ResetFrameIndex();
			m_Renderer.ResetFrameIndex();

		ImGui::End();

		ImGui::Begin("Scene");
		for (size_t i = 0; i < m_Scene.Borders.size(); i++)
		{
			ImGui::PushID(i);
			
			BoundingSphere& sphere = m_Scene.Borders[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
			
			ImGui::Separator();

			ImGui::PopID();
		}
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);

			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
			ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1);

			ImGui::Separator();

			ImGui::PopID();
		}

		for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		{
			ImGui::PushID(i);

			Material& material = m_Scene.Materials[i];
			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Metallic", &material.Metallic, 0.05f, 0.0f, 1.0f);
			ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.EmissionColor));
			ImGui::DragFloat("Emission Power", &material.EmissionPower, 0.05f, 0.0f, FLT_MAX);

			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}