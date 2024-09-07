#pragma once

#include "AssetBank.h"

namespace Engine {
	class ShaderAsset;
	class Texture2DAsset;
	class TextureCubemapAsset;
	class MaterialAsset;
	class MeshAsset;

	class AssetUtils {
	public:
		static std::shared_ptr<IAsset> LoadAssetFromJson(AssetBank& assetBank, const nlohmann::json& assetJson, AssetType type);

		template<typename T>
		static std::string GetAssetExtension() {
			if constexpr (std::is_same_v<T, ShaderAsset>)
				return ".shader";
			else if constexpr (std::is_same_v<T, Texture2DAsset>)
				return ".tex2d";
			else if constexpr (std::is_same_v<T, TextureCubemapAsset>)
				return ".cubemap";
			else if constexpr (std::is_same_v<T, MaterialAsset>)
				return ".material";
			else if constexpr (std::is_same_v<T, MeshAsset>)
				return ".mesh";

			ENGINE_ERROR("[AssetUtils::GetAssetExtension] Unknown asset type.");
			return ".unknown";
		}
	};
}
