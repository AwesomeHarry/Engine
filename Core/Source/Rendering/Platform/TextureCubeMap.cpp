#include "TextureCubeMap.h"
#include <glad/glad.h>
#include <stb_image.h>
#include "Logging/Logging.h"

using namespace Engine;

void TextureCubeMap::SetData(const std::vector<float*>& data) {
	BindInternal();
	for (uint32_t i = 0; i < data.size(); i++) {
		SetDataInternal(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, data[i]);
	}
	Unbind();
}

void Engine::TextureCubeMap::SetData(const CubeMapData& data) {
	//SetData(data.GetArray());
}

std::shared_ptr<TextureCubeMap> TextureCubeMap::Utils::FromFile(const std::vector<std::string>& paths) {
	TextureSpec spec;
	std::vector<float*> cubemapData;

	stbi_set_flip_vertically_on_load(false);
	for (const auto& path : paths) {
		auto fileData = Texture::Utils::LoadFromFile(path);

		spec.width = fileData.width;
		spec.height = fileData.height;
		spec.format = fileData.format;

		cubemapData.push_back(reinterpret_cast<float*>(fileData.data));
	}

	auto texture = std::make_shared<TextureCubeMap>(spec);
	texture->SetData(cubemapData);

	for (auto data : cubemapData) {
		stbi_image_free(data);
	}

	return texture;
}
