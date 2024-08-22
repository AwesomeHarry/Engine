#include "SceneSerializer.h"
#include "Scene/Components/Native/NameComponent.h"
#include "Util/Mesh/GltfIO.h"

#include <json.hpp>
#include "Scene/Entity.h"
#include "Scene/Components/Native/Components.h"

#include <fstream>
#include <filesystem>

using namespace Engine;
using json = nlohmann::json;

namespace nlohmann {
	template<>
	struct adl_serializer<NameComponent> {
		static void to_json(json& j, const NameComponent& component) {
			j = json{
				{"name",component.name}
			};
		}

		static void from_json(const json& j, NameComponent& component) {
			j.at("name").get_to(component.name);
		}
	};

	template<>
	struct adl_serializer<glm::vec3> {
		static void to_json(json& j, const glm::vec3& vec) {
			j = json{
				{"x",vec.x},
				{"y",vec.y},
				{"z",vec.z}
			};
		}

		static void from_json(const json& j, glm::vec3& vec) {
			j.at("x").get_to(vec.x);
			j.at("y").get_to(vec.y);
			j.at("z").get_to(vec.z);
		}
	};

	template<>
	struct adl_serializer<TransformComponent> {
		static void to_json(json& j, const TransformComponent& component) {
			j = json{
				{"position",component.position},
				{"rotation",component.rotation},
				{"scale",component.scale}
			};
		}

		static void from_json(const json& j, TransformComponent& component) {
			j.at("position").get_to(component.position);
			j.at("rotation").get_to(component.rotation);
			j.at("scale").get_to(component.scale);
		}
	};

	template<>
	struct adl_serializer<FlagSet<BufferBit>> {
		static void to_json(json& j, const FlagSet<BufferBit>& flags) {
			j = flags.to_string();
		}

		static void from_json(const json& j, FlagSet<BufferBit>& flags) {
			std::string flags_str = j.get<std::string>();
			flags.reset();
			for (char c : flags_str) {
				if (c == '1') {
					flags |= static_cast<Engine::BufferBit>(flags_str.length() - (&c - flags_str.data()));
				}
			}
		}
	};

	template<>
	struct adl_serializer<Framebuffer::FramebufferSpec> {
		static void to_json(json& j, const Framebuffer::FramebufferSpec& fbSpec) {
			j = json{
				{"width",fbSpec.width},
				{"height",fbSpec.height},
				{"attachments",json::array()},
				{"includeDepthStencil",fbSpec.includeDepthStencil}
			};
			json& attachments = j["attachments"];
			for (const auto& attachment : fbSpec.attachments) {
				attachments.push_back(attachment);
			}
		}

		static void from_json(const json& j, Framebuffer::FramebufferSpec& fbSpec) {
			j.at("width").get_to(fbSpec.width);
			j.at("height").get_to(fbSpec.height);
			j.at("includeDepthStencil").get_to(fbSpec.includeDepthStencil);
			for (const auto& attachment : j.at("attachments")) {
				fbSpec.attachments.push_back(attachment.get<ImageFormat>());
			}
		}
	};

	template<>
	struct adl_serializer<CameraComponent> {
		static void to_json(json& j, const CameraComponent& component) {
			j = json{
				{"type",component.type},
				{"orthographicSize",component.orthographicSpec.size},
				{"perspectiveFov",component.perspectiveSpec.fov},
				{"nearPlane",component.nearPlane},
				{"farPlane",component.farPlane},
				{"clearFlags",component.clearFlags},
				{"priority",component.priority},
				{"backgroundType",component.backgroundType},
				{"backgroundColor",component.backgroundColor},
				//{"skyboxCubemap",component.skyboxCubemap},
				{"skyboxExposure",component.skyboxExposure},
				{"renderTarget",component.renderTarget},
			};
			if (component.renderFramebuffer == nullptr)
				j["renderFramebuffer"] = nullptr;
			else
				j["renderFramebuffer"] = component.renderFramebuffer->GetSpecification();
		}

		static void from_json(const json& j, CameraComponent& component) {
			j.at("type").get_to(component.type);
			j.at("orthographicSize").get_to(component.orthographicSpec.size);
			j.at("perspectiveFov").get_to(component.perspectiveSpec.fov);
			j.at("nearPlane").get_to(component.nearPlane);
			j.at("farPlane").get_to(component.farPlane);
			j.at("clearFlags").get_to(component.clearFlags);
			j.at("priority").get_to(component.priority);
			j.at("backgroundType").get_to(component.backgroundType);
			j.at("backgroundColor").get_to(component.backgroundColor);
			//j.at("skyboxCubemap").get_to(component.skyboxCubemap);
			j.at("skyboxExposure").get_to(component.skyboxExposure);
			j.at("renderTarget").get_to(component.renderTarget);
			if (j.at("renderFramebuffer").is_null())
				component.renderFramebuffer = nullptr;
			else {
				Framebuffer::FramebufferSpec fbSpec;
				j.at("renderFramebuffer").get_to(fbSpec);
				component.renderFramebuffer = std::make_shared<Framebuffer>(fbSpec);
			}
		}
	};

	template<>
	struct adl_serializer<Mesh> {
		static void from_json(const json& j, Mesh& mesh) {
			std::string path = j.at("path").get<std::string>();
			tinygltf::Model model = GltfIO::LoadFile(path);
			for (const auto& primitive : model.meshes[0].primitives) {
				mesh.AddSubmesh(GltfIO::LoadPrimitive(model, primitive));
			}
		}
	};

	template<>
	struct adl_serializer<MeshFilterComponent> {
		static void from_json(const json& j, MeshFilterComponent& component) {
			component.mesh = std::make_shared<Mesh>(j.at("mesh").get<Mesh>());
		}
	};
}

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
			std::filesystem::path meshPath = assetsPath / mfc.mesh->GetName();
			std::string meshPathStr = meshPath.string();
			GltfIO::ExportMeshToGltf(*mfc.mesh, meshPathStr);
			entityJson["meshFilter"] = json{{"mesh",meshPathStr}};
		}

		sceneJson["entities"].push_back(entityJson);
	}

	std::ofstream file(savePath.string());
	file << sceneJson.dump(4);
	file.close();

	ENGINE_TRACE("Scene saved to {0}", savePath.string());
}
