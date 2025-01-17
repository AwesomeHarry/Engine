#include "BaseTexture.h"

#include <glad/glad.h>
#include "Logging/Logging.h"

using namespace Engine;

uint32_t TextureTypeToOpenGLTextureType(TextureType type) {
	switch (type) {
	case TextureType::Tex2D: return GL_TEXTURE_2D;
	case TextureType::TexCubemap: return GL_TEXTURE_CUBE_MAP;
	}

	ENGINE_ERROR("[TextureTypeToOpenGLTextureType] Unsupported texture type");
	return 0;
}

TextureType OpenGLTextureTypeToTextureType(uint32_t type) {
	switch (type) {
	case GL_TEXTURE_2D: return TextureType::Tex2D;
	case GL_TEXTURE_CUBE_MAP: return TextureType::TexCubemap;
	}

	ENGINE_ERROR("[OpenGLTextureTypeToTextureType] Unsupported OpenGL texture type");
	return TextureType::Tex2D;
}

BaseTexture::BaseTexture(TextureType type, const TextureSpec& spec)
	: _format(spec.format), _type(type), _internalType(TextureTypeToOpenGLTextureType(type)),
	_width(spec.width), _height(spec.height),
	_id(0),
	_internalFormat(0), _dataFormat(0), _dataType(0) {

	_internalFormat = Utils::ImageFormatToOpenGLInternalFormat(spec.format);
	_dataFormat = Utils::ImageFormatToOpenGLDataFormat(spec.format);
	_dataType = Utils::ImageFormatToOpenGLDataType(spec.format);

	glGenTextures(1, &_id);
}

BaseTexture::~BaseTexture() {
	glDeleteTextures(1, &_id);
}

void BaseTexture::Bind(uint32_t slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	BindInternal();
}

void BaseTexture::Unbind() const {
	glBindTexture(_internalType, 0);
}

void BaseTexture::BindInternal() const {
	glBindTexture(_internalType, _id);
}

void BaseTexture::SetDataInternal(uint32_t target, void* data) {
	glTexImage2D(target, 0, _internalFormat, _width, _height, 0, _dataFormat, _dataType, data);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		ENGINE_ERROR("[BaseTexture::SetDataInternal] Error setting texture data: {}", error);
	}
}

#include <stb_image.h>
#include "Util/FileIO.h"

namespace Engine::Texture::Utils {
	FileTextureData LoadFromFile(const std::string& path) {
		FileTextureData data;
		int channels;
		if (FileIO::HasExtension(path, ".hdr")) {
			float* fdata = stbi_loadf(path.c_str(), &data.width, &data.height, &channels, 0);
			if (fdata) {
				data.data = fdata;
				switch (channels) {
				case 3: data.format = ImageFormat::RGB16F; break;
				case 4: data.format = ImageFormat::RGBA16F; break;
				default:
					ENGINE_ERROR("Texture::LoadFromFile] Unsupported number of channels: {}", channels);
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
					ENGINE_ERROR("[Texture::LoadFromFile] Unsupported number of channels: {}", channels);
					stbi_image_free(udata);
					break;
				}
				return data;
			}
		}

		return data;
	}
}