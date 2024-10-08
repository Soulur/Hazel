#include "hzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 2000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t Maxindices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;	// TODO: RenderCaps

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBuffeBase = nullptr;
		QuadVertex* QuadVertexBuffePtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;	// 0 = white texture

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats; 
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();

		s_Data.QuadVertexArray = VertexArray::Create();

		// Vertex Buffer
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout ({
			{ ShaderDataType::Float3 , "a_Position" },
			{ ShaderDataType::Float4 , "a_Color" },
			{ ShaderDataType::Float2 , "a_TexCoord" },
			{ ShaderDataType::Float  , "a_TexIndex" },
			{ ShaderDataType::Float  , "a_TilingFactor" }
		});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBuffeBase = new QuadVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.Maxindices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.Maxindices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.Maxindices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// White Shader
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData , sizeof (uint32_t));
		
		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; ++i)
			samplers[i] = i;

		// Shader
		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", samplers , s_Data.MaxTextureSlots);

		// Set all texture slots to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f , -0.5f , 0.0f , 1.0f };
		s_Data.QuadVertexPositions[1] = {  0.5f , -0.5f , 0.0f , 1.0f };
		s_Data.QuadVertexPositions[2] = {  0.5f ,  0.5f , 0.0f , 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f ,  0.5f , 0.0f , 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		HZ_PROFILE_FUNCTION();

		delete[] s_Data.QuadVertexBuffeBase;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBuffePtr = s_Data.QuadVertexBuffeBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBuffePtr - (uint8_t*)s_Data.QuadVertexBuffeBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBuffeBase , dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount == 0)
			return; // Nothing to draw

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
			s_Data.TextureSlots[i]->Bind(i);

		// Bind textures
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray , s_Data.QuadIndexCount);
		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBuffePtr = s_Data.QuadVertexBuffeBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x , position.y , 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= Renderer2DData::Maxindices)
			FlushAndReset();

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f; // TilingFactor

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x , size.y , 1.0f });

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { {0.0f , 0.0f} , {1.0f , 0.0f} , {1.0f , 1.0f} , {0.0f , 1.0f} };

		for (size_t i = 0; i < quadVertexCount; ++i)
		{
			s_Data.QuadVertexBuffePtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBuffePtr->Color = color;
			s_Data.QuadVertexBuffePtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBuffePtr->TexIndex = textureIndex;
			s_Data.QuadVertexBuffePtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBuffePtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x , position.y , 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= Renderer2DData::Maxindices)
			FlushAndReset();

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; ++i)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.QuadIndexCount >= Renderer2DData::Maxindices)
				FlushAndReset();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x , size.y , 1.0f });

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { {0.0f , 0.0f} , {1.0f , 0.0f} , {1.0f , 1.0f} , {0.0f , 1.0f} };

		for (size_t i = 0; i < quadVertexCount; ++i)
		{
			s_Data.QuadVertexBuffePtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBuffePtr->Color = tintColor;
			s_Data.QuadVertexBuffePtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBuffePtr->TexIndex = textureIndex;
			s_Data.QuadVertexBuffePtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBuffePtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	// DrawRotatedQuad /////////////////////////////////////////////////////////////////////////////////////////////////

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x , position.y , 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= Renderer2DData::Maxindices)
			FlushAndReset();

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f; // TilingFactor

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f , 0.0f , 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x , size.y , 1.0f });

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { {0.0f , 0.0f} , {1.0f , 0.0f} , {1.0f , 1.0f} , {0.0f , 1.0f} };

		for (size_t i = 0; i < quadVertexCount; ++i)
		{
			s_Data.QuadVertexBuffePtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBuffePtr->Color = color;
			s_Data.QuadVertexBuffePtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBuffePtr->TexIndex = textureIndex;
			s_Data.QuadVertexBuffePtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBuffePtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x , position.y , 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= Renderer2DData::Maxindices)
			FlushAndReset();

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; ++i)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.QuadIndexCount >= Renderer2DData::Maxindices)
				FlushAndReset();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f , 0.0f , 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x , size.y , 1.0f });

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { {0.0f , 0.0f} , {1.0f , 0.0f} , {1.0f , 1.0f} , {0.0f , 1.0f} };

		for (size_t i = 0; i < quadVertexCount; ++i)
		{
			s_Data.QuadVertexBuffePtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBuffePtr->Color = tintColor;
			s_Data.QuadVertexBuffePtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBuffePtr->TexIndex = textureIndex;
			s_Data.QuadVertexBuffePtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBuffePtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

}