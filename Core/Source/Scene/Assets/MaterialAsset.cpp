#include "MaterialAsset.h"
#include "Scene/AssetManager.h"

using namespace Engine;

bool Asset<Material>::Create() {
	// Load Shader
	auto shaderInstanceID = AssetManager::GetInstanceID<Shader>(shaderGUID);
	if (shaderInstanceID == entt::null) {
		ENGINE_ERROR("Shader not found: {0}", shaderGUID);
		return false;
	}
	auto shader = AssetManager::GetAsset<Shader>(shaderInstanceID).instance;

	// Create Material
	instance = std::make_shared<Engine::Material>(shader);

	// Load Uniforms
	for (const auto& uniform : _uniformMap) {
		switch (uniform.type) {
		case UniformType::Bool:
			instance->SetUniform(uniform.name, std::get<bool>(uniform.value));
			break;
		case UniformType::Int:
			instance->SetUniform(uniform.name, std::get<int>(uniform.value));
			break;
		case UniformType::Float:
			instance->SetUniform(uniform.name, std::get<float>(uniform.value));
			break;
		case UniformType::Vec2:
			instance->SetUniform(uniform.name, std::get<glm::vec2>(uniform.value));
			break;
		case UniformType::Vec3:
			instance->SetUniform(uniform.name, std::get<glm::vec3>(uniform.value));
			break;
		case UniformType::Vec4:
			instance->SetUniform(uniform.name, std::get<glm::vec4>(uniform.value));
			break;
		case UniformType::Mat3:
			instance->SetUniform(uniform.name, std::get<glm::mat3>(uniform.value));
			break;
		case UniformType::Mat4:
			instance->SetUniform(uniform.name, std::get<glm::mat4>(uniform.value));
			break;
		default:
			ENGINE_WARN("Uniform type not supported, not serialized: {0}", uniform.name);
			break;
		}
	}

	// Load Textures
	for (const auto& texture : _textureMap) {
		auto instanceID = AssetManager::GetInstanceID<Engine::Texture2D>(texture.guid);
		if (instanceID != entt::null) {
			auto asset = AssetManager::GetAsset<Engine::Texture2D>(instanceID);
			instance->SetTexture(asset.instance, texture.name);
			continue;
		}

		instanceID = AssetManager::GetInstanceID<Engine::TextureCubemap>(texture.guid);
		if (instanceID != entt::null) {
			auto asset = AssetManager::GetAsset<Engine::TextureCubemap>(instanceID);
			instance->SetTexture(asset.instance, texture.name);
			continue;
		}

		ENGINE_ERROR("Texture not found: {0}", texture.guid);
	}

	return true;
}
