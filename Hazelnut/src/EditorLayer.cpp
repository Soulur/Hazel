#include "EditorLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel {


    EditorLayer::EditorLayer()
        : Layer("Sandbo2D"), m_CameraController( (float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight())
    {
    }

    void EditorLayer::OnAttach()
    {
        HZ_PROFILE_FUNCTION();

        m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");

        Hazel::FramebufferSpecification fbSpec;
        fbSpec.Width = Hazel::Application::Get().GetWindow().GetWidth();
        fbSpec.Height= Hazel::Application::Get().GetWindow().GetHeight();
        m_Framebuffer = Hazel::Framebuffer::Create(fbSpec);
    }

    void EditorLayer::OnDetach()
    {
        HZ_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Hazel::Timestep ts)
    {

        HZ_PROFILE_FUNCTION();

        // Resize 

        if (Hazel::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
        }


        // Update
        if (m_ViewportFocused)
            m_CameraController.OnUpdate(ts);

        // Renderer
        Hazel::Renderer2D::ResetStats();
        {
            HZ_PROFILE_SCOPE("Renderer Prep");
            m_Framebuffer->Bind();
            Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
            Hazel::RenderCommand::Clear();
        }

        static float rotation = 0.0f;
        rotation += ts * 2.0f;

        {
            HZ_PROFILE_SCOPE("Renderer Draw");
            Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
            Hazel::Renderer2D::DrawRotatedQuad({ 1.0f,0.0f }, { 1.0f , 0.5f }, -45.0f, { 0.1f , 0.2f , 0.3f , 1.0f });
            Hazel::Renderer2D::DrawQuad({ -1.0f,0.0f }, { 0.8f , 0.8f }, { m_SqueraColor });
            Hazel::Renderer2D::DrawQuad({ 0.0f,-0.5f }, { 0.5f , 0.5f }, { 0.8f , 0.2f , 0.3f , 1.0f });
            Hazel::Renderer2D::DrawQuad({ 0.0f , 0.0f , -0.1f }, { 20.0f , 20.0f }, m_CheckerboardTexture, 10.0f);
            Hazel::Renderer2D::DrawRotatedQuad({ -1.0f,-1.0f }, { 1.0f , 1.0f }, rotation, m_CheckerboardTexture, 10.0f);
            Hazel::Renderer2D::EndScene();

            Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
            for (float y = -5.0f; y < 5.0f; y += 0.6f)
            {
                for (float x = -5.0f; x < 5.0f; x += 0.5f)
                {
                    glm::vec4 color = { (x + 5.0f) / 10.f , (y + 5.0f) / 10.0f , (x + y + 6.0f) / 10.0f , 0.7f };
                    Hazel::Renderer2D::DrawQuad({ x , y }, { 0.45f , 0.45f }, color);
                }
            }
            Hazel::Renderer2D::EndScene();
            m_Framebuffer->Unbind();
        }
    }

    void EditorLayer::OnImGuiRender()
    {
        static bool dockspaceOpen = true;

        static bool opt_fullscreen_persistant = true;
        static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
        bool opt_fullscreen = opt_fullscreen_persistant;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Dockspace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit")) Hazel::Application::Get().Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Settings
        {
            ImGui::Begin("Settings");

            auto stats = Hazel::Renderer2D::GetStats();
            ImGui::Text("Renderer2D Stats: ");
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Quad Count: %d", stats.QuadCount);
            ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
            ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

            ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SqueraColor));

            ImGui::End();
        }

        // Viewport
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0 , 0 });
            ImGui::Begin("Viewport");

            m_ViewportFocused = ImGui::IsWindowFocused();
            m_ViewportHovered = ImGui::IsWindowHovered();
            Application::Get().GetImGuiLayer()->BlockEvent(!m_ViewportFocused || !m_ViewportHovered);

            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

            uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
            ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x , m_ViewportSize.y }, ImVec2{ 0 , 1 }, ImVec2{ 1 , 0 });
            ImGui::PopStyleVar();
            ImGui::End();
        }

        ImGui::End();

    }

    void EditorLayer::OnEvent(Hazel::Event& e)
    {
        m_CameraController.OnEvent(e);
    }
}