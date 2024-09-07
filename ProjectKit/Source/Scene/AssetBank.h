#pragma once

#include <memory>
#include <unordered_map>

#include "Logging/Logging.h"
#include "AssetTypes.h"

namespace Engine {
	class IAsset;

	// Used To Manage Assets: Store / Delete / Get
	class AssetBank {
	public:
		// Checks if Asset with id exists
		bool HasAsset(uint32_t id);

		// Deletes Asset with id
		void DeleteAsset(uint32_t id);

		// Deletes all assets
		void DeleteAll();

		// Generates new unique ID
		uint32_t GenerateID();

		// Registers Asset in AssetBank
		template<typename T>
		void RegisterAsset(std::shared_ptr<T> asset) {
			auto id = asset->GetId();
			if (id == 0) {
				ENGINE_ERROR("[AssetBank] Invalid asset ID: {0}", id);
				return;
			}

			if (HasAsset(id)) {
				ENGINE_ERROR("[AssetBank] Asset with ID already exists: {0}", id);
				return;
			}

			// Could cause gaps in IDs, so inefficient
			if (id >= _idCounter) {
				_idCounter = id + 1;
			}

			_assets[id] = asset;
		}

		// Gets Asset with id
		template<typename T>
		std::shared_ptr<T> GetAsset(uint32_t id) {
			if (!HasAsset(id)) {
				ENGINE_ERROR("[AssetBank] Asset not found: {0}", id);
				return nullptr;
			}

			auto castedAsset = std::dynamic_pointer_cast<T>(_assets[id]);

			if (!castedAsset) {
				ENGINE_ERROR("[AssetBank] Asset cannot be casted to this type: {0}", id);
				return nullptr;
			}

			return castedAsset;
		}

	private:
		// Stores all assets
		std::unordered_map<uint32_t, std::shared_ptr<IAsset>> _assets;

		// Used to generate unique IDs for assets
		// id=0 is reserved for invalid assets
		uint32_t _idCounter = 1;
	};

	// Used To Create Assets
	class AssetFactory {
	public:
		// Creates an Asset and registers it to the AssetBank
		template<typename T, typename... Args>
		static std::shared_ptr<T> CreateAsset(AssetBank& assetBank, Args&&... args) {
			auto assetID = assetBank.GenerateID();
			auto asset = std::make_shared<T>(assetBank, assetID, std::forward<Args>(args)...);
			assetBank.RegisterAsset(asset);
			return asset;
		}
	};
}