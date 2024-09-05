#include <Hazel.h>

#include "imgui/imgui.h"

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
		std::shared_ptr<Hazel::VertexBuffer> vertexBuffer;
		std::shared_ptr<Hazel::IndexBuffer>  indexBuffer;

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
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.25, 1.0);
				color = v_Color;
			}
		)";
		m_Shader.reset(new Hazel::Shader(vertexSrc, fragmentSrc));

		////////////////////////////////////////////////////////////////////////////////////////

		std::shared_ptr<Hazel::VertexBuffer> m_BlueVertexBuffer;
		std::shared_ptr<Hazel::IndexBuffer> m_BlueIndexBuffer;

		// Vertex Array
		m_BlueVertexArray.reset(Hazel::VertexArray::Create());

		float m_Bluevertices[3 * 4] = {
			-0.75f , -0.75f , 0.0f,
			 0.75f , -0.75f , 0.0f,
			 0.75f ,  0.75f , 0.0f,
			-0.75f ,  0.75f , 0.0f,
		};

		// Vertex Buffer
		m_BlueVertexBuffer.reset(Hazel::VertexBuffer::Create(m_Bluevertices, sizeof(m_Bluevertices)));

		Hazel::BufferLayout m_Bluelayout = {
			{ Hazel::ShaderDataType::Float3 , "a_Position" }
		};
		m_BlueVertexBuffer->SetLayout(m_Bluelayout);
		m_BlueVertexArray->AddVertexBuffer(m_BlueVertexBuffer);

		// Index Buffer
		uint32_t m_Blueindices[6] = { 0 , 1 , 2 , 2 , 3 , 0 };
		m_BlueIndexBuffer.reset(Hazel::IndexBuffer::Create(m_Blueindices, sizeof(m_Blueindices) / sizeof(uint32_t)));
		m_BlueVertexArray->SetIndexBuffer(m_BlueIndexBuffer);

		// Shader
		std::string m_BluevertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string m_BluefragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec4 v_Postion;

			void main()
			{
				color = vec4(0.2 , 0.3 , 0.8 , 1.0);
			}
		)";
		m_BlueShader.reset(new Hazel::Shader(m_BluevertexSrc, m_BluefragmentSrc));
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
			m_CameraRotation += m_CameraRotationSpeed;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed;


		Hazel::RenderCommand::SetClearColor({ 0.143f, 0.143f, 0.143f, 1 });
		Hazel::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Hazel::Renderer::BeginScene(m_Camera);

		Hazel::Renderer::Submit(m_BlueShader, m_BlueVertexArray);
		Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		Hazel::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnEvent(Hazel::Event& event) override
	{
	}
private:
	std::shared_ptr<Hazel::Shader> m_Shader;
	std::shared_ptr<Hazel::VertexArray> m_VertexArray;

	std::shared_ptr<Hazel::Shader> m_BlueShader;
	std::shared_ptr<Hazel::VertexArray> m_BlueVertexArray;

	Hazel::OrthographicCammera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

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