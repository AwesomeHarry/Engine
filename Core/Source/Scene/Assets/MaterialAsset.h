#include "BaseAsset.h"

#include "Rendering/Platform/Material.h"
#include "Rendering/Platform/Texture2D.h"
#include "Rendering/Platform/TextureCubemap.h"

namespace Engine {
	template<>
	struct Asset<Material> : BaseAsset<Material> {
		Asset() { type = AssetType::Mesh; }
		uint32_t shaderGUID = 0;

		struct SerializedUniform {
			std::string name;
			UniformType type;
			UniformValue value;
		};

		struct SerializedTexture {
			std::string name;
			uint32_t guid;
			uint32_t bindingIndex;
		};
	private:
		std::vector<SerializedUniform> _uniformMap;
		std::vector<SerializedTexture> _textureMap;

		bool Create() override;

		friend void to_json(nlohmann::json&, const Asset<Material>&);
		friend void from_json(const nlohmann::json&, Asset<Material>&);
	};

	inline void to_json(nlohmann::json& j, const Asset<Material>::SerializedUniform& uniform) {
		j["name"] = uniform.name;
		j["type"] = uniform.type;
		switch (uniform.type) {
		case UniformType::Bool:
			j["value"] = std::get<bool>(uniform.value);
			break;
		case UniformType::Int:
			j["value"] = std::get<int>(uniform.value);
			break;
		case UniformType::Float:
			j["value"] = std::get<float>(uniform.value);
			break;
		case UniformType::Vec2:
			j["value"] = std::get<glm::vec2>(uniform.value);
			break;
		case UniformType::Vec3:
			j["value"] = std::get<glm::vec3>(uniform.value);
			break;
		case UniformType::Vec4:
			j["value"] = std::get<glm::vec4>(uniform.value);
			break;
		case UniformType::Mat3:
			j["value"] = std::get<glm::mat3>(uniform.value);
			break;
		case UniformType::Mat4:
			j["value"] = std::get<glm::mat4>(uniform.value);
			break;
		default:
			break;
		}
	}

	inline void from_json(const nlohmann::json& j, Asset<Material>::SerializedUniform& uniform) {
		j.at("name").get_to(uniform.name);
		j.at("type").get_to(uniform.type);
		switch (uniform.type) {
		case UniformType::Bool:
			j.at("value").get_to(std::get<bool>(uniform.value));
			break;
		case UniformType::Int:
			j.at("value").get_to(std::get<int>(uniform.value));
			break;
		case UniformType::Float:
			j.at("value").get_to(std::get<float>(uniform.value));
			break;
		case UniformType::Vec2:
			j.at("value").get_to(std::get<glm::vec2>(uniform.value));
			break;
		case UniformType::Vec3:
			j.at("value").get_to(std::get<glm::vec3>(uniform.value));
			break;
		case UniformType::Vec4:
			j.at("value").get_to(std::get<glm::vec4>(uniform.value));
			break;
		case UniformType::Mat3:
			j.at("value").get_to(std::get<glm::mat3>(uniform.value));
			break;
		case UniformType::Mat4:
			j.at("value").get_to(std::get<glm::mat4>(uniform.value));
			break;
		default:
			break;
		}
	}

	inline void to_json(nlohmann::json& j, const Asset<Material>::SerializedTexture& texture) {
		j["name"] = texture.name;
		j["guid"] = texture.guid;
		j["bindingIndex"] = texture.bindingIndex;
	}

	inline void from_json(const nlohmann::json& j, Asset<Material>::SerializedTexture& texture) {
		j.at("name").get_to(texture.name);
		j.at("guid").get_to(texture.guid);
		j.at("bindingIndex").get_to(texture.bindingIndex);
	}

	inline void to_json(nlohmann::json& j, const Asset<Material>& asset) {
		SERIALIZE_ASSET_INFO;
		j["shaderIndex"] = asset.shaderGUID;
		// Get uniforms from instance
		if (asset.isLoaded) {
			std::vector<Asset<Material>::SerializedUniform> uniforms;
			auto uniformNames = asset.instance->GetUniformNames();
			auto uniformInfos = asset.instance->GetShader().GetUniformInfos();
			for (const auto& name : uniformNames) {
				Asset<Material>::SerializedUniform uniform;
				uniform.name = name;
				uniform.type = uniformInfos[name].type;
				uniform.value = asset.instance->GetUniform(name).value();
				uniforms.push_back(uniform);
			}
			j["uniforms"] = uniforms;
		}
		else {
			j["uniforms"] = asset._uniformMap;
		}

		// Serialize Textures
		//if (asset.isLoaded) {} // Not implemented
		j["textures"] = asset._textureMap;
	}

	inline void from_json(const nlohmann::json& j, Asset<Material>& asset) {
		DESERIALIZE_ASSET_INFO;
		j.at("shaderIndex").get_to(asset.shaderGUID);
		j.at("uniforms").get_to(asset._uniformMap);
		j.at("textures").get_to(asset._textureMap);
	}
}
