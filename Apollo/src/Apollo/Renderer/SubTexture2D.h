#pragma once

#include "Apollo/Renderer/Texture.h"

#include <glm/glm.hpp>

#include <array>

namespace Apollo
{
	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		const Ref<Texture2D>& GetTexture() const { return m_Texture; }
		const glm::vec2* GetTexCoords() const { return m_TexCoords.data(); }

		static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = { 1.0f, 1.0f });
	private:
		Ref<Texture2D> m_Texture;
		std::array<glm::vec2, 4> m_TexCoords;
	};
}
