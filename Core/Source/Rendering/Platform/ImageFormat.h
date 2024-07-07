#pragma once
#include <cstdint>

namespace Engine {
    enum class ImageFormat {
        None = 0,
        // Color formats
        R8,
        RG8,
        RGB8,
        RGBA8,
        R16F,
        RG16F,
        RGB16F,
        RGBA16F,
        R32F,
        RG32F,
        RGB32F,
        RGBA32F,
        // Integer formats
        R8I,
        RG8I,
        RGB8I,
        RGBA8I,
        R16I,
        RG16I,
        RGB16I,
        RGBA16I,
        R32I,
        RG32I,
        RGB32I,
        RGBA32I,
        // Unsigned integer formats
        R8UI,
        RG8UI,
        RGB8UI,
        RGBA8UI,
        R16UI,
        RG16UI,
        RGB16UI,
        RGBA16UI,
        R32UI,
        RG32UI,
        RGB32UI,
        RGBA32UI,
        // Depth/stencil formats
        D16,
        D24,
        D32F,
        D24S8,
        D32FS8,
        // Compressed formats (if needed)
        // DXT1,
        // DXT3,
        // DXT5,
        // etc.
    };

    namespace Utils {
        uint32_t ImageFormatToOpenGLDataFormat(ImageFormat format);

        uint32_t ImageFormatToOpenGLInternalFormat(ImageFormat format);

        uint32_t ImageFormatToOpenGLDataType(ImageFormat format);
    }
}