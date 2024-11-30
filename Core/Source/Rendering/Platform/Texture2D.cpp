#include "Texture2D.h"
#include <glad/glad.h>
#include <stb_image.h>
#include "Logging/Logging.h"

using namespace Engine;

Texture2D::Texture2D(const TextureSpec& spec)
	: BaseTexture(TextureType::Tex2D, spec) {
	BindInternal();

	//SetDataInternal(GL_TEXTURE_2D, nullptr);
	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _width, _height, 0, _dataFormat, _dataType, nullptr);
}

void Texture2D::SetData(void* data) {
	BindInternal();
	SetDataInternal(GL_TEXTURE_2D, data);
	Unbind();
}

std::shared_ptr<Texture2D> Texture2D::Utils::FromFile(const std::string& path, bool flipV) {
	stbi_set_flip_vertically_on_load(flipV);
	auto fileData = Texture::Utils::LoadFromFile(path);

	TextureSpec spec;
	spec.width = fileData.width;
	spec.height = fileData.height;
	spec.format = fileData.format;

	//auto texture = std::make_shared<Texture2D>(spec);
	auto texture = std::make_shared<Texture2D>(spec);
	texture->SetData(fileData.data);

	free(fileData.data);
	return texture;
}