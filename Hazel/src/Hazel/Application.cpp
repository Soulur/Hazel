#include "hzpch.h"
#include "Application.h"

#include "Hazel/Log.h"

#include "Renderer/Renderer.h"

#include "Input.h"

namespace Hazel {

#define BIND_EVENT_FN(x) std::bind(&Application::x , this , std::placeholders::_1)

	Application* Application::s_Instace = nullptr;

	Application::Application()
		: m_Camera(-1.6f , 1.6f , -0.9f , 0.9f )
	{
		HZ_CORE_ASSERT(!s_Instace, "Application already exists!");
		s_Instace = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		// Vertex Array
		m_VertexArray.reset(VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f , -0.5f , 0.0f, 1.0f , 0.0f , 1.0f , 1.0f,
			 0.5f , -0.5f , 0.0f, 0.0f , 0.0f , 1.0f , 1.0f,
			 0.0f ,  0.5f , 0.0f, 1.0f , 1.0f , 0.0f , 1.0f
		};

		// Vertex Buffer
		std::shared_ptr<VertexBuffer> vertexBuffer;
		std::shared_ptr<IndexBuffer> indexBuffer;

		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		BufferLayout layout = {
			{ ShaderDataType::Float3 , "a_Position" },
			{ ShaderDataType::Float4 , "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		// Index Buffer
		uint32_t indices[3] = { 0 , 1 , 2 };
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof (uint32_t) ));
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
		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		////////////////////////////////////////////////////////////////////////////////////////

		std::shared_ptr<VertexBuffer> m_BlueVertexBuffer;
		std::shared_ptr<IndexBuffer> m_BlueIndexBuffer;

		// Vertex Array
		m_BlueVertexArray.reset(VertexArray::Create());

		float m_Bluevertices[3 * 4] = {
			-0.75f , -0.75f , 0.0f,
			 0.75f , -0.75f , 0.0f,
			 0.75f ,  0.75f , 0.0f,
			-0.75f ,  0.75f , 0.0f,
		};

		// Vertex Buffer
		m_BlueVertexBuffer.reset(VertexBuffer::Create(m_Bluevertices, sizeof(m_Bluevertices)));

		BufferLayout m_Bluelayout = {
			{ ShaderDataType::Float3 , "a_Position" }
		};
		m_BlueVertexBuffer->SetLayout(m_Bluelayout);
		m_BlueVertexArray->AddVertexBuffer(m_BlueVertexBuffer);

		// Index Buffer
		uint32_t m_Blueindices[6] = { 0 , 1 , 2 , 2 , 3 , 0 };
		m_BlueIndexBuffer.reset(IndexBuffer::Create(m_Blueindices, sizeof(m_Blueindices) / sizeof(uint32_t)));
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
		m_BlueShader.reset(new Shader(m_BluevertexSrc, m_BluefragmentSrc));
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		//HZ_CORE_TRACE( "{0}" , e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled())
				break;
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
			RenderCommand::SetClearColor( { 0.143f, 0.143f, 0.143f, 1 });
			RenderCommand::Clear();

			m_Camera.SetPosition({ 1.0f , 1.0f , 0.0f });
			m_Camera.SetRotation(45.0f);

			Renderer::BeginScene(m_Camera);

			Renderer::Submit(m_BlueShader , m_BlueVertexArray);
			Renderer::Submit(m_Shader , m_VertexArray);

			Renderer::EndScene();

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}

	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

}