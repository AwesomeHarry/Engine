#include "BaseTexture.h"

#include <glad/glad.h>
#include "Logging/Logging.h"

using namespace Engine;

void BaseTexture::Bind(uint32_t slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	BindInternal();
}

void BaseTexture::Unbind() const {
	glBindTexture(_type, 0);
}

BaseTexture::BaseTexture(TextureType type, const TextureSpec& spec)
	: _type(TextureTypeToOpenGLTextureType(type)),
	_width(spec.width), _height(spec.height),
	_id(0),
	_internalFormat(0), _dataFormat(0), _dataType(0) {

	_internalFormat = Utils::ImageFormatToOpenGLInternalFormat(spec.format);
	_dataFormat = Utils::ImageFormatToOpenGLDataFormat(spec.format);
	_dataType = Utils::ImageFormatToOpenGLDataType(spec.format);

	glGenTextures(1, &_id);
	glBindTexture(_type, _id);

	if (type == TextureType::Tex2D)
		glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _width, _height, 0, _dataFormat, _dataType, nullptr);

	glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(_type, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

BaseTexture::~BaseTexture() {
	glDeleteTextures(1, &_id);
}

void BaseTexture::BindInternal() const {
	glBindTexture(_type, _id);
}

void BaseTexture::SetDataInternal(uint32_t target, const void* data) {
	glTexImage2D(target, 0, _internalFormat, _width, _height, 0, _dataFormat, _dataType, data);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		ENGINE_ERROR("OpenGL error when setting texture data");
	}
}

uint32_t BaseTexture::TextureTypeToOpenGLTextureType(TextureType type) {
	switch (type) {
	case TextureType::Tex2D: return GL_TEXTURE_2D;
	case TextureType::TexCubeMap: return GL_TEXTURE_CUBE_MAP;
	}

	ENGINE_ERROR("Unsupported texture type");
	return 0;
}

#include <stb_image.h>

namespace Engine::Texture::Utils {
	bool hasExtension(const std::string& filepath, const std::string& extension) {
		if (filepath.length() >= extension.length())
			return 0 == filepath.compare(filepath.length() - extension.length(), extension.length(), extension);
		return false;
	}

	FileTextureData LoadFromFile(const std::string& path) {
		FileTextureData data;
		int channels;
		if (hasExtension(path, ".hdr")) {
			float* fdata = stbi_loadf(path.c_str(), &data.width, &data.height, &channels, 0);
			if (fdata) {
				data.data = fdata;
				switch (channels) {
				case 3: data.format = ImageFormat::RGB16F; break;
				case 4: data.format = ImageFormat::RGBA16F; break;
				default:
					ENGINE_ERROR("Unsupported number of channels: {}", channels);
					stbi_image_free(fdata);
					break;
				}
				return data;
			}
		}
		else {
			unsigned char* udata = stbi_load(path.c_str(), &data.width, &data.height, &channels, 0);
			if (udata) {
				data.data = udata;
				switch (channels) {
				case 1: data.format = ImageFormat::R8; break;
				case 3: data.format = ImageFormat::RGB8; break;
				case 4: data.format = ImageFormat::RGBA8; break;
				default:
					ENGINE_ERROR("Unsupported number of channels: {}", channels);
					stbi_image_free(udata);
					break;
				}
				return data;
			}
		}
	}
}