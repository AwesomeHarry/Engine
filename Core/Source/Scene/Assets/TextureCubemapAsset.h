#pragma once

#include "BaseAsset.h"
#include "Rendering/Platform/Texture2D.h"
#include "Rendering/Platform/TextureCubemap.h"

namespace Engine {
	template<>
	struct Asset<TextureCubemap> : public BaseAsset<TextureCubemap> {
		Asset() { type = AssetType::TextureCubemap; }
		std::string filepath;

	private:
		bool Create() override {
			auto tex2D = Texture2D::Utils::FromFile(filepath);
			instance = TextureCubemap::Utils::FromTexture2D(tex2D, TextureCubemap::Utils::Texture2DCubemapFormat::Equirectangle);
			if (!instance) {
				ENGINE_ERROR("Failed to create TextureCubemapAsset: id={}", guid);
				return false;
			}

			return true;
		}

		friend void to_json(nlohmann::json& j, const Asset<TextureCubemap>& asset);
		friend void from_json(const nlohmann::json& j, Asset<TextureCubemap>& asset);
	};

	inline void to_json(nlohmann::json& j, const Asset<TextureCubemap>& asset) {
		SERIALIZE_ASSET_INFO;
		j["filepath"] = asset.filepath;
	}

	inline void from_json(const nlohmann::json& j, Asset<TextureCubemap>& asset) {
		DESERIALIZE_ASSET_INFO;
		j.at("filepath").get_to(asset.filepath);
	}
}
