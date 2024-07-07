#include "Framebuffer.h"
#include <glad/glad.h>
#include "Logging/Logging.h"

using namespace Engine;

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

std::shared_ptr<Texture> Framebuffer::GetColorAttachment(uint32_t index) const {
    if (index < _colorAttachments.size())
        return _colorAttachments[index].texture;
    return nullptr;
}

std::shared_ptr<Texture> Framebuffer::GetDepthAttachment() const {
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

            _colorAttachments[i].texture = std::make_shared<Texture>(textureSpec);
            _colorAttachments[i].format = _specification.attachments[i];

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _colorAttachments[i].texture->GetID(), 0);
        }
    }

    if (_specification.includeDepthStencil) {
        TextureSpec depthSpec;
        depthSpec.width = _specification.width;
        depthSpec.height = _specification.height;
        depthSpec.format = ImageFormat::D24S8;

        _depthAttachment.texture = std::make_shared<Texture>(depthSpec);
        _depthAttachment.format = ImageFormat::D24S8;

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _depthAttachment.texture->GetID(), 0);
    }

    if (_colorAttachments.size() > 1) {
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(_colorAttachments.size(), buffers);
    }
    else if (_colorAttachments.empty()) {
        // Only depth-pass
        glDrawBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ENGINE_ERROR("Failed to create frambuffer!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
