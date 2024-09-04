#pragma once

#include "AssetBank.h"

namespace Engine {
	class AssetUtils {
	public:
		static std::shared_ptr<IAsset> LoadAssetFromJson(AssetBank& assetBank, const nlohmann::json& assetJson);
	};
}
