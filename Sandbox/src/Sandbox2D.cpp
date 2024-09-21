#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbo2D") , m_CameraController(1920.0f / 1080.0f)
{
	
}

void Sandbox2D::OnAttach()
{
	HZ_PROFILE_FUNCTION();

	m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	HZ_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{

	HZ_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	// Renderer
	Hazel::Renderer2D::ResetStats();
	{
		HZ_PROFILE_SCOPE("Renderer Prep");
		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Hazel::RenderCommand::Clear();
	}

	static float rotation = 0.0f;
	rotation += ts * 50.0f;

	{
		HZ_PROFILE_SCOPE("Renderer Draw");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Hazel::Renderer2D::DrawRotatedQuad({ 1.0f,0.0f }, { 1.0f , 0.5f }, -45.0f, {0.1f , 0.2f , 0.3f , 1.0f});
		Hazel::Renderer2D::DrawQuad({ -1.0f,0.0f }, { 0.8f , 0.8f }, { m_SqueraColor });
		Hazel::Renderer2D::DrawQuad({ 0.0f,-0.5f }, { 0.5f , 0.5f }, { 0.8f , 0.2f , 0.3f , 1.0f });
		Hazel::Renderer2D::DrawQuad({ 0.0f , 0.0f , -0.1f }, { 20.0f , 20.0f }, m_CheckerboardTexture, 10.0f);
		Hazel::Renderer2D::DrawRotatedQuad({ -1.0f,-1.0f }, { 1.0f , 1.0f }, rotation , m_CheckerboardTexture , 10.0f);
		Hazel::Renderer2D::EndScene();

		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.6f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.f , (y + 5.0f) / 10.0f , (x + y + 6.0f) / 10.0f , 0.7f };
				Hazel::Renderer2D::DrawQuad({ x , y  }, { 0.45f , 0.45f }, color);
			}
		}
		Hazel::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");

	auto stats = Hazel::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats: ");
	ImGui::Text("Draw Calls: %d" , stats.DrawCalls);
	ImGui::Text("Quad Count: %d" , stats.QuadCount);
	ImGui::Text("Vertices: %d" , stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d" , stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SqueraColor));

	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}