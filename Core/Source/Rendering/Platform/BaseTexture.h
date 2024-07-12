#pragma once
#include <cstdint>

#include "ImageFormat.h"

namespace Engine {
	enum class TextureType {
		Tex2D,
		TexCubeMap
	};

	struct TextureSpec {
		int32_t width = 1;
		int32_t height = 1;
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
		inline uint32_t GetInternalFormat() const { return _internalFormat; }
		inline uint32_t GetDataFormat() const { return _dataFormat; }
		inline uint32_t GetDataType() const { return _dataType; }
	protected:
		void BindInternal() const;
		void SetDataInternal(uint32_t target, const void* data);
		uint32_t TextureTypeToOpenGLTextureType(TextureType type);
	protected:
		uint32_t _id;
		uint32_t _type;
		uint32_t _width, _height;
		uint32_t _internalFormat, _dataFormat, _dataType;
	};
}