#pragma once

#include "Asset.h"
#include "Rendering/Platform/Shader.h"

namespace Engine {
	class ShaderAsset : public Asset<Shader> {
	public:
		ShaderAsset() { type = AssetType::Shader; }

		nlohmann::json Serialize() const override {
			nlohmann::json j;
			to_json(j, static_cast<const AssetInfo&>(*this));
			j["filepath"] = _filepath;
			j["uniformBlocks"] = _instance ? _instance->GetUniformBlocks() : _uniformBlockMap;
			return j;
		}

		void Deserialize(const nlohmann::json& j, AssetManager& assetManager) override {
			from_json(j, static_cast<AssetInfo&>(*this));
			j.at("filepath").get_to(_filepath);
			j.at("uniformBlocks").get_to(_uniformBlockMap);
		}
	protected:
		std::shared_ptr<Shader> Create(AssetManager& assetManager) override {
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