#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <fstream>

#include <json.hpp>

namespace fs = std::filesystem;

namespace Engine {
	// A Project is a wrapped AssetBank with a managed filesystem. 
	class Project {
	public:
		Project(const std::string& name, const fs::path& rootPath)
			: _name(name), _rootPath(rootPath) {

			if (!fs::exists(_rootPath)) {
				fs::create_directories(_rootPath);
			}

			//_assetBank = std::make_unique<AssetBank>();

			//LoadAllAssets();
		}

		// Getters
		const std::string& GetName() const { return _name; }
		const fs::path& GetRootPath() const { return _rootPath; }
	private:
		std::string _name;
		fs::path _rootPath;
	};
}