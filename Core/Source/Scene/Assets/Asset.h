#pragma once

#include <stdint.h>
#include <memory>
#include <json.hpp>

namespace Engine {
	class AssetManager;

	class AssetRef {
	public:
		AssetRef() : _guid(0) {}
		explicit AssetRef(uint32_t guid) : _guid(guid) {}

		template<typename T>
		std::shared_ptr<T> Resolve(AssetManager& assetManager) const;

		uint32_t GetGuid() const { return _guid; }
	private:
		uint32_t _guid;
	};

	// JSON Serialization for AssetRef
	inline void to_json(nlohmann::json& j, const AssetRef& assetRef) {
		j = assetRef.GetGuid();
	}

	inline void from_json(const nlohmann::json& j, AssetRef& assetRef) {
		assetRef = AssetRef(j.get<uint32_t>());
	}

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
			{"guid", assetInfo.guid},
			{"name", assetInfo.name}
		};
	}

	inline void from_json(const nlohmann::json& j, AssetInfo& assetInfo) {
		j.at("type").get_to(assetInfo.type);
		j.at("guid").get_to(assetInfo.guid);
		j.at("name").get_to(assetInfo.name);
	}

	// Base Asset Class
	class BaseAsset : public AssetInfo {
	public:
		virtual ~BaseAsset() = default;
		virtual void Load(AssetManager& assetManager) = 0;
		virtual nlohmann::json Serialize() const = 0;
		virtual void Deserialize(const nlohmann::json& j, AssetManager& assetManager) = 0;
	};

	// Template Asset Class
	template<typename T>
	class Asset : public BaseAsset {
	public:
		Asset() { type = AssetType::Unknown; }
		virtual ~Asset() = default;

		void Load(AssetManager& assetManager) override {
			if (!_instance) {
				_instance = Create(assetManager);
			}
		}

		std::shared_ptr<T> GetInstance() const { return _instance; }
	protected:
		virtual std::shared_ptr<T> Create(AssetManager& assetManager) = 0;
		std::shared_ptr<T> _instance;
	};
}