#pragma once

#include "Base.h"

#include "Window.h"
#include "Hazel/Core/LayerStack.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"

#include "Hazel/Core/Timestep.h"

#include "Hazel/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Hazel {

	class Application
	{
	public:
		Application(const std::string& name = "Hazel App");
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		Window& GetWindow() { return *m_Window; }

		static Application& Get() { return *s_Instace; }
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		Timestep m_Timestep;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instace;
		friend int::main(int argc, char** argv);
	};

	// To be defined in CLIENT
	Application* CreatApplication();

}

