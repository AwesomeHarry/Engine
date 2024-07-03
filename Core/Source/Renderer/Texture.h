#pragma once
#include <cstdint>
#include <string>
#include <variant>

namespace Engine {
	enum class ImageFormat {
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	struct TextureSpec {
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
	};

	class Texture {
	public:
		Texture(const TextureSpec& spec);
		Texture(const std::string& path);
		~Texture();

		void SetData(void* data, uint32_t size);
		void Bind(uint32_t slot);
		void Unbind();

		inline uint32_t GetWidth() const { return _width; }
		inline uint32_t GetHeight() const { return _height; }
	private:
		void initTexture(void* data);
	private:
		uint32_t _id;
		uint32_t _width, _height;
		uint32_t _internalFormat, _dataFormat, _dataType;
	};
}