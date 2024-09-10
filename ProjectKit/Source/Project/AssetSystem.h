#pragma once

#include <memory>

#include "Util/Guid.h"
#include <unordered_map>
#include <json.hpp>

namespace Engine {
	using AssetGUID = std::string;

	enum class AssetType {
		Unkown = 0, Shader, Texture, Material, Mesh
	};

	class Asset;
	class AssetRef;

	class AssetBank {
		std::unordered_map<AssetGUID, std::shared_ptr<Asset>> _assets;
		friend class AssetRef;
		friend class AssetFactory;
	};

	struct AssetRef {
		GUID guid;

		AssetRef() = default;
		AssetRef(const GUID& id) : guid(id) {}

		bool IsValid() const { return !guid.ToString().empty(); }

		template<typename T>
		std::shared_ptr<T> Resolve(AssetBank& assetBank) {
			if (!IsValid()) return nullptr;

			auto it = assetBank._assets.find(guid.ToString());
			if (it == assetBank._assets.end())
				return nullptr;

			return std::dynamic_pointer_cast<T>(it->second);
		}
	};

	inline void to_json(nlohmann::json& j, const AssetRef& p) {
		j = p.guid.ToString();
	}

	inline void from_json(const nlohmann::json& j, AssetRef& p) {
		p.guid = GUID(j.get<std::string>());
	}

	class Asset {
	public:
		Asset(AssetBank& assetBank, AssetType type) : _assetBank(assetBank), _guid(GUID::Invalid()), _type(type) {}

		virtual nlohmann::json Serialize() const {
			nlohmann::json data;
			data["guid"] = _guid.ToString();
			return data;
		}
		virtual void Deserialize(const nlohmann::json& data) {
			_guid = GUID(data["guid"].get<std::string>());
		}

		virtual void Load() = 0;
		virtual void Unload() = 0;

		void SetGUID(const GUID& guid) { _guid = guid; }
		const GUID& GetGUID() const { return _guid; }

		AssetRef GetRef() const { return AssetRef(_guid); }
		AssetType GetType() const { return _type; }

	protected:
		bool _loaded = false;
		AssetBank& _assetBank;

		GUID _guid;
		AssetType _type;
	};

	class AssetFactory {
	public:
		template<typename T>
		static std::pair<std::shared_ptr<T>, AssetRef> Create(AssetBank& assetBank) {
			auto asset = std::make_shared<T>(assetBank);
			GUID guid;
			asset->SetGUID(guid);
			assetBank._assets[guid.ToString()] = asset;
			return { asset, AssetRef(guid) };
		}
	};
}
