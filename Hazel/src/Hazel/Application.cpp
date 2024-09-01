#include "hzpch.h"
#include "Application.h"

#include "Hazel/Log.h"

#include <glad/glad.h>

#include "Input.h"

namespace Hazel {

#define BIND_EVENT_FN(x) std::bind(&Application::x , this , std::placeholders::_1)

	Application* Application::s_Instace = nullptr;

	Application::Application()
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

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
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

		std::shared_ptr<VertexBuffer> t_VertexBuffer;
		std::shared_ptr<IndexBuffer> t_IndexBuffer;

		// Vertex Array
		t_VertexArray.reset(VertexArray::Create());

		float t_vertices[3 * 4] = {
			-0.5f , -0.5f , 0.0f,
			 0.5f , -0.5f , 0.0f,
			 0.5f ,  0.5f , 0.0f,
			-0.5f ,  0.5f , 0.0f,
		};

		// Vertex Buffer
		t_VertexBuffer.reset(VertexBuffer::Create(t_vertices, sizeof(t_vertices)));

		BufferLayout t_layout = {
			{ ShaderDataType::Float3 , "a_Position" }
		};
		t_VertexBuffer->SetLayout(t_layout);
		t_VertexArray->AddVertexBuffer(t_VertexBuffer);

		// Index Buffer
		uint32_t t_indices[6] = { 0 , 1 , 2 , 2 , 3 , 0 };
		t_IndexBuffer.reset(IndexBuffer::Create(t_indices, sizeof(t_indices) / sizeof(uint32_t)));
		t_VertexArray->SetIndexBuffer(t_IndexBuffer);

		// Shader
		std::string t_vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string t_fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;
			void main()
			{
				color = vec4(0.0f , 1.0f , 1.0f , 0.1f);
			}
		)";
		t_Shader.reset(new Shader(t_vertexSrc, t_fragmentSrc));
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

			glClearColor(0.143f, 0.143f, 0.143f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			t_Shader->Bind();
			t_VertexArray->Bind();
			glDrawElements(GL_TRIANGLES, t_VertexArray->GetIndexBuffers()->GetCount(), GL_UNSIGNED_INT, nullptr);

			m_Shader->Bind();
			m_VertexArray->Bind();
			glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffers()->GetCount(), GL_UNSIGNED_INT, nullptr);

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