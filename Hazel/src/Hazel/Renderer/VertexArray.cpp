#include "hzpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h" 

namespace Hazel {

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::Node:	HZ_CORE_ASSERT(false, "RendererAPI::Node is currently not suported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return new OpenGLVertexArray();
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}