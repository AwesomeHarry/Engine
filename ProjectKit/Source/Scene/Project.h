#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <fstream>

#include <json.hpp>

#include "Scene/AssetBank.h"
#include "Scene/Asset.h"

#include "AssetUtils.h"

namespace fs = std::filesystem;

namespace Engine {
	// A Project is a wrapped AssetBank with a managed filesystem. 
	class Project {
	public:
		Project(const std::string& name, const fs::path& rootPath)
			: _name(name), _rootPath(rootPath) {
			_assetBank = std::make_unique<AssetBank>();

			if (!fs::exists(_rootPath)) {
				fs::create_directories(_rootPath);
			}

			LoadAllAssets();
		}

		template<typename T, typename... Args>
		std::shared_ptr<T> CreateAsset(const fs::path& path, Args&&... args) {
			auto asset = AssetFactory::CreateAsset<T>(*_assetBank, std::forward<Args>(args)...);
			fs::path assetPath = _rootPath;

			// Add extension if not present
			std::string extension = path.extension().string();
			if (extension.empty()) {
				extension = AssetUtils::GetAssetExtension<T>();
				assetPath /= path.string() + extension;
			}
			else {
				assetPath /= path;
			}

			_assetPaths[asset->GetId()] = assetPath;
			_pathToId[assetPath.string()] = asset->GetId();
			return asset;
		}

		// Saves asset to file
		void SaveAsset(uint32_t id) {
			auto it = _assetPaths.find(id);
			if (it == _assetPaths.end()) {
				ENGINE_ERROR("[Project::SaveAsset] Asset not found with id: {0}", id);
				return;
			}

			// Create directories if they don't exist
			auto path = it->second;
			if (!fs::exists(path.parent_path())) {
				if (!fs::create_directories(path.parent_path())) {
					ENGINE_ERROR("[Project::SaveAsset] Failed to create directories for asset: {0}", path.string());
					return;
				}
			}

			// If a file already exists, remove it
			if (fs::is_regular_file(path)) {
				fs::remove(path);
			}

			// Serialize Asset and write to file
			auto asset = _assetBank->GetAsset<IAsset>(id);
			if (!asset) {
				ENGINE_ERROR("[Project::SaveAsset] Asset not found with id: {0}", id);
				return;
			}

			auto j = asset->Serialize();

			std::ofstream file(path);
			file << j.dump(4);
			file.close();

			ENGINE_INFO("[Project::SaveAsset] Asset saved: {0}", path.string());
		}

		// Saves all assets to file
		void SaveAllAssets() {
			for (auto& [id, path] : _assetPaths) {
				SaveAsset(id);
			}
		}

		// Loads asset from file
		void AddAsset(const fs::path& path) {
			auto assetPath = _rootPath / path;

			if (!fs::is_regular_file(assetPath)) {
				ENGINE_ERROR("[Project::AddAsset] File not found: {0}", path.string());
				return;
			}

			AssetType type = ParseAssetType(assetPath);
			if (type == AssetType::Unknown) {
				ENGINE_ERROR("[Project::AddAsset] Unknown asset type: {0}", path.string());
				return;
			}

			nlohmann::json jsonData;
			std::ifstream file(assetPath);
			file >> jsonData;
			file.close();

			auto asset = AssetUtils::LoadAssetFromJson(*_assetBank, jsonData, type);

			uint32_t id = asset->GetId();
			_assetPaths[id] = assetPath;
			_pathToId[assetPath.string()] = id;
		}

		// Loads all assets from root directory
		void LoadAllAssets() {
			for (const auto& entry : fs::directory_iterator(_rootPath)) {
				AddAsset(entry.path());
			}
		}

		template<typename T>
		std::shared_ptr<T> GetAsset(uint32_t id) {
			return _assetBank->GetAsset<T>(id);
		}

		template<typename T>
		std::shared_ptr<T> GetAsset(const fs::path& path) {
			fs::path fullPath = _rootPath / path;
			auto it = _pathToId.find(fullPath.string());
			if (it == _pathToId.end()) {
				ENGINE_ERROR("[Project::GetAssetByPath] Asset not found at path: {0}", fullPath.string());
				return nullptr;
			}
			return GetAsset<T>(it->second);
		}

		// Getters
		const std::string& GetName() const { return _name; }
		const fs::path& GetRootPath() const { return _rootPath; }
	private:
		AssetType ParseAssetType(const fs::path& path) {
			std::string extension = path.extension().string();
			if (extension == ".shader")			return AssetType::Shader;
			if (extension == ".tex2d")		return AssetType::Texture2D;
			if (extension == ".cubemap") return AssetType::TextureCubemap;
			if (extension == ".material")		return AssetType::Material;
			if (extension == ".mesh")			return AssetType::Mesh;
			return AssetType::Unknown;
		}
	private:
		std::string _name;
		fs::path _rootPath;

		std::unique_ptr<AssetBank> _assetBank;
		std::unordered_map<uint32_t, fs::path> _assetPaths;
		std::unordered_map<std::string, uint32_t> _pathToId;
	};
}