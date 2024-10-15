#include "ExampleLayer.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


ExampleLayer::ExampleLayer()
	: Layer("Example"), m_CameraController(1920.0f / 1080.0f)
{
	// Vertex Array
	m_VertexArray = Hazel::VertexArray::Create();

	float vertices[3 * 7] = {
		-0.5f , -0.5f , 0.0f, 1.0f , 0.0f , 1.0f , 1.0f,
		 0.5f , -0.5f , 0.0f, 0.0f , 0.0f , 1.0f , 1.0f,
		 0.0f ,  0.5f , 0.0f, 1.0f , 1.0f , 0.0f , 1.0f
	};

	// Vertex Buffer
	Hazel::Ref<Hazel::VertexBuffer> vertexBuffer;
	Hazel::Ref<Hazel::IndexBuffer>  indexBuffer;

	vertexBuffer = Hazel::VertexBuffer::Create(vertices, sizeof(vertices));

	Hazel::BufferLayout layout = {
		{ Hazel::ShaderDataType::Float3 , "a_Position" },
		{ Hazel::ShaderDataType::Float4 , "a_Color" }
	};
	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	// Index Buffer
	uint32_t indices[3] = { 0 , 1 , 2 };
	indexBuffer = Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	// Shader
	m_Shader = m_ShaderLibrary.Load("assets/shaders/VertexPosColor.glsl");

	////////////////////////////////////////////////////////////////////////////////////////

	auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

	 m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
	 m_ChernoLogoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");

	textureShader->Bind();
	textureShader->SetInt("u_Texture", 0);
}

void ExampleLayer::OnAttach()
{
}

void ExampleLayer::OnDetach()
{
}

void ExampleLayer::OnUpdate(Hazel::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);

	// Renderer
	Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Hazel::RenderCommand::Clear();

	Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	
	m_FlatColorShader->Bind();
	m_FlatColorShader->SetFloat3("u_Color" , m_SquareColor);
	
	for (int y = 0; y < 20; ++y)
	{
		for (int x = 0; x < 20; ++x)
		{
			glm::vec3 pos(x * 0.11f, y * 0.11f , 0.0f );
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
			Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
		}
	}

	auto textureShader = m_ShaderLibrary.Get("Texture");

	m_Texture->Bind();
	Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
	m_SquareVA->Bind();
	Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	// Triangle
	// Hazel::Renderer::Submit(m_Shader, m_VertexArray);

	Hazel::Renderer::EndScene();
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Settings"); 
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void ExampleLayer::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}