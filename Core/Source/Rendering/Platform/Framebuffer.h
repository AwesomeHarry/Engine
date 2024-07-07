#pragma once
#include "Texture.h"
#include <vector>
#include <memory>

namespace Engine {
    class Framebuffer {
    public:
        struct Attachment {
            std::shared_ptr<Texture> texture;
            ImageFormat format;
        };

        struct FramebufferSpec {
            uint32_t width;
            uint32_t height;
            std::vector<ImageFormat> attachments;
            bool includeDepthStencil = true;
        };

        Framebuffer(const FramebufferSpec& spec);
        ~Framebuffer();

        void Bind();
        void Unbind();

        void Resize(uint32_t width, uint32_t height);

        std::shared_ptr<Texture> GetColorAttachment(uint32_t index = 0) const;
        std::shared_ptr<Texture> GetDepthAttachment() const;

        const FramebufferSpec& GetSpecification() const { return _specification; }

    private:
        void Invalidate();

        uint32_t _id = 0;
        FramebufferSpec _specification;

        std::vector<Attachment> _colorAttachments;
        Attachment _depthAttachment;
    };
}
