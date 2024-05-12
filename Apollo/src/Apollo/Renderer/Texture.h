#pragma once

namespace Apollo
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Bind() = 0;
	};

	class Texture2D : public Texture
	{
		static Ref<Texture2D> Create(const std::string& path);
	};
}
