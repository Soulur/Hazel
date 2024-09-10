#include <Hazel.h>

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition({ 0.0f,0.0f,0.0f })
	{
		// Vertex Array
		m_VertexArray.reset(Hazel::VertexArray::Create());

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

		Hazel::Ref<Hazel::VertexBuffer> m_BlueVertexBuffer;
		Hazel::Ref<Hazel::IndexBuffer> m_BlueIndexBuffer;

		// Vertex Array
		m_BlueVertexArray.reset(Hazel::VertexArray::Create());

		float m_Bluevertices[5 * 4] = {
			-0.5f , -0.5f , 0.0f , 0.0f , 0.0f ,
			 0.5f , -0.5f , 0.0f , 1.0f , 0.0f ,
			 0.5f ,  0.5f , 0.0f , 1.0f , 1.0f ,
			-0.5f ,  0.5f , 0.0f , 0.0f , 1.0f 
		};

		// Vertex Buffer
		m_BlueVertexBuffer.reset(Hazel::VertexBuffer::Create(m_Bluevertices, sizeof(m_Bluevertices)));

		Hazel::BufferLayout m_Bluelayout = {
			{ Hazel::ShaderDataType::Float3 , "a_Position" },
			{ Hazel::ShaderDataType::Float2 , "a_TexCoord" }
		};
		m_BlueVertexBuffer->SetLayout(m_Bluelayout);
		m_BlueVertexArray->AddVertexBuffer(m_BlueVertexBuffer);

		// Index Buffer
		uint32_t m_Blueindices[6] = { 0 , 1 , 2 , 2 , 3 , 0 };
		m_BlueIndexBuffer.reset(Hazel::IndexBuffer::Create(m_Blueindices, sizeof(m_Blueindices) / sizeof(uint32_t)));
		m_BlueVertexArray->SetIndexBuffer(m_BlueIndexBuffer);

		// Shader
		m_FlatColorShader = m_shaderLibrary.Load("assets/shaders/FlatColor.glsl");

		auto textureShader = m_shaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernLogoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}
	
	void OnUpdate(Hazel::Timestep ts) override
	{
		if (Hazel::Input::IsKeyPressed(HZ_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed * ts;

		Hazel::RenderCommand::SetClearColor({ 0.143f, 0.143f, 0.143f, 1 });
		Hazel::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Hazel::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3 ("u_Color" , m_SqueraColor );
		
		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0; x < 20; ++x)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f , 0.0f );
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Hazel::Renderer::Submit(m_FlatColorShader, m_BlueVertexArray , transform);
			}
		}

		auto textureShader = m_shaderLibrary.Get("Texture");

		m_Texture->Bind();
		Hazel::Renderer::Submit(textureShader, m_BlueVertexArray , glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_ChernLogoTexture->Bind();
		Hazel::Renderer::Submit(textureShader, m_BlueVertexArray , glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		
		// Triangle
		// Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		Hazel::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");

		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SqueraColor));
		
		ImGui::End();
	}

	void OnEvent(Hazel::Event& event) override
	{
	}
private:
	Hazel::ShaderLibrary m_shaderLibrary;
	Hazel::Ref<Hazel::Shader> m_Shader;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;

	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::VertexArray> m_BlueVertexArray;

	Hazel::Ref<Hazel::Texture2D> m_Texture , m_ChernLogoTexture;

	Hazel::OrthographicCammera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec3 m_SqueraColor = { 0.2f, 0.3f, 0.8f };
};

class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};


Hazel::Application* Hazel::CreatApplication()
{
	return new Sandbox();
}