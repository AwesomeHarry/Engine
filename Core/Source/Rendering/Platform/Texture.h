#pragma once
#include <cstdint>
#include <string>
#include <variant>

#include "ImageFormat.h"

namespace Engine {
	struct TextureSpec {
		uint32_t width = 1;
		uint32_t height = 1;
		ImageFormat format = ImageFormat::RGBA8;
	};

	class Texture {
	public:
		Texture(const TextureSpec& spec);
		Texture(const std::string& path);
		~Texture();

		void SetData(void* data, uint32_t size);
		void Bind(uint32_t slot);
		void Unbind();

		inline uint32_t GetID() const { return _id; }
		inline uint32_t GetWidth() const { return _width; }
		inline uint32_t GetHeight() const { return _height; }
		inline uint32_t GetInternalFormat() const { return _internalFormat; }
		inline uint32_t GetDataFormat() const { return _dataFormat; }
		inline uint32_t GetDataType() const { return _dataType; }
	private:
		void initTexture(void* data);
	private:
		uint32_t _id;
		uint32_t _width, _height;
		uint32_t _internalFormat, _dataFormat, _dataType;
	};
}