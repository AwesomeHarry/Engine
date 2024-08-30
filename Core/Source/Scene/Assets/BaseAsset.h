#pragma once

#include <memory>
#include <string>
#include <json.hpp>

namespace Engine {
	enum class AssetType { Unknown = 0, Shader, Texture2D, TextureCubemap, Material, Mesh };
	NLOHMANN_JSON_SERIALIZE_ENUM(AssetType, {
		{AssetType::Unknown, "Unknown"},
		{AssetType::Shader, "Shader"},
		{AssetType::Texture2D, "Texture2D"},
		{AssetType::TextureCubemap, "TextureCubemap"},
		{AssetType::Material, "Material"},
		{AssetType::Mesh, "Mesh"}
								 });

	struct AssetInfo {
		AssetType type = AssetType::Unknown;
		uint32_t guid = 0;
		std::string name;
	};

	inline void to_json(nlohmann::json& j, const AssetInfo& assetInfo) {
		j = nlohmann::json{
			{"type", assetInfo.type},
			{"id", assetInfo.guid},
			{"name", assetInfo.name}
		};
	}

	inline void from_json(const nlohmann::json& j, AssetInfo& assetInfo) {
		j.at("type").get_to(assetInfo.type);
		j.at("id").get_to(assetInfo.guid);
		j.at("name").get_to(assetInfo.name);
	}

	template<typename T>
	struct BaseAsset : public AssetInfo {
		bool isLoaded = false;
		std::shared_ptr<T> instance; // Generated on first get.
		virtual ~BaseAsset() = default;
		void LoadInstance() {
			if (!isLoaded) {
				isLoaded = Create();
			}
		}
	private:
		virtual bool Create() = 0;
	};

	template<typename T>
	struct Asset : public BaseAsset<T> {
		Asset() { type = AssetType::Unknown; }
		void Create() override {}
	};

#define SERIALIZE_ASSET_INFO to_json(j, static_cast<const AssetInfo&>(asset));
#define DESERIALIZE_ASSET_INFO from_json(j, static_cast<AssetInfo&>(asset));
}