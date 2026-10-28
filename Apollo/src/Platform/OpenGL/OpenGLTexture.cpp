#include "pch.h"

#include "OpenGLTexture.h"

#include "glad/glad.h"

#include <cstring>
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

namespace Apollo
{
	namespace
	{
		struct ComRelease
		{
			void operator()(IUnknown* pointer) const
			{
				if (pointer)
					pointer->Release();
			}
		};

		template<typename T>
		using ComPtr = std::unique_ptr<T, ComRelease>;

		struct ComApartment
		{
			bool Uninit = false;

			ComApartment()
			{
				HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
				Uninit = SUCCEEDED(result);
			}

			~ComApartment()
			{
				if (Uninit)
					CoUninitialize();
			}
		};

		std::wstring ToWide(const std::string& path)
		{
			if (path.empty())
				return {};

			int size = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), (int)path.size(), nullptr, 0);
			if (size <= 0)
				return {};

			std::wstring wide(size, L'\0');
			MultiByteToWideChar(CP_UTF8, 0, path.c_str(), (int)path.size(), wide.data(), size);
			return wide;
		}

		// WIC 读出的第一行是图像顶部，OpenGL 贴图第一行是底部，这里翻转。
		void FlipVertically(std::vector<uint8_t>& pixels, uint32_t width, uint32_t height)
		{
			uint32_t stride = width * 4;
			std::vector<uint8_t> flipped(pixels.size());
			for (uint32_t y = 0; y < height; y++)
			{
				std::memcpy(flipped.data() + (size_t)(height - 1 - y) * stride,
					pixels.data() + (size_t)y * stride,
					stride);
			}
			pixels.swap(flipped);
		}

		bool LoadImageRGBA(const std::string& path, std::vector<uint8_t>& pixels, uint32_t& width, uint32_t& height)
		{
			ComApartment apartment;
			std::wstring widePath = ToWide(path);
			if (widePath.empty())
				return false;

			IWICImagingFactory* factoryRaw = nullptr;
			HRESULT result = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factoryRaw));
			if (FAILED(result))
				return false;
			ComPtr<IWICImagingFactory> factory(factoryRaw);

			IWICBitmapDecoder* decoderRaw = nullptr;
			result = factory->CreateDecoderFromFilename(widePath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoderRaw);
			if (FAILED(result))
				return false;
			ComPtr<IWICBitmapDecoder> decoder(decoderRaw);

			IWICBitmapFrameDecode* frameRaw = nullptr;
			result = decoder->GetFrame(0, &frameRaw);
			if (FAILED(result))
				return false;
			ComPtr<IWICBitmapFrameDecode> frame(frameRaw);

			IWICFormatConverter* converterRaw = nullptr;
			result = factory->CreateFormatConverter(&converterRaw);
			if (FAILED(result))
				return false;
			ComPtr<IWICFormatConverter> converter(converterRaw);

			result = converter->Initialize(frame.get(), GUID_WICPixelFormat32bppRGBA,
				WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
			if (FAILED(result))
				return false;

			UINT imageWidth = 0;
			UINT imageHeight = 0;
			converter->GetSize(&imageWidth, &imageHeight);
			if (imageWidth == 0 || imageHeight == 0)
				return false;

			width = imageWidth;
			height = imageHeight;
			pixels.resize((size_t)width * height * 4);
			result = converter->CopyPixels(nullptr, width * 4, (UINT)pixels.size(), pixels.data());
			if (FAILED(result))
				return false;

			FlipVertically(pixels, width, height);
			return true;
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;
		CreateStorage();
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		std::vector<uint8_t> pixels;
		if (!LoadImageRGBA(path, pixels, m_Width, m_Height))
		{
			APOLLO_CORE_ERROR("Failed to load texture: {0}", path);
			APOLLO_CORE_ASSERT(false, "Failed to load texture");
			return;
		}

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;
		CreateStorage();
		SetData(pixels.data(), (uint32_t)pixels.size());
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		if (m_RendererID)
			glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::CreateStorage()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bytesPerPixel = m_DataFormat == GL_RGBA ? 4 : 3;
		APOLLO_CORE_ASSERT(size == m_Width * m_Height * bytesPerPixel, "Texture data must cover the full image");

		GLint previousAlignment = 4;
		glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousAlignment);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
		glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}
