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

void Framebuffer::ModifyColorAttachment(int index, TextureTarget target, BaseTexture& texture) {
	if (index > _colorAttachments.size() - 1) {
		ENGINE_WARN("[Framebuffer::ModifyColorAttachment] Failed to modify attachment, index outside valid range.")
			return;
	}

	Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, Utils::TextureTargetToOpenGLTarget(target), texture.GetInstanceID(), 0);
	Unbind();
}

void Framebuffer::ModifyDepthAttachment(TextureTarget target, BaseTexture& texture) {}

std::shared_ptr<Texture2D> Framebuffer::GetColorAttachment(uint32_t index) const {
	if (index < _colorAttachments.size())
		return _colorAttachments[index].texture;
	return nullptr;
}

std::shared_ptr<Texture2D> Framebuffer::GetDepthAttachment() const {
	return _depthAttachment.texture;
}

void Framebuffer::Invalidate() {
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

		for (uint32_t i = 0; i < _specification.attachments.size(); i++) {
			TextureSpec textureSpec;
			textureSpec.width = _specification.width;
			textureSpec.height = _specification.height;
			textureSpec.format = _specification.attachments[i];

			auto texture = std::make_shared<Texture2D>(textureSpec);
			_colorAttachments[i].texture = texture;
			_colorAttachments[i].format = _specification.attachments[i];

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _colorAttachments[i].texture->GetInstanceID(), 0);

			// Check if attachment was successful
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				ENGINE_ERROR("[Framebuffer::Invalidate] Failed to attach color buffer {}: {}", i, GetFramebufferStatusString(status));
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

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _depthAttachment.texture->GetInstanceID(), 0);

		// Check if depth-stencil attachment was successful
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			ENGINE_ERROR("[Framebuffer::Invalidate] Failed to attach depth-stencil buffer: {}", GetFramebufferStatusString(status));
			return;
		}
	}

	if (_colorAttachments.size() > 0) {
		GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers((uint32_t)_colorAttachments.size(), buffers);
	}
	else if (_colorAttachments.empty()) {
		// Only depth-pass
		glDrawBuffer(GL_NONE);
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		ENGINE_ERROR("[Framebuffer::Invalidate] Framebuffer is incomplete: {}", GetFramebufferStatusString(status));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int Engine::Utils::TextureTargetToOpenGLTarget(TextureTarget target) {
	switch (target) {
	case TextureTarget::Texture2D:
		return GL_TEXTURE_2D;
	case TextureTarget::CubemapPosX:
		return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	case TextureTarget::CubemapNegX:
		return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
	case TextureTarget::CubemapPosY:
		return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
	case TextureTarget::CubemapNegY:
		return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
	case TextureTarget::CubemapPosZ:
		return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
	case TextureTarget::CubemapNegZ:
		return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
	default:
		ENGINE_ERROR("[Framebuffer::TextureTargetToOpenGLTarget] Invalid texture target");
		return -1;
	}
}



F_Buffer::F_Buffer(uint32_t width, uint32_t height) :
	_width(width), _height(height), _id(0), _renderBufferID(0), _renderBufferFormat(ImageFormat::D24) {
	glGenFramebuffers(1, &_id);
}

F_Buffer::~F_Buffer() {
	glDeleteFramebuffers(1, &_id);
}

void F_Buffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	if (_useRenderBuffer)
		glBindRenderbuffer(GL_RENDERBUFFER, _renderBufferID);
	glViewport(0, 0, _width, _height);
}

void F_Buffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void F_Buffer::BindTexture(F_TextureAttachment textureAttachment) {
	Bind();

	if (textureAttachment.type == F_TextureAttachmentType::Color) {
		GLenum bindingPoint = GL_COLOR_ATTACHMENT0 + textureAttachment.bindingPoint;
		if (textureAttachment.texture->GetType() == TextureType::Tex2D)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + textureAttachment.bindingPoint, GL_TEXTURE_2D, textureAttachment.texture->GetInstanceID(), textureAttachment.mipLevel);
		else if (textureAttachment.texture->GetType() == TextureType::TexCubemap)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + textureAttachment.bindingPoint, GL_TEXTURE_CUBE_MAP_POSITIVE_X + textureAttachment.cubemapTarget, textureAttachment.texture->GetInstanceID(), textureAttachment.mipLevel);

		_textureAttachments[textureAttachment.bindingPoint] = textureAttachment;
	}
	else if (textureAttachment.type == F_TextureAttachmentType::Depth) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureAttachment.texture->GetInstanceID(), textureAttachment.mipLevel);

		_depthAttachment = textureAttachment.texture;
	}
	else if (textureAttachment.type == F_TextureAttachmentType::Stencil) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureAttachment.texture->GetInstanceID(), textureAttachment.mipLevel);
	}

	GLint textureWidth, textureHeight;
	glBindTexture(GL_TEXTURE_2D, textureAttachment.texture->GetInstanceID());
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureHeight);

	GLint renderbufferWidth, renderbufferHeight;
	glBindRenderbuffer(GL_RENDERBUFFER, _renderBufferID);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &renderbufferWidth);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &renderbufferHeight);

	CheckStatus();
	Unbind();
}

void F_Buffer::BindRenderbuffer(ImageFormat format) {
	Bind();

	if (!_useRenderBuffer) {
		glGenRenderbuffers(1, &_renderBufferID);
		_useRenderBuffer = true;
	}
	glBindRenderbuffer(GL_RENDERBUFFER, _renderBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, Utils::ImageFormatToOpenGLInternalFormat(format), _width, _height);

	GLenum attachmentType = 0;
	switch (format) {
	case ImageFormat::D16:
	case ImageFormat::D24:
	case ImageFormat::D32F:
		attachmentType = GL_DEPTH_ATTACHMENT;
		break;
	case ImageFormat::S8:
		attachmentType = GL_STENCIL_ATTACHMENT;
		break;
	case ImageFormat::D24S8:
	case ImageFormat::D32FS8:
		attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
		break;
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, _renderBufferID);

	CheckStatus();

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	Unbind();
}

void F_Buffer::Resize(uint32_t newWidth, uint32_t newHeight, bool resizeAttachments) {
	_width = newWidth;
	_height = newHeight;

	Bind();

	if (resizeAttachments) {
		for (auto& attachment : _textureAttachments) {
			// Resize textures
		}

		if (_useRenderBuffer) {
			glBindRenderbuffer(GL_RENDERBUFFER, _renderBufferID);
			glRenderbufferStorage(GL_RENDERBUFFER, Utils::ImageFormatToOpenGLInternalFormat(_renderBufferFormat), _width, _height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}

	CheckStatus();
	Unbind();
}

void F_Buffer::CheckStatus() {
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		ENGINE_ERROR("[Framebuffer::CheckStatus] {}", GetFramebufferStatusString(status));
		return;
	}
}
