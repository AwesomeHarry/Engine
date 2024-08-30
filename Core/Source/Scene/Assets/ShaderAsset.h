#pragma once

#include "BaseAsset.h"
#include "Rendering/Platform/Shader.h"

namespace Engine {
	template<>
	struct Asset<Shader> : public BaseAsset<Shader> {
		Asset() { type = AssetType::Shader; }
		std::string filepath;

	private:
		bool Create() override {
			instance = Shader::Utils::FromFile(filepath);
			if (!instance) {
				ENGINE_ERROR("Failed to load ShaderAsset: id={}", guid);
				return false;
			}

			for (const auto& [name, index] : _uniformBlockMap) {
				instance->BindUniformBlock(name, index);
			}

			return true;
		}
	private:
		// Reflects json deserialization
		std::map<std::string, uint32_t> _uniformBlockMap;

		friend void to_json(nlohmann::json& j, const Asset<Shader>& asset);
		friend void from_json(const nlohmann::json& j, Asset<Shader>& asset);
	};

	inline void to_json(nlohmann::json& j, const Asset<Shader>& asset) {
		SERIALIZE_ASSET_INFO;
		j["filepath"] = asset.filepath;
		j["uniformBlocks"] = asset.isLoaded ? asset.instance->GetUniformBlocks() : asset._uniformBlockMap;
	}

	inline void from_json(const nlohmann::json& j, Asset<Shader>& asset) {
		DESERIALIZE_ASSET_INFO;
		j.at("filepath").get_to(asset.filepath);
		j.at("uniformBlocks").get_to(asset._uniformBlockMap);
	}
}
