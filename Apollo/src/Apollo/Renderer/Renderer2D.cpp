#include "pch.h"
#include "Renderer2D.h"

#include "Apollo/Renderer/RenderCommand.h"
#include "Apollo/Renderer/Shader.h"
#include "Apollo/Renderer/VertexArray.h"
#include "Apollo/Renderer/Buffer.h"

#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace Apollo
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 16;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		bool FlushEveryQuad = false;
		Renderer2D::Statistics Stats;
	};

	static Renderer2DData* s_Data = nullptr;

	static std::string BuildTextureFragmentShader()
	{
		// 用常量下标采样。GLSL 330 不允许用变量直接索引 sampler 数组，
		// 这样写在目前的 OpenGL 上下文里能通过编译。
		std::string source = R"(
			#version 330 core
			layout(location = 0) out vec4 color;

			in vec4 v_Color;
			in vec2 v_TexCoord;
			in float v_TexIndex;

			uniform sampler2D u_Textures[16];

			vec4 SampleTexture(int index)
			{
		)";

		for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
		{
			source += "\t\t\tif (index == " + std::to_string(i) + ") return texture(u_Textures[" + std::to_string(i) + "], v_TexCoord);\n";
		}

		source += R"(
				return vec4(1.0, 0.0, 1.0, 1.0);
			}

			void main()
			{
				color = SampleTexture(int(v_TexIndex + 0.5)) * v_Color;
			}
		)";
		return source;
	}

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DData();
		s_Data->QuadVertexArray.reset(VertexArray::Create());

		s_Data->QuadVertexBuffer.reset(VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex)));
		s_Data->QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" }
		});
		s_Data->QuadVertexArray->AddVertexBuffer(s_Data->QuadVertexBuffer);

		s_Data->QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxVertices];

		uint32_t* indices = new uint32_t[Renderer2DData::MaxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < Renderer2DData::MaxIndices; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;

			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, Renderer2DData::MaxIndices));
		s_Data->QuadVertexArray->SetIndexBuffer(indexBuffer);
		delete[] indices;

		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whitePixel = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whitePixel, sizeof(uint32_t));
		s_Data->TextureSlots[0] = s_Data->WhiteTexture;

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			layout(location = 2) in vec2 a_TexCoord;
			layout(location = 3) in float a_TexIndex;

			uniform mat4 u_ViewProjection;

			out vec4 v_Color;
			out vec2 v_TexCoord;
			out float v_TexIndex;

			void main()
			{
				v_Color = a_Color;
				v_TexCoord = a_TexCoord;
				v_TexIndex = a_TexIndex;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		s_Data->TextureShader.reset(Shader::Create(vertexSrc, BuildTextureFragmentShader()));

		int samplers[Renderer2DData::MaxTextureSlots];
		for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
			samplers[i] = (int)i;

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetIntArray("u_Textures", samplers, Renderer2DData::MaxTextureSlots);
	}

	void Renderer2D::Shutdown()
	{
		if (!s_Data)
			return;

		delete[] s_Data->QuadVertexBufferBase;
		delete s_Data;
		s_Data = nullptr;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data->Stats = {};
		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::SetFlushEveryQuad(bool enabled)
	{
		s_Data->FlushEveryQuad = enabled;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(glm::vec3(position, 0.0f), size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint)
	{
		DrawQuad(glm::vec3(position, 0.0f), size, texture, tint);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, texture, tint);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		DrawQuad(transform, s_Data->WhiteTexture, color);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tint)
	{
		constexpr glm::vec4 positions[] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f }
		};
		constexpr glm::vec2 texCoords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = -1.0f;
		for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
		{
			if (s_Data->TextureSlots[i].get() == texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex < 0.0f)
		{
			if (s_Data->TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
			s_Data->TextureSlotIndex++;
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = glm::vec3(transform * positions[i]);
			s_Data->QuadVertexBufferPtr->Color = tint;
			s_Data->QuadVertexBufferPtr->TexCoord = texCoords[i];
			s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data->QuadVertexBufferPtr++;
		}

		s_Data->QuadIndexCount += 6;
		s_Data->Stats.QuadCount++;

		if (s_Data->FlushEveryQuad)
			NextBatch();
	}

	void Renderer2D::ResetStats()
	{
		s_Data->Stats = {};
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data->Stats;
	}

	void Renderer2D::StartBatch()
	{
		s_Data->QuadIndexCount = 0;
		s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;
		s_Data->TextureSlotIndex = 1;
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::Flush()
	{
		if (s_Data->QuadIndexCount == 0)
			return;

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->QuadVertexBufferPtr - (uint8_t*)s_Data->QuadVertexBufferBase);
		s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertexBufferBase, dataSize);

		for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
			s_Data->TextureSlots[i]->Bind(i);

		RenderCommand::DrawIndexed(s_Data->QuadVertexArray, s_Data->QuadIndexCount);
		s_Data->Stats.DrawCalls++;
		if (s_Data->TextureSlotIndex > s_Data->Stats.TextureCount)
			s_Data->Stats.TextureCount = s_Data->TextureSlotIndex;
	}
}
