#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbo2D") , m_CameraController(1280.0f / 720.0f)
{
	
}

void Sandbox2D::OnAttach()
{
	m_BlueVertexArray = Hazel::VertexArray::Create();

	float m_Bluevertices[5 * 4] = {
			-0.5f , -0.5f , 0.0f,
			 0.5f , -0.5f , 0.0f,
			 0.5f ,  0.5f , 0.0f,
			-0.5f ,  0.5f , 0.0f
	};

	// Vertex Buffer
	Hazel::Ref<Hazel::VertexBuffer> m_BlueVertexBuffer;
	m_BlueVertexBuffer.reset(Hazel::VertexBuffer::Create(m_Bluevertices, sizeof(m_Bluevertices)));

	Hazel::BufferLayout m_Bluelayout = {
		{ Hazel::ShaderDataType::Float3 , "a_Position" }
	};
	m_BlueVertexBuffer->SetLayout(m_Bluelayout);
	m_BlueVertexArray->AddVertexBuffer(m_BlueVertexBuffer);

	// Index Buffer
	Hazel::Ref<Hazel::IndexBuffer> m_BlueIndexBuffer;
	uint32_t m_Blueindices[6] = { 0 , 1 , 2 , 2 , 3 , 0 };
	m_BlueIndexBuffer.reset(Hazel::IndexBuffer::Create(m_Blueindices, sizeof(m_Blueindices) / sizeof(uint32_t)));
	m_BlueVertexArray->SetIndexBuffer(m_BlueIndexBuffer);

	// Shader
	m_FlatColorShader = Hazel::Shader::Create("assets/shaders/FlatColor.glsl");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);

	// Renderer
	Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Hazel::RenderCommand::Clear();

	Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

	std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SqueraColor);

	m_FlatColorShader->Bind();
	Hazel::Renderer::Submit(m_FlatColorShader, m_BlueVertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	Hazel::Renderer::EndScene();
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