#pragma once
#include "Texture2D.h"
#include <vector>
#include <memory>

namespace Engine {
	enum class TextureTarget {
		Texture2D = 0,
		CubemapPosX = 1,
		CubemapNegX = 2,
		CubemapPosY = 3,
		CubemapNegY = 4,
		CubemapPosZ = 5,
		CubemapNegZ = 6
	};

	namespace Utils {
		int TextureTargetToOpenGLTarget(TextureTarget target);
	}

	class Framebuffer {
	public:
		struct Attachment {
			std::shared_ptr<Texture2D> texture;
			ImageFormat format = ImageFormat::RGB8;
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

		void ModifyColorAttachment(int index, TextureTarget target, BaseTexture& texture);
		void ModifyDepthAttachment(TextureTarget target, BaseTexture& texture);

		std::shared_ptr<Texture2D> GetColorAttachment(uint32_t index = 0) const;
		std::shared_ptr<Texture2D> GetDepthAttachment() const;

		const FramebufferSpec& GetSpecification() const { return _specification; }

	private:
		void Invalidate();

		uint32_t _id = 0;
		FramebufferSpec _specification;

		std::vector<Attachment> _colorAttachments;
		Attachment _depthAttachment;
	};

	enum class F_TextureAttachmentType {
		Color, Depth, Stencil
	};

	struct F_TextureAttachment {
		F_TextureAttachmentType type = F_TextureAttachmentType::Color;
		uint32_t bindingPoint = 0;
		uint32_t cubemapTarget = 0;
		std::shared_ptr<BaseTexture> texture;
		uint32_t mipLevel = 0;
	};

	class F_Buffer {
	public:
		F_Buffer(uint32_t width, uint32_t height);
		~F_Buffer();

		void Bind();
		void Unbind();

		void BindTexture(F_TextureAttachment textureAttachment);
		void BindRenderbuffer(ImageFormat format = ImageFormat::D24S8);

		void Resize(uint32_t newWidth, uint32_t newHeight, bool resizeAttachments = true);

		const F_TextureAttachment& GetTextureAttachment(uint32_t bindingPoint) { return _textureAttachments.at(bindingPoint); }
		const std::shared_ptr<BaseTexture>& GetDepthAttachment() { return _depthAttachment; }
		uint32_t GetTextureAttachmentCount() { return _textureAttachments.size(); }

		uint32_t GetWidth() { return _width; }
		uint32_t GetHeight() { return _height; }

		uint32_t GetInstanceID() { return _id; }
	private:
		void CheckStatus();
	private:
		uint32_t _id;
		uint32_t _width, _height;

		bool _useRenderBuffer = false;
		uint32_t _renderBufferID;
		ImageFormat _renderBufferFormat;

		std::unordered_map<uint32_t, F_TextureAttachment> _textureAttachments;
		std::shared_ptr<BaseTexture> _depthAttachment;
	};
}
