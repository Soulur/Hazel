#pragma once

#include "Hazel/Renderer/OrthoGraphicCamera.h"
#include "Hazel/Core/Timestep.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"

namespace Hazel {

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio , bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		OrthographicCammera& GetCamera() { return m_Camera; }
		const OrthographicCammera GetCamera() const { return m_Camera; }

		float GetZoomlevel() const { return m_ZoomLevel; }
		void SetZoomlevel(float level) { m_ZoomLevel = level; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent & e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCammera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 5.0f , m_CameraRotationSpeed = 180.0f;
	};
}