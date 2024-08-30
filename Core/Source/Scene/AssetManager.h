#include <entt/entt.hpp>
#include <json.hpp>
#include <filesystem>
#include <memory>
#include <string>

#include "Assets/AllAssets.h"
#include "SerializerExtensions.h"

namespace Engine {
	class AssetManager {
	public:
		/* Set Project Root */
		// Sets the project root directory.
		static void SetProjectRoot(const std::filesystem::path& projectRoot) {
			if (!std::filesystem::exists(projectRoot)) {
				ENGINE_ERROR("Project root does not exist: {0}", projectRoot.string());
				return;
			}

			_registry.clear();
			_idCounter = 0;
			_projectRoot = projectRoot;

			// Load all assets
			for (const auto& entry : std::filesystem::recursive_directory_iterator(_projectRoot)) {
				if (entry.is_regular_file()) {
					std::string path = entry.path().filename().string();
					LoadAsset(path);
				}
			}
		}

		/* Create Asset */
		// Creates an asset from base type.
		template<typename T, typename... Args>
		static entt::entity CreateAsset(const std::string& name, Args&&... args) {
			auto entityId = _registry.create();

			Asset<T> asset = CreateAssetFromArgs<T>(std::forward<Args>(args)...);
			asset.name = name;
			asset.guid = _idCounter++;

			// Add asset to registry
			_registry.emplace<Asset<T>>(entityId, asset);

			// Save to disk
			SaveAsset<T>(entityId);

			return entityId;
		}

		/* Save Asset */
		// Saves an asset to disk.
		template<typename T>
		static void SaveAsset(entt::entity entity) {
			auto asset = _registry.get<Asset<T>>(entity);
			std::string filename = asset.name + "_" + std::to_string(asset.guid) + ".asset";
			std::filesystem::path path = _projectRoot / filename;
			nlohmann::json assetJson = asset; // Convert to json
			std::ofstream file(path);
			file << assetJson.dump(4);
			file.close();
		}

		/* Load Asset */
		// Loads an asset from disk.
		// Path is relative to the project root.
		static entt::entity LoadAsset(const std::string& path) {
			std::filesystem::path assetPath = _projectRoot / path;
			if (!std::filesystem::exists(assetPath)) {
				ENGINE_ERROR("Asset does not exist: {0}", assetPath.string());
				return entt::null;
			}

			std::ifstream file(assetPath);
			nlohmann::json assetJson;
			file >> assetJson;
			file.close();

			AssetType assetType = assetJson.at("type").get<AssetType>();
			uint32_t assetId = assetJson.at("id").get<uint32_t>();

			auto entityId = _registry.create();
			switch (assetType) {
			case AssetType::Shader:
			{
				Asset<Shader> shaderAsset = assetJson.get<Asset<Shader>>();
				_registry.emplace<Asset<Shader>>(entityId, shaderAsset);
			}
			break;
			case AssetType::Texture2D:
			{
				Asset<Texture2D> texture2DAsset = assetJson.get<Asset<Texture2D>>();
				_registry.emplace<Asset<Texture2D>>(entityId, texture2DAsset);
			}
			break;
			default:
				ENGINE_ERROR("Unknown asset type.");
				return entt::null;
			}

			_idCounter = std::max(_idCounter, assetId + 1);
			return entityId;
		}

		/* Get Asset */
		// By GUID
		template<typename T>
		static Asset<T>& GetAsset(uint32_t guid) {
			auto view = _registry.view<Asset<T>>();
			for (auto entity : view) {
				auto& asset = view.get<Asset<T>>(entity);
				if (asset.guid == guid)
					return asset;
			}

			ENGINE_ERROR("Asset not found.");
			return Asset<T>();
		}

		// By Instance ID
		template<typename T>
		static Asset<T>& GetAsset(entt::entity entity) {
			if (entity == entt::null) {
				ENGINE_ERROR("Entity is null.");
				return Asset<T>();
			}

			if (!_registry.all_of<Asset<T>>(entity)) {
				ENGINE_ERROR("Entity does not have asset component.");
				return Asset<T>();
			}

			auto& asset = _registry.get<Asset<T>>(entity);
			asset.LoadInstance();
			return asset;
		}

		// By Name
		template<typename T>
		static Asset<T>& GetAsset(const std::string& name) {
			auto entity = GetInstanceID<T>(name);
			return GetAsset<T>(entity);
		}

		/* Get Instance ID */
		// By GUID
		template<typename T>
		static entt::entity GetInstanceID(uint32_t guid) {
			auto view = _registry.view<Asset<T>>();
			for (auto entity : view) {
				auto& asset = view.get<Asset<T>>(entity);
				if (asset.guid == guid)
					return entity;
			}

			return entt::null;
		}

		// By Name
		template<typename T>
		static entt::entity GetInstanceID(const std::string& name) {
			auto view = _registry.view<Asset<T>>();
			for (auto entity : view) {
				auto& asset = view.get<Asset<T>>(entity);
				if (asset.name == name)
					return entity;
			}

			return entt::null;
		}
	private:
		/* Create Resource */
		// Create Asset using required args.
		template<typename T, typename... Args>
		static Asset<T> CreateAssetFromArgs(Args&&... args) {
			if constexpr (std::is_same_v<T, Shader>)
				return CreateShaderAsset(std::forward<Args>(args)...);
			if constexpr (std::is_same_v<T, Texture2D>)
				return CreateTexture2DAsset(std::forward<Args>(args)...);
			if constexpr (std::is_same_v<T, Material>)
				return CreateMaterialAsset(std::forward<Args>(args)...);
			if constexpr (std::is_same_v<T, Mesh>)
				return CreateMeshAsset(std::forward<Args>(args)...);
			else
				ENGINE_ERROR("Unsupported asset type.");

			return Asset<T>();
		}

		static Asset<Shader> CreateShaderAsset(const std::string& filepath) {
			Asset<Shader> asset;
			asset.LoadInstance();
			asset.filepath = filepath;
			return asset;
		}

		static Asset<Texture2D> CreateTexture2DAsset(const std::string& filepath) {
			Asset<Texture2D> asset;
			asset.filepath = filepath;
			asset.LoadInstance();
			return asset;
		}

		static Asset<TextureCubemap> CreateTextureCubemapAsset(const std::string& filepath) {
			Asset<TextureCubemap> asset;
			asset.filepath = filepath;
			asset.LoadInstance();
			return asset;
		}

		static Asset<Mesh> CreateMeshAsset(const std::string& filepath, uint32_t meshIndex) {
			Asset<Mesh> asset;
			asset.filepath = filepath;
			asset.meshIndex = meshIndex;
			asset.LoadInstance();
			return asset;
		}

		static Asset<Material> CreateMaterialAsset(uint32_t shaderGUID) {
			Asset<Material> asset;
			asset.shaderGUID = shaderGUID;
			asset.LoadInstance();
			return asset;
		}

	private:
		static std::filesystem::path _projectRoot;
		static entt::registry _registry;
		static uint32_t _idCounter;
	};
}
