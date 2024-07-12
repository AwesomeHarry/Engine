#include "Framebuffer.h"
#include <glad/glad.h>
#include "Logging/Logging.h"

using namespace Engine;

std::string GetFramebufferStatusString(GLenum status) {
    switch (status) {
    case GL_FRAMEBUFFER_COMPLETE:
        return "GL_FRAMEBUFFER_COMPLETE";
    case GL_FRAMEBUFFER_UNDEFINED:
        return "GL_FRAMEBUFFER_UNDEFINED";
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
    case GL_FRAMEBUFFER_UNSUPPORTED:
        return "GL_FRAMEBUFFER_UNSUPPORTED";
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
    default:
        return "Unknown error";
    }
}

Framebuffer::Framebuffer(const FramebufferSpec& spec)
    : _specification(spec) {
    Invalidate();
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &_id);
}

void Framebuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    glViewport(0, 0, _specification.width, _specification.height);
}

void Framebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0 || width > 8192 || height > 8192) {
        return;
    }
    _specification.width = width;
    _specification.height = height;

    Invalidate();
}

std::shared_ptr<Texture2D> Framebuffer::GetColorAttachment(uint32_t index) const {
    if (index < _colorAttachments.size())
        return _colorAttachments[index].texture;
    return nullptr;
}

std::shared_ptr<Texture2D> Framebuffer::GetDepthAttachment() const {
    return _depthAttachment.texture;
}

void Engine::Framebuffer::Invalidate() {
    if (_id) {
        glDeleteFramebuffers(1, &_id);
        _colorAttachments.clear();
        _depthAttachment = {};
    }

    glGenFramebuffers(1, &_id);
    glBindFramebuffer(GL_FRAMEBUFFER, _id);

    // Attachments
    if (_specification.attachments.size() > 0) {
        _colorAttachments.resize(_specification.attachments.size());

        for (size_t i = 0; i < _specification.attachments.size(); i++) {
            TextureSpec textureSpec;
            textureSpec.width = _specification.width;
            textureSpec.height = _specification.height;
            textureSpec.format = _specification.attachments[i];

            auto texture = std::make_shared<Texture2D>(textureSpec);
            _colorAttachments[i].texture = texture;
            _colorAttachments[i].format = _specification.attachments[i];

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _colorAttachments[i].texture->GetID(), 0);

            // Check if attachment was successful
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                ENGINE_ERROR("Failed to attach color buffer {}: {}", i, GetFramebufferStatusString(status));
                return;
            }
        }
    }

    if (_specification.includeDepthStencil) {
        TextureSpec depthSpec;
        depthSpec.width = _specification.width;
        depthSpec.height = _specification.height;
        depthSpec.format = ImageFormat::D24S8;

        _depthAttachment.texture = std::make_shared<Texture2D>(depthSpec);
        _depthAttachment.format = ImageFormat::D24S8;

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _depthAttachment.texture->GetID(), 0);

        // Check if depth-stencil attachment was successful
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            ENGINE_ERROR("Failed to attach depth-stencil buffer: {}", GetFramebufferStatusString(status));
            return;
        }
    }

    if (_colorAttachments.size() > 0) {
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(_colorAttachments.size(), buffers);
    }
    else if (_colorAttachments.empty()) {
        // Only depth-pass
        glDrawBuffer(GL_NONE);
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        ENGINE_ERROR("Framebuffer is incomplete: {}", GetFramebufferStatusString(status));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}