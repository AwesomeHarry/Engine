#pragma once
#include <stdint.h>
#include <memory>
#include <json.hpp>

#include "AssetBank.h"

namespace Engine {
	class AssetRef {
	public:
		AssetRef()
			: _id(0) {}
		explicit AssetRef(uint32_t guid)
			: _id(guid) {}

		uint32_t GetId() const { return _id; }

		template<typename T>
		std::shared_ptr<T> Resolve(AssetBank& assetBank) const {
			auto asset = assetBank.GetAsset<T>(_id);
			if (!asset) {
				ENGINE_ERROR("[AssetRef] Asset does not exist: {0}", _id);
			}
			return asset;
		}

	private:
		uint32_t _id;
	};

	// JSON Serialization declarations
	void to_json(nlohmann::json& j, const AssetRef& assetRef);
	void from_json(const nlohmann::json& j, AssetRef& assetRef);
}
