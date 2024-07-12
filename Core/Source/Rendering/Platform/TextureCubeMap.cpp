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
		int channels;
		float* data = stbi_loadf(path.c_str(), &spec.width, &spec.height, &channels, 0);

		float min=100, max=0;
		for (int i = 0; i < 100000; i++) {
			if (data[i] < min) min = data[i];
			if (data[i] > max) max = data[i];
		}
		ENGINE_TRACE("Min: {}, Max: {}", min, max);

		if (!data) {
			ENGINE_WARN("Failed to load texture from path: {}", path);
			return nullptr;
		}

		if (channels == 3) spec.format = ImageFormat::RGB16F;
		else if (channels == 4) spec.format = ImageFormat::RGBA16F;
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
