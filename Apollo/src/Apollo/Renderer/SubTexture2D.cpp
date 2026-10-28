#include "pch.h"
#include "SubTexture2D.h"

namespace Apollo
{
	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
		: m_Texture(texture)
	{
		m_TexCoords[0] = { min.x, min.y };
		m_TexCoords[1] = { max.x, min.y };
		m_TexCoords[2] = { max.x, max.y };
		m_TexCoords[3] = { min.x, max.y };
	}

	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize)
	{
		float width = (float)texture->GetWidth();
		float height = (float)texture->GetHeight();
		float insetX = 0.5f / width;
		float insetY = 0.5f / height;

		glm::vec2 min = {
			(coords.x * cellSize.x) / width + insetX,
			(coords.y * cellSize.y) / height + insetY
		};
		glm::vec2 max = {
			((coords.x + spriteSize.x) * cellSize.x) / width - insetX,
			((coords.y + spriteSize.y) * cellSize.y) / height - insetY
		};

		return std::make_shared<SubTexture2D>(texture, min, max);
	}
}
