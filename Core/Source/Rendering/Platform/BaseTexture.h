#pragma once
#include <cstdint>

#include "ImageFormat.h"

namespace Engine {
	enum class TextureType {
		Tex2D,
		TexCubeMap
	};

	struct TextureSpec {
		uint32_t width = 1;
		uint32_t height = 1;
		ImageFormat format = ImageFormat::RGBA8;
	};

	class BaseTexture {
	public:
		BaseTexture(TextureType type, const TextureSpec& spec);
		~BaseTexture();

		void Bind(uint32_t slot) const;
		void Unbind() const;

		inline uint32_t GetID() const { return _id; }
		inline uint32_t GetWidth() const { return _width; }
		inline uint32_t GetHeight() const { return _height; }
		inline ImageFormat GetFormat() const { return _format; }
	protected:
		void BindInternal() const;
		void SetDataInternal(uint32_t target, void* data);
		uint32_t TextureTypeToOpenGLTextureType(TextureType type);
	protected:
		uint32_t _id;
		uint32_t _type;
		uint32_t _width, _height;
		ImageFormat _format;
		uint32_t _internalFormat, _dataFormat, _dataType;
	};
}

#include <string>

namespace Engine::Texture::Utils {
	struct FileTextureData {
		int32_t width, height;
		ImageFormat format;
		void* data;
	};

	FileTextureData LoadFromFile(const std::string& path);
}