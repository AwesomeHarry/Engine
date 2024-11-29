#pragma once

#include "Project/AssetSystem.h";
#include "Rendering/Platform/Mesh.h"
#include "Util/Mesh/GltfIO.h"

namespace Engine {
	class MeshAsset : public Asset {
	public:
		MeshAsset(AssetBank& assetBank) : Asset(assetBank, AssetType::Mesh), _meshIndex(0) {}

		void Load() override {
			auto model = GltfIO::LoadModel(_meshPath);
			_internalMesh = GltfIO::LoadMesh(model, _meshIndex);
			_loaded = true;
		}

		void Unload() override {
			_internalMesh = nullptr;
			_loaded = false;
		}

		std::shared_ptr<Mesh> GetInternal() {
			if (!_loaded) Load();
			return _internalMesh;
		}

		nlohmann::json Serialize() const override {
			nlohmann::json data = Asset::Serialize();
			data["meshPath"] = _meshPath;
			data["meshIndex"] = _meshIndex;
			return data;
		}

		void Deserialize(const nlohmann::json& data) override {
			Asset::Deserialize(data);
			_meshPath = data["meshPath"].get<std::string>();
			_meshIndex = data["meshIndex"].get<uint32_t>();
			if (_loaded) Unload();
		}

		const std::string& GetMeshPath() const { return _meshPath; }
		void SetMeshPath(const std::string& meshPath) {
			if (meshPath != _meshPath) Unload(); // If path changed, unload the mesh
			_meshPath = meshPath;
		}

		uint32_t GetMeshIndex() const { return _meshIndex; }
		void SetMeshIndex(uint32_t meshIndex) {
			if (meshIndex != _meshIndex) Unload(); // If index changed, unload the mesh
			_meshIndex = meshIndex;
		}
	private:
		std::shared_ptr<Mesh> _internalMesh = nullptr;
		std::string _meshPath;
		uint32_t _meshIndex;
	};
}