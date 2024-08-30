#include "BaseAsset.h"

#include "Rendering/Platform/Mesh.h"
#include "Util/Mesh/GltfIO.h"

namespace Engine {
	template<>
	struct Asset<Mesh> : BaseAsset<Mesh> {
		Asset() { type = AssetType::Mesh; }
		std::string filepath;
		uint32_t meshIndex = 0;

	private:
		bool Create() override {
			auto gltfModel = GltfIO::LoadModel(filepath);
			auto mesh = GltfIO::LoadMesh(gltfModel, meshIndex);
			if (!mesh) {
				ENGINE_ERROR("Failed to load MeshAsset: id={}", guid);
				return false;
			}

			return true;
		}

		friend void to_json(nlohmann::json&, const Asset<Mesh>&);
		friend void from_json(const nlohmann::json&, Asset<Mesh>&);
	};

	inline void to_json(nlohmann::json& j, const Asset<Mesh>& asset) {
		SERIALIZE_ASSET_INFO;
		j["filepath"] = asset.filepath;
		j["meshIndex"] = asset.meshIndex;
	}

	inline void from_json(const nlohmann::json& j, Asset<Mesh>& asset) {
		DESERIALIZE_ASSET_INFO;
		j.at("filepath").get_to(asset.filepath);
		j.at("meshIndex").get_to(asset.meshIndex);
	}
}
