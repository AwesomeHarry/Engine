#include "SceneSerializer.h"
#include "SerializerExtensions.h"

#include <json.hpp>
#include <fstream>
#include <filesystem>

using namespace Engine;
using json = nlohmann::json;



void SceneSerializer::Serialize(const Scene& scene, const std::string& fileName) {
	std::filesystem::path execPath = std::filesystem::current_path();

	std::filesystem::path savePath = execPath / "saves" / (fileName + ".scene");
	std::filesystem::path assetsPath = execPath / "saves" / "assets";

	std::filesystem::create_directories(savePath.parent_path());
	std::filesystem::create_directories(assetsPath);

	// Serialize scene to file
	json sceneJson;
	sceneJson["entities"] = json::array();

	auto& reg = scene.GetRegistry();
	for (auto entity : reg.view<entt::entity>()) {
		json entityJson;
		entityJson["entity"] = entity;

		auto& nc = reg.get<NameComponent>(entity);
		entityJson["name"] = nc.name;

		auto& tc = reg.get<TransformComponent>(entity);
		entityJson["transform"] = tc;

		if (reg.all_of<CameraComponent>(entity)) {
			auto& cc = reg.get<CameraComponent>(entity);
			entityJson["camera"] = cc;
		}

		if (reg.all_of<MeshFilterComponent>(entity)) {
			auto& mfc = reg.get<MeshFilterComponent>(entity);
			//std::filesystem::path meshPath = assetsPath / mfc.mesh->GetName();
			//std::string meshPathStr = meshPath.string();
			//GltfIO::ExportMeshToGltf(*mfc.mesh, meshPathStr);
			//entityJson["meshFilter"] = json{{"mesh",meshPathStr}};
		}

		sceneJson["entities"].push_back(entityJson);
	}

	std::ofstream file(savePath.string());
	file << sceneJson.dump(4);
	file.close();

	ENGINE_TRACE("Scene saved to {0}", savePath.string());
}
