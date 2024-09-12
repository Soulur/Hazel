#include "hzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();
		s_Data->QuadVertexArray = VertexArray::Create();

		float m_Bluevertices[5  * 4] = {
				-0.5f , -0.5f , 0.0f, 0.0f , 0.0f ,
				 0.5f , -0.5f , 0.0f, 1.0f , 0.0f ,
				 0.5f ,  0.5f , 0.0f, 1.0f , 1.0f ,
				-0.5f ,  0.5f , 0.0f, 0.0f , 1.0f 
		};

		// Vertex Buffer
		Ref<VertexBuffer> m_BlueVertexBuffer;
		m_BlueVertexBuffer.reset(VertexBuffer::Create(m_Bluevertices, sizeof(m_Bluevertices)));

		BufferLayout m_Bluelayout = {
			{ ShaderDataType::Float3 , "a_Position" },
			{ ShaderDataType::Float2 , "a_TexCoord" }
		};
		m_BlueVertexBuffer->SetLayout(m_Bluelayout);
		s_Data->QuadVertexArray->AddVertexBuffer(m_BlueVertexBuffer);

		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData , sizeof (uint32_t));

		// Index Buffer
		Ref<IndexBuffer> m_BlueIndexBuffer;
		uint32_t m_Blueindices[6] = { 0 , 1 , 2 , 2 , 3 , 0 };
		m_BlueIndexBuffer.reset(IndexBuffer::Create(m_Blueindices, sizeof(m_Blueindices) / sizeof(uint32_t)));
		s_Data->QuadVertexArray->SetIndexBuffer(m_BlueIndexBuffer);

		// Shader
		s_Data->TextureShader   = Shader::Create("assets/shaders/Texture.glsl");

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::EndScene()
	{
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x , position.y , 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		s_Data->TextureShader->SetFloat4("u_Color", color);
		s_Data->WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * /* rotation */
			glm::scale(glm::mat4(1.0f), { size.x , size.y , 1.0f });
		s_Data->TextureShader->SetMat4("u_Transform", transform); 

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQuad({ position.x , position.y , 0.0f }, size, texture);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		s_Data->TextureShader->SetFloat4("u_Color", glm::vec4(1.0f));

		texture->Bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x , size.y , 1.0f });
		 
		s_Data->TextureShader->SetMat4("u_Transform", transform);

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

}