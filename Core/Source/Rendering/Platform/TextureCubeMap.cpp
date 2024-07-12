#include "TextureCubeMap.h"
#include <glad/glad.h>
#include <stb_image.h>
#include "Logging/Logging.h"

using namespace Engine;

void TextureCubeMap::SetData(const std::vector<uint8_t*>& data) {
	Bind();
	for (uint32_t i = 0; i < data.size(); i++) {
		SetDataInternal(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, data[i]);
	}
	Unbind();
}

std::shared_ptr<TextureCubeMap> TextureCubeMap::Utils::FromFile(const std::vector<std::string>& paths) {
	TextureSpec spec;
	std::vector<uint8_t*> cubemapData;

	stbi_set_flip_vertically_on_load(true);
	for (const auto& path : paths) {
		int width, height, channels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!data) {
			ENGINE_WARN("Failed to load texture from path: {}", path);
			return nullptr;
		}

		if (channels == 3) spec.format = ImageFormat::RGB8;
		else if (channels == 4) spec.format = ImageFormat::RGBA8;
		else {
			ENGINE_ERROR("Unsupported number of channels: {}", channels);
			stbi_image_free(data);
			return nullptr;
		}

		cubemapData.push_back(data);
	}

	auto texture = std::make_shared<TextureCubeMap>(spec);
	texture->SetData(cubemapData);

	for (auto data : cubemapData) {
		stbi_image_free(data);
	}

	return texture;
}
