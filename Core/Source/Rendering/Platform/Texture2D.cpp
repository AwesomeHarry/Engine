#include "Texture2D.h"
#include <glad/glad.h>
#include <stb_image.h>
#include "Logging/Logging.h"

using namespace Engine;

void Texture2D::SetData(const void* data) {
	BindInternal();
	SetDataInternal(GL_TEXTURE_2D, data);
	Unbind();
}

std::shared_ptr<Texture2D> Texture2D::Utils::FromFile(const std::string& path) {
	TextureSpec spec;

	stbi_set_flip_vertically_on_load(true);
	int channels;
	unsigned char* data = stbi_load(path.c_str(), &spec.width, &spec.height, &channels, 0);
	if (!data) {
		ENGINE_WARN("Failed to load texture from path: {}", path);
		return nullptr;
	}

	if (channels == 3)	spec.format = ImageFormat::RGB8;
	else if (channels == 4) spec.format = ImageFormat::RGBA8;
	else {
		ENGINE_ERROR("Unsupported number of channels: {}", channels);
		stbi_image_free(data);
		return nullptr;
	}

	auto texture = std::make_shared<Texture2D>(spec);
	texture->SetData(data);

	stbi_image_free(data);
	return texture;
}