#include "ImageFormat.h"
#include <glad/glad.h>
#include "Logging/Logging.h"

using namespace Engine;

uint32_t Utils::ImageFormatToOpenGLDataFormat(ImageFormat format) {
	switch (format) {
	case ImageFormat::R8:
	case ImageFormat::R16F:
	case ImageFormat::R32F:
	case ImageFormat::R8I:
	case ImageFormat::R16I:
	case ImageFormat::R32I:
	case ImageFormat::R8UI:
	case ImageFormat::R16UI:
	case ImageFormat::R32UI:
		return GL_RED;
	case ImageFormat::RG8:
	case ImageFormat::RG16F:
	case ImageFormat::RG32F:
	case ImageFormat::RG8I:
	case ImageFormat::RG16I:
	case ImageFormat::RG32I:
	case ImageFormat::RG8UI:
	case ImageFormat::RG16UI:
	case ImageFormat::RG32UI:
		return GL_RG;
	case ImageFormat::RGB8:
	case ImageFormat::RGB16F:
	case ImageFormat::RGB32F:
	case ImageFormat::RGB8I:
	case ImageFormat::RGB16I:
	case ImageFormat::RGB32I:
	case ImageFormat::RGB8UI:
	case ImageFormat::RGB16UI:
	case ImageFormat::RGB32UI:
		return GL_RGB;
	case ImageFormat::RGBA8:
	case ImageFormat::RGBA16F:
	case ImageFormat::RGBA32F:
	case ImageFormat::RGBA8I:
	case ImageFormat::RGBA16I:
	case ImageFormat::RGBA32I:
	case ImageFormat::RGBA8UI:
	case ImageFormat::RGBA16UI:
	case ImageFormat::RGBA32UI:
		return GL_RGBA;
	case ImageFormat::D16:
	case ImageFormat::D24:
	case ImageFormat::D32F:
		return GL_DEPTH_COMPONENT;
	case ImageFormat::S8:
		return GL_STENCIL_INDEX;
	case ImageFormat::D24S8:
	case ImageFormat::D32FS8:
		return GL_DEPTH_STENCIL;
	}
	ENGINE_ASSERT(false, "[ImageFormatToOpenGLDataFormat] Invalid image format!");
	return 0;
}

uint32_t Utils::ImageFormatToOpenGLInternalFormat(ImageFormat format) {
	switch (format) {
	case ImageFormat::R8:       return GL_R8;
	case ImageFormat::RG8:      return GL_RG8;
	case ImageFormat::RGB8:     return GL_RGB8;
	case ImageFormat::RGBA8:    return GL_RGBA8;
	case ImageFormat::R16F:     return GL_R16F;
	case ImageFormat::RG16F:    return GL_RG16F;
	case ImageFormat::RGB16F:   return GL_RGB16F;
	case ImageFormat::RGBA16F:  return GL_RGBA16F;
	case ImageFormat::R32F:     return GL_R32F;
	case ImageFormat::RG32F:    return GL_RG32F;
	case ImageFormat::RGB32F:   return GL_RGB32F;
	case ImageFormat::RGBA32F:  return GL_RGBA32F;
	case ImageFormat::R8I:      return GL_R8I;
	case ImageFormat::RG8I:     return GL_RG8I;
	case ImageFormat::RGB8I:    return GL_RGB8I;
	case ImageFormat::RGBA8I:   return GL_RGBA8I;
	case ImageFormat::R16I:     return GL_R16I;
	case ImageFormat::RG16I:    return GL_RG16I;
	case ImageFormat::RGB16I:   return GL_RGB16I;
	case ImageFormat::RGBA16I:  return GL_RGBA16I;
	case ImageFormat::R32I:     return GL_R32I;
	case ImageFormat::RG32I:    return GL_RG32I;
	case ImageFormat::RGB32I:   return GL_RGB32I;
	case ImageFormat::RGBA32I:  return GL_RGBA32I;
	case ImageFormat::R8UI:     return GL_R8UI;
	case ImageFormat::RG8UI:    return GL_RG8UI;
	case ImageFormat::RGB8UI:   return GL_RGB8UI;
	case ImageFormat::RGBA8UI:  return GL_RGBA8UI;
	case ImageFormat::R16UI:    return GL_R16UI;
	case ImageFormat::RG16UI:   return GL_RG16UI;
	case ImageFormat::RGB16UI:  return GL_RGB16UI;
	case ImageFormat::RGBA16UI: return GL_RGBA16UI;
	case ImageFormat::R32UI:    return GL_R32UI;
	case ImageFormat::RG32UI:   return GL_RG32UI;
	case ImageFormat::RGB32UI:  return GL_RGB32UI;
	case ImageFormat::RGBA32UI: return GL_RGBA32UI;
	case ImageFormat::D16:      return GL_DEPTH_COMPONENT16;
	case ImageFormat::D24:      return GL_DEPTH_COMPONENT24;
	case ImageFormat::D32F:     return GL_DEPTH_COMPONENT32F;
	case ImageFormat::S8:		return GL_STENCIL_INDEX8;
	case ImageFormat::D24S8:    return GL_DEPTH24_STENCIL8;
	case ImageFormat::D32FS8:   return GL_DEPTH32F_STENCIL8;
	}
	ENGINE_ASSERT(false, "[ImageFormatToOpenGLInternalFormat] Invalid image format!");
	return 0;
}

uint32_t Utils::ImageFormatToOpenGLDataType(ImageFormat format) {
	switch (format) {
	case ImageFormat::R8:
	case ImageFormat::RG8:
	case ImageFormat::RGB8:
	case ImageFormat::RGBA8:
		return GL_UNSIGNED_BYTE;
	case ImageFormat::R16F:
	case ImageFormat::RG16F:
	case ImageFormat::RGB16F:
	case ImageFormat::RGBA16F:
	case ImageFormat::R32F:
	case ImageFormat::RG32F:
	case ImageFormat::RGB32F:
	case ImageFormat::RGBA32F:
	case ImageFormat::D32F:
		return GL_FLOAT;
	case ImageFormat::R8I:
	case ImageFormat::RG8I:
	case ImageFormat::RGB8I:
	case ImageFormat::RGBA8I:
		return GL_BYTE;
	case ImageFormat::R16I:
	case ImageFormat::RG16I:
	case ImageFormat::RGB16I:
	case ImageFormat::RGBA16I:
		return GL_SHORT;
	case ImageFormat::R32I:
	case ImageFormat::RG32I:
	case ImageFormat::RGB32I:
	case ImageFormat::RGBA32I:
		return GL_INT;
	case ImageFormat::R8UI:
	case ImageFormat::RG8UI:
	case ImageFormat::RGB8UI:
	case ImageFormat::RGBA8UI:
	case ImageFormat::S8:
		return GL_UNSIGNED_BYTE;
	case ImageFormat::R16UI:
	case ImageFormat::RG16UI:
	case ImageFormat::RGB16UI:
	case ImageFormat::RGBA16UI:
		return GL_UNSIGNED_SHORT;
	case ImageFormat::R32UI:
	case ImageFormat::RG32UI:
	case ImageFormat::RGB32UI:
	case ImageFormat::RGBA32UI:
		return GL_UNSIGNED_INT;
	case ImageFormat::D16:
		return GL_UNSIGNED_SHORT;
	case ImageFormat::D24:
		return GL_UNSIGNED_INT;
	case ImageFormat::D24S8:
		return GL_UNSIGNED_INT_24_8;
	case ImageFormat::D32FS8:
		return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
	}
	ENGINE_ASSERT(false, "[ImageFormatToOpenGLDataType] Invalid image format!");
	return 0;
}
