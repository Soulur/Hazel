#include "hzpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <gl/GL.h>

namespace Hazel {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_windowHandle(windowHandle)
	{
		HZ_CORE_ASSERT(m_windowHandle, "Window Handle is null!");
	}

	void OpenGLContext::Init()
	{
		HZ_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "Failed to initalize GLad!");

		HZ_CORE_INFO("OpenGL Info:");
		HZ_CORE_INFO("  Vendor  : {0}" , glGetString(GL_VENDOR));
		HZ_CORE_INFO("  Renderer: {0}" , glGetString(GL_RENDERER));
		HZ_CORE_INFO("  Version : {0}" , glGetString(GL_VERSION));

		HZ_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor>= 5), 
			"Hazel requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		HZ_PROFILE_FUNCTION();

		glfwSwapBuffers(m_windowHandle);
	}
}