#pragma once

#include "BaseAsset.h"
#include "Rendering/Platform/Texture2D.h"

namespace Engine {
	template<>
	struct Asset<Texture2D> : public BaseAsset<Texture2D> {
		Asset() { type = AssetType::Texture2D; }
		std::string filepath;
		
	private:
		bool Create() override {
			instance = Texture2D::Utils::FromFile(filepath);
			if (!instance) {
				ENGINE_ERROR("Failed to load TextureAsset: id={}", guid);
				return false;
			}
			
			return true;
		}

		friend void to_json(nlohmann::json& j, const Asset<Texture2D>& asset);
		friend void from_json(const nlohmann::json& j, Asset<Texture2D>& asset);
	};

	inline void to_json(nlohmann::json& j, const Asset<Texture2D>& asset) {
		SERIALIZE_ASSET_INFO;
		j["filepath"] = asset.filepath;
	}

	inline void from_json(const nlohmann::json& j, Asset<Texture2D>& asset) {
		DESERIALIZE_ASSET_INFO;
		j.at("filepath").get_to(asset.filepath);
	}
}
