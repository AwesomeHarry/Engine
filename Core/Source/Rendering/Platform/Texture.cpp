#include "Texture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include "Logging/Logging.h"

using namespace Engine;

Texture::Texture(const TextureSpec& spec)
	: _width(spec.width), _height(spec.height), _id(0), _internalFormat(0), _dataFormat(0), _dataType(0) {

	_internalFormat = Utils::ImageFormatToOpenGLDataFormat(spec.format);
	_dataFormat = Utils::ImageFormatToOpenGLDataFormat(spec.format);
	_dataType = Utils::ImageFormatToOpenGLDataType(spec.format);

	initTexture(nullptr);
}

Texture::Texture(const std::string& path)
	: _id(0), _width(0), _height(0), _internalFormat(0), _dataFormat(0), _dataType(0) {

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data) {
		ENGINE_WARN("Failed to load texture from path: {}", path);
		return;
	}

	_width = width;
	_height = height;

	if (channels == 3) {
		_internalFormat = GL_RGB8;
		_dataFormat = GL_RGB;
		_dataType = GL_UNSIGNED_BYTE;
	}
	else if (channels == 4) {
		_internalFormat = GL_RGBA8;
		_dataFormat = GL_RGBA;
		_dataType = GL_UNSIGNED_BYTE;
	}
	else {
		ENGINE_ERROR("Unsupported number of channels: {}", channels);
		stbi_image_free(data);
		return;
	}

	initTexture(data);
	stbi_image_free(data);
}

Texture::~Texture() {
	glDeleteTextures(1, &_id);
}

void Texture::SetData(void* data, uint32_t size) {
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, _dataFormat, _dataType, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(uint32_t slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::initTexture(void* data) {
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _width, _height, 0, _dataFormat, _dataType, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}
