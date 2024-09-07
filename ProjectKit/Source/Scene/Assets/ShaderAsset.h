#pragma once

#include "Scene/Asset.h"
#include "Rendering/Platform/Shader.h"

namespace Engine {
	class ShaderAsset : public Asset<Shader> {
	public:
		ShaderAsset(AssetBank& assetBank, uint32_t id, const std::string& filepath)
			: Asset(assetBank, id, "", AssetType::Shader), _filepath(filepath) {}

		virtual nlohmann::json Serialize() const override {
			nlohmann::json json = Asset::Serialize();
			json["filepath"] = _filepath;
			// Gets uniform blocks from instance if it exists, otherwise gets from existing map
			json["uniformBlocks"] = _instance ? _instance->GetUniformBlocks() : _uniformBlockMap;
			return json;
		}

		virtual void Deserialize(const nlohmann::json& json) override {
			Asset::Deserialize(json);
			_filepath = json["filepath"].get<std::string>();
			_uniformBlockMap = json["uniformBlocks"].get<std::map<std::string, uint32_t>>();
		}
	protected:
		std::shared_ptr<Shader> CreateInstance() override {
			auto instance = Shader::Utils::FromFile(_filepath);
			if (!instance) {
				ENGINE_ERROR("[ShaderAsset] Failed to load shader from file: {0}", _filepath);
				return nullptr;
			}
			for (const auto& [name, binding] : _uniformBlockMap) {
				instance->BindUniformBlock(name, binding);
			}
			return instance;
		}
	private:
		std::string _filepath;
		std::map<std::string, uint32_t> _uniformBlockMap;
	};
}