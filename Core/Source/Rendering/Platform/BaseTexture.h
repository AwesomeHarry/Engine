#pragma once
#include <cstdint>

#include "ImageFormat.h"

namespace Engine {
	enum class TextureType {
		Tex2D,
		TexCubemap
	};

	enum class TextureWrap {
		ClampToEdge = 0x812F,
		ClampToBorder = 0x812D,
		MirroredRepeat = 0x8370,
		Repeat = 0x2901
	};

	enum class TextureFilter {
		Nearest = 0x2600,
		Linear = 0x2601,
		NearestMipmapNearest = 0x2700,
		LinearMipmapNearest = 0x2701,
		NearestMipmapLinear = 0x2702,
		LinearMipmapLinear = 0x2703
	};

	struct TextureSpec {
		uint32_t width = 1;
		uint32_t height = 1;
		ImageFormat format = ImageFormat::RGBA8;
		TextureWrap wrapS = TextureWrap::ClampToEdge;
		TextureWrap wrapT = TextureWrap::ClampToEdge;
		TextureWrap wrapR = TextureWrap::ClampToEdge;
		TextureFilter minFilter = TextureFilter::Linear;
		TextureFilter magFilter = TextureFilter::Linear;
	};

	class BaseTexture {
	public:
		BaseTexture(TextureType type, const TextureSpec& spec);
		~BaseTexture();

		void Bind(uint32_t slot) const;
		void Unbind() const;

		void GenerateMipmaps();

		inline uint32_t GetInstanceID() const { return _id; }
		inline TextureType GetType() const { return _type; }
		inline uint32_t GetWidth() const { return _width; }
		inline uint32_t GetHeight() const { return _height; }
		inline ImageFormat GetFormat() const { return _format; }
	protected:
		void BindInternal() const;
		void SetDataInternal(uint32_t target, void* data);
	protected:
		uint32_t _id;
		TextureType _type;
		ImageFormat _format;

		TextureWrap _wrapS, _wrapT, _wrapR;
		TextureFilter _minFilter, _magFilter;

		uint32_t _width, _height;
		uint32_t _internalType;
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