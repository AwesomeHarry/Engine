#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <fstream>

#include <json.hpp>

#include "AssetSystem.h"

#include "Assets/ShaderAsset.h"
#include "Assets/TextureAsset.h"
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"
#include "Assets/SceneAsset.h"

namespace fs = std::filesystem;

namespace Engine {
	std::string GetAssetExtension(AssetType assetType) {
		switch (assetType) {
		case AssetType::Shader: return ".shader";
		case AssetType::Texture: return ".texture";
		case AssetType::Material: return ".material";
		case AssetType::Mesh: return ".mesh";
		case AssetType::Scene: return ".scene";

		default: ENGINE_ERROR("[GetAssetExtension] Unknown asset type '{0}'");
		}
	}

	AssetType ParseAssetType(const std::string& extension) {
		if (extension == ".shader")		return AssetType::Shader;
		if (extension == ".texture")	return AssetType::Texture;
		if (extension == ".material")	return AssetType::Material;
		if (extension == ".mesh")		return AssetType::Mesh;
		if (extension == ".scene")		return AssetType::Scene;

		ENGINE_ERROR("[ParseAssetType] Unknown asset extension '{0}'", extension);
		return AssetType::Unkown;
	}

	// A Project is a wrapped AssetBank with a managed filesystem. 
	class Project {
	public:
		Project(const std::string& name, const fs::path& rootPath)
			: _name(name) {

			_rootPath = rootPath.parent_path() / name;
			fs::create_directories(_rootPath);

			_assetBank = std::make_unique<AssetBank>();

			LoadAllAssets();
		}

		// Create Asset
		template<typename T>
		std::pair<std::shared_ptr<T>, AssetRef> CreateAsset(const std::string& name, const fs::path& path) {
			// Check for invalid name
			if (name.empty()) {
				ENGINE_ERROR("[Project::CreateAsset] Invalid asset name '{0}'", name);
				return { nullptr, GUID::Invalid() };
			}

			// Check for duplicate name
			if (_nameToAssetRefs.find(name) != _nameToAssetRefs.end()) {
				ENGINE_ERROR("[Project::CreateAsset] Asset with name '{0}' already exists in project!", name);
				return { nullptr, GUID::Invalid() };
			}

			auto [asset, assetRef] = AssetFactory::Create<T>(*_assetBank);

			AssetData data;
			data.path = path.parent_path() / (name + GetAssetExtension(asset->GetType()));
			data.name = name;

			_assetData[assetRef.guid.ToString()] = data;
			_nameToAssetRefs[name] = assetRef;

			// Save the asset to disk
			SaveAsset(assetRef);

			return { asset,assetRef };
		}

		// Save Asset
		void SaveAsset(AssetRef assetRef) {
			auto it = _assetData.find(assetRef.guid.ToString());
			if (it == _assetData.end()) {
				ENGINE_ERROR("[Project::SaveAsset] Asset with guid '{0}' not found in project!", assetRef.guid.ToString());
				return;
			}

			const AssetData& data = it->second;
			fs::path assetPath = _rootPath / data.path;

			// Ensure directory exists
			fs::create_directories(assetPath.parent_path());

			if (fs::is_regular_file(assetPath)) {
				ENGINE_WARN("[Project::SaveAsset] Overwriting asset '{0}' at '{1}'", assetRef.guid.ToString(), assetPath.string());
			}

			// Get Asset Data
			auto asset = assetRef.Resolve<Asset>(*_assetBank);
			nlohmann::json assetJson = asset->Serialize();

			std::ofstream file(assetPath);
			file << assetJson.dump(4);
			file.close();

			ENGINE_INFO("[Project::SaveAsset] Saved asset '{0}' to '{1}'", assetRef.guid.ToString(), assetPath.string());
		}

		// Load Asset
		void LoadAsset(const fs::path& path) {
			fs::path assetPath = _rootPath / path;
			std::string assetName = assetPath.filename().string();

			// Check for invalid name
			if (assetName.empty()) {
				ENGINE_ERROR("[Project::LoadAsset] Invalid asset name '{0}'", assetName);
				return;
			}

			// Check for duplicate name
			if (_nameToAssetRefs.find(assetName) != _nameToAssetRefs.end()) {
				ENGINE_ERROR("[Project::LoadAsset] Asset with name '{0}' already exists in project!", assetName);
				return;
			}

			if (!fs::is_regular_file(assetPath)) {
				ENGINE_ERROR("[Project::LoadAsset] Asset not found at '{0}'", assetPath.string());
				return;
			}

			// Load Asset Data
			std::ifstream file(assetPath);
			nlohmann::json assetJson;
			file >> assetJson;
			file.close();

			// Get Asset Type
			std::shared_ptr<Asset> asset;

			AssetType assetType = ParseAssetType(assetPath.extension().string());
			GUID guid(assetJson["guid"].get<std::string>());

			if (guid == GUID::Invalid()) {
				ENGINE_ERROR("[Project::LoadAsset] Invalid GUID for asset '{0}'", assetName);
				return;
			}

			if (HasAsset(AssetRef(guid))) {
				ENGINE_ERROR("[Project::LoadAsset] Asset with GUID '{0}' already exists in project!", guid.ToString());
				return;
			}

			switch (assetType) {
			case AssetType::Shader: // Shader
				asset = AssetFactory::CreateWithGUID<ShaderAsset>(*_assetBank, guid).first;
				break;
			case AssetType::Texture: // Texture
				asset = AssetFactory::CreateWithGUID<TextureAsset>(*_assetBank, guid).first;
				break;
			case AssetType::Material: // Material
				asset = AssetFactory::CreateWithGUID<MaterialAsset>(*_assetBank, guid).first;
				break;
			case AssetType::Mesh: // Mesh
				asset = AssetFactory::CreateWithGUID<MeshAsset>(*_assetBank, guid).first;
				break;
			case AssetType::Scene: // Scene
				asset = AssetFactory::CreateWithGUID<SceneAsset>(*_assetBank, guid).first;
				break;
			default:
				ENGINE_ERROR("[Project::LoadAsset] Unknown asset type '{0}'");
				return;
			}

			AssetData data;
			data.path = assetPath;
			data.name = assetName;

			_assetData[asset->GetGUID().ToString()] = data;
			_nameToAssetRefs[assetName] = asset->GetRef();

			// Deserialize Asset
			asset->Deserialize(assetJson);

			ENGINE_INFO("[Project::LoadAsset] Loaded asset '{0}' from '{1}'", assetName, assetPath.string());
		}

		// Has Asset
		bool HasAsset(AssetRef assetRef) {
			return _assetData.find(assetRef.guid.ToString()) != _assetData.end();
		}

		// Get Asset
		AssetRef GetAssetRef(const std::string& name) {
			auto it = _nameToAssetRefs.find(name);
			if (it == _nameToAssetRefs.end()) {
				ENGINE_ERROR("[Project::GetAssetRef] Asset with name '{0}' not found in project!", name);
				return AssetRef();
			}

			return it->second;
		}

		// Save All Assets
		void SaveAllAssets() {
			for (auto& [guid, _] : _assetData) {
				SaveAsset(AssetRef(guid));
			}
		}

		// Load All Assets
		void LoadAllAssets() {
			for (auto& entry : fs::recursive_directory_iterator(_rootPath)) {
				if (entry.is_regular_file()) {
					fs::path relativePath = fs::relative(entry.path(), _rootPath);
					LoadAsset(relativePath);
				}
			}
		}

		// Getters
		const std::string& GetProjectName() const { return _name; }
		const fs::path& GetRootPath() const { return _rootPath; }
		AssetBank& GetAssetBank() { return *_assetBank; }
	private:
		std::string _name;
		fs::path _rootPath;

		std::unique_ptr<AssetBank> _assetBank;

		struct AssetData {
			fs::path path;
			std::string name;
		};

		std::unordered_map<AssetGUID, AssetData> _assetData;
		std::unordered_map<std::string, AssetRef> _nameToAssetRefs;
	};
}