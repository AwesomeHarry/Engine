#pragma once

#include "Project/AssetSystem.h"
#include "Project/Scene/Scene.h"
#include "Project/Scene/Entity.h"
#include "Project/Scene/Components/Native/Components.h"

#include "Serializers/ComponentSerializer.h"

namespace Engine {
	// No lazy loading for scenes
	class SceneAsset : public Asset {
	public:
		SceneAsset(AssetBank& assetBank) : Asset(assetBank, AssetType::Scene) {
			_loaded = true;
			_internalScene = std::make_shared<Scene>();
		}

		void Load() override {}
		void Unload() override {}

		nlohmann::json Serialize() const override {
			nlohmann::json data = Asset::Serialize();
			auto& registry = _internalScene->GetRegistry();
			for (auto entity : registry.view<entt::entity>()) {
				if (registry.all_of<DebugShapeManager>(entity))
					continue;

				auto& transform = registry.get<TransformComponent>(entity);

				Entity e(entity, _internalScene.get());

				nlohmann::json entityData;
				entityData["name"] = e.GetName();
				entityData["transform"] = e.GetTransform();

				nlohmann::json components;

				if (e.HasComponent<MeshFilterComponent>())
					components["mesh-filter"] = e.GetComponent<MeshFilterComponent>();

				if (e.HasComponent<MeshRendererComponent>())
					components["mesh-renderer"] = e.GetComponent<MeshRendererComponent>();

				if (e.HasComponent<CameraComponent>())
					components["camera"] = e.GetComponent<CameraComponent>();

				entityData["components"] = components;
				data["entities"].push_back(entityData);
			}

			return data;
		}

		void Deserialize(const nlohmann::json& data) override {
			Asset::Deserialize(data);
			_internalScene = std::make_shared<Scene>();
			auto entities = data["entities"];
			for (auto entity : entities) {
				Entity e = _internalScene->CreateEntity(entity["name"]);
				e.GetTransform() = entity["transform"];

				auto components = entity["components"];
				if (components.contains("mesh-filter")) {
					auto id = components["mesh-filter"]["id"].get<GUID>();
					auto meshAsset = AssetRef(id).Resolve<MeshAsset>(_assetBank);
					e.AddComponent<MeshFilterComponent>(meshAsset);
				}

				if (components.contains("mesh-renderer")) {
					auto id = components["mesh-renderer"]["material"].get<GUID>();
					auto materialAsset = AssetRef(id).Resolve<MaterialAsset>(_assetBank);
					e.AddComponent<MeshRendererComponent>(materialAsset);
				}

				if (components.contains("camera")) {
					auto& cameraData = components["camera"];
					CameraComponent cameraComponent;
					cameraComponent.type = cameraData["projectionType"];
					cameraComponent.orthographicSpec.size = cameraData["orthographicSize"];
					cameraComponent.perspectiveSpec.fov = cameraData["perspectiveFOV"];
					cameraComponent.nearPlane = cameraData["nearPlane"];
					cameraComponent.farPlane = cameraData["farPlane"];
					cameraComponent.clearFlags = cameraData["clearFlags"];
					cameraComponent.priority = cameraData["priority"];
					cameraComponent.backgroundType = cameraData["backgroundType"];
					cameraComponent.backgroundColor = cameraData["backgroundColor"];
					cameraComponent.skyboxExposure = cameraData["skyboxExposure"];

					if (cameraData.contains("backgroundCubemap")) {
						auto id = cameraData["backgroundCubemap"].get<GUID>();
						cameraComponent.skyboxCubemap = AssetRef(id).Resolve<TextureAsset>(_assetBank);
					}

					e.AddComponent<CameraComponent>(cameraComponent);
				}
			}
		}

		std::shared_ptr<Scene> GetInternal() {
			return _internalScene;
		}
	private:
		std::shared_ptr<Scene> _internalScene;
	};
}
