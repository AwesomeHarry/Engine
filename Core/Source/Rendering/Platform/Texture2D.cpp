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
	auto fileData = Texture::Utils::LoadFromFile(path);

	spec.width = fileData.width;
	spec.height = fileData.height;
	spec.format = fileData.format;

	auto texture = std::make_shared<Texture2D>(spec);
	texture->SetData(fileData.data);

	stbi_image_free(fileData.data);
	return texture;
}