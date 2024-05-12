#pragma once

#include "Apollo/Renderer/Texture.h"

namespace Apollo
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(std::string path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void Bind() override;
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
	};
}