#include <Hazel.h>
// ---Entry Point---------------------
#include <Hazel/Core/EntryPoint.h>
// -----------------------------------

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("Example") , m_CameraController(1920.0f/ 1080.0f)
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

		vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));

		Hazel::BufferLayout layout = {
			{ Hazel::ShaderDataType::Float3 , "a_Position" },
			{ Hazel::ShaderDataType::Float4 , "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		// Index Buffer
		uint32_t indices[3] = { 0 , 1 , 2 };
		indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// Shader
		m_Shader = m_shaderLibrary.Load("assets/shaders/VertexPosColor.glsl");

		////////////////////////////////////////////////////////////////////////////////////////

		auto textureShader = m_shaderLibrary.Load("assets/shaders/Texture.glsl");

 		// m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		// m_ChernLogoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}
	
	void OnUpdate(Hazel::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		// Renderer
		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Hazel::RenderCommand::Clear();

		Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

		//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		//
		//std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
		//std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3 ("u_Color" , m_SqueraColor );
		//
		//for (int y = 0; y < 20; ++y)
		//{
		//	for (int x = 0; x < 20; ++x)
		//	{
		//		glm::vec3 pos(x * 0.11f, y * 0.11f , 0.0f );
		//		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
		//		Hazel::Renderer::Submit(m_FlatColorShader, m_BlueVertexArray , transform);
		//	}
		//}

		//auto textureShader = m_shaderLibrary.Get("Texture");

		//m_Texture->Bind();
		//Hazel::Renderer::Submit(textureShader, m_BlueVertexArray , glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		//m_ChernLogoTexture->Bind();
		//Hazel::Renderer::Submit(textureShader, m_BlueVertexArray , glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		
		// Triangle
		 Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		Hazel::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SqueraColor));
		ImGui::End();
	}

	void OnEvent(Hazel::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}
private:
	Hazel::ShaderLibrary m_shaderLibrary;
	Hazel::Ref<Hazel::Shader> m_Shader;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;

	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::VertexArray> m_BlueVertexArray;

	Hazel::Ref<Hazel::Texture2D> m_Texture , m_ChernLogoTexture;

	Hazel::OrthographicCameraController m_CameraController;

	glm::vec3 m_SqueraColor = { 0.2f, 0.3f, 0.8f };
};

class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
	}

};


Hazel::Application* Hazel::CreatApplication()
{
	return new Sandbox();
}