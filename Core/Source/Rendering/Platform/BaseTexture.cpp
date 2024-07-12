#include "BaseTexture.h"

#include <glad/glad.h>
#include "Logging/Logging.h"

using namespace Engine;

void BaseTexture::Bind(uint32_t slot) {
	Bind();
	glActiveTexture(GL_TEXTURE0 + slot);
}

void BaseTexture::Unbind() {
	glBindTexture(_type, 0);
}

BaseTexture::BaseTexture(TextureType type, const TextureSpec& spec)
	: _type(TextureTypeToOpenGLTextureType(type)),
	_width(spec.width), _height(spec.height),
	_id(0),
	_internalFormat(0), _dataFormat(0), _dataType(0) {

	_internalFormat = Utils::ImageFormatToOpenGLDataFormat(spec.format);
	_dataFormat = Utils::ImageFormatToOpenGLDataFormat(spec.format);
	_dataType = Utils::ImageFormatToOpenGLDataType(spec.format);

	glGenTextures(1, &_id);
	glBindTexture(_type, _id);

	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _width, _height, 0, _dataFormat, _dataType, nullptr);

	glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

BaseTexture::~BaseTexture() {
	glDeleteTextures(1, &_id);
}

void BaseTexture::Bind() {
	glBindTexture(_type, _id);
}

void BaseTexture::SetDataInternal(uint32_t target, const void* data) {
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, _dataFormat, _dataType, data);
}


uint32_t BaseTexture::TextureTypeToOpenGLTextureType(TextureType type) {
	switch (type) {
	case TextureType::Tex2D: return GL_TEXTURE_2D;
	case TextureType::TexCubeMap: return GL_TEXTURE_CUBE_MAP;
	}

	ENGINE_ERROR("Unsupported texture type");
	return 0;
}