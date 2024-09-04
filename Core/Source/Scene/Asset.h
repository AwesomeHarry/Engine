#pragma once

#include <stdint.h>
#include <memory>
#include <json.hpp>

#include "AssetTypes.h"
#include "Logging/Logging.h"

namespace Engine {
	class AssetBank;

	// Asset Interface Class
	class IAsset {
	public:
		virtual void Instantiate() = 0;
		virtual nlohmann::json Serialize() const = 0;
		virtual void Deserialize(const nlohmann::json& j) = 0;

		virtual uint32_t GetId() const = 0;
		virtual AssetType GetType() const = 0;
		virtual const std::string& GetName() const = 0;
	};

	// Template Asset Class
	template<typename T>
	class Asset : public IAsset {
	public:
		Asset(AssetBank& assetBank, uint32_t id, const std::string& name, AssetType type)
			: _assetBank(assetBank), _id(id), _name(name), _type(type) {}
		virtual ~Asset() = default;

		void Instantiate() override;
		bool IsInstantiated() const { return _instance != nullptr; }

		virtual nlohmann::json Serialize() const override;
		virtual void Deserialize(const nlohmann::json& j) override;

		void SetName(const std::string& name) { _name = name; }

		uint32_t GetId() const override { return _id; }
		AssetType GetType() const override { return _type; }
		const std::string& GetName() const override { return _name; }
		std::shared_ptr<T> GetInstance();

	protected:
		virtual std::shared_ptr<T> CreateInstance() = 0;
	protected:
		AssetBank& _assetBank;
		uint32_t _id;
		AssetType _type;
		std::string _name;
		std::shared_ptr<T> _instance;
	};

	template<typename T>
	inline void Asset<T>::Instantiate() {
		if (!_instance) {
			_instance = CreateInstance();
			ENGINE_INFO("Asset [{0}] Instantiated:\t[Id] {1}\t[Name] '{2}'", AssetTypeToString(_type), _id, _name);
		}
	}

	template<typename T>
	inline nlohmann::json Asset<T>::Serialize() const {
		nlohmann::json j;
		j["id"] = _id;
		j["type"] = _type;
		j["name"] = _name;
		return j;
	}

	template<typename T>
	inline void Asset<T>::Deserialize(const nlohmann::json& j) {
		_id = j["id"].get<uint32_t>();
		_type = j["type"].get<AssetType>();
		_name = j["name"].get<std::string>();
	}

	template<typename T>
	inline std::shared_ptr<T> Asset<T>::GetInstance() {
		Instantiate(); return _instance;
	}
}