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
	{
		HZ_PROFILE_SCOPE("Renderer Prep");
		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Hazel::RenderCommand::Clear();
	}

	{
		HZ_PROFILE_SCOPE("Renderer Draw");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Hazel::Renderer2D::DrawQuad({ -1.0f,0.0f }, { 0.8f , 0.8f }, { m_SqueraColor });
		Hazel::Renderer2D::DrawQuad({ 0.0f,0.0f }, { 1.0f , 1.0f }, { 0.8f , 0.2f , 0.3f , 1.0f });
		Hazel::Renderer2D::DrawQuad({ 0.0f,0.0f, -0.1f }, { 5.0f , 5.0f }, m_CheckerboardTexture);
		Hazel::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SqueraColor));

	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}