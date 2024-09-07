#pragma once

#include "Scene/Asset.h"
#include "Rendering/Platform/Mesh.h"
#include "Util/Mesh/GltfIO.h"

namespace Engine {
	class MeshAsset : public Asset<Mesh> {
	public:
		MeshAsset(AssetBank& assetBank, uint32_t id, const std::string& filepath, uint32_t meshIndex)
			: Asset(assetBank, id, "", AssetType::Mesh), _filepath(filepath), _meshIndex(meshIndex) {}

		virtual nlohmann::json Serialize() const override {
			nlohmann::json json = Asset::Serialize();
			json["filepath"] = _filepath;
			json["meshIndex"] = _meshIndex;
			return json;
		}

		virtual void Deserialize(const nlohmann::json& json) override {
			Asset::Deserialize(json);
			_filepath = json["filepath"].get<std::string>();
			_meshIndex = json["meshIndex"].get<uint32_t>();
		}
	protected:
		std::shared_ptr<Mesh> CreateInstance() override {
			auto model = GltfIO::LoadModel(_filepath);
			auto instance = GltfIO::LoadMesh(model, _meshIndex);
			if (!instance) {
				ENGINE_ERROR("[MeshAsset] Failed to load mesh from file: {0}", _filepath);
				return nullptr;
			}
			return instance;
		}
	private:
		std::string _filepath;
		uint32_t _meshIndex;
	};
}