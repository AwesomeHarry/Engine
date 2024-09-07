#pragma once

#include <entt/entt.hpp>

#include "Scene/Asset.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include "Scene/Components/Native/Components.h"
#include "Util/Serializers/ComponentSerializer.h"

namespace Engine {
	class SceneAsset : public Asset<Scene> {
	public:
		SceneAsset(AssetBank& assetBank, uint32_t id)
			: Asset(assetBank, id, "", AssetType::Scene) {}

		virtual nlohmann::json Serialize() const override {
			nlohmann::json json = Asset::Serialize();

			// Serialize entities
			json["entities"] = nlohmann::json::array();
			const auto& registry = _instance->GetRegistry();
			auto allEntities = registry.view<entt::entity>(entt::exclude<DebugShapeManager>);
			for (auto entity : allEntities) {
				// Serialize entity
				Entity e(entity, _instance.get());

				nlohmann::json entityJson = nlohmann::json::object();
				entityJson["name"] = e.GetName();
				entityJson["transform"] = e.GetTransform();

				if (e.HasComponent<MeshFilterComponent>()) {
					const auto& meshFilter = e.GetComponent<MeshFilterComponent>();
					entityJson["meshFilter"] = meshFilter.meshAsset->GetId();
				}

				if (e.HasComponent<MeshRendererComponent>()) {
					const auto& meshRenderer = e.GetComponent<MeshRendererComponent>();
					nlohmann::json meshRendererJson = nlohmann::json::object();
					meshRendererJson["materialId"] = meshRenderer.materialAsset->GetId();
					// Serialize material instance
					auto overridenUniformNames = meshRenderer.materialInstance->GetOverriddenUniformNames();
					for (const auto& name : overridenUniformNames) {
						meshRendererJson["materialInstance"]["overridenUniforms"][name] = meshRenderer.materialInstance->GetUniformValue(name).value();
					}
					auto overridenTextureNames = meshRenderer.materialInstance->GetOverriddenTextureNames();
					for (const auto& name : overridenTextureNames) {
						meshRendererJson["materialInstance"]["overridenTextures"][name] = meshRenderer.materialInstance->GetTextureRef(name).GetId();
					}
					entityJson["meshRenderer"] = meshRendererJson;
				}

				if (e.HasComponent<CameraComponent>()) {
					const auto& cameraComponent = e.GetComponent<CameraComponent>();
					nlohmann::json cameraJson = nlohmann::json::object();

					// Projection
					cameraJson["projectionType"] = cameraComponent.type;
					cameraJson["size"] = cameraComponent.orthographicSpec.size;
					cameraJson["fov"] = cameraComponent.perspectiveSpec.fov;
					cameraJson["nearPlane"] = cameraComponent.nearPlane;
					cameraJson["farPlane"] = cameraComponent.farPlane;

					// Rendering
					cameraJson["clearFlags"] = cameraComponent.clearFlags;
					cameraJson["priority"] = cameraComponent.priority;

					// Environment
					cameraJson["backgroundType"] = cameraComponent.backgroundType;
					cameraJson["backgroundColor"] = cameraComponent.backgroundColor;
					if (cameraComponent.skyboxCubemapAsset)
						cameraJson["skyboxCubemap"] = cameraComponent.skyboxCubemapAsset->GetId();
					cameraJson["skyboxExposure"] = cameraComponent.skyboxExposure;

					// Output
					cameraJson["renderTarget"] = cameraComponent.renderTarget;
				}

				json["entities"].push_back(entityJson);
			}

			return json;
		}

		virtual void Deserialize(const nlohmann::json& json) override {
			Asset::Deserialize(json);

			// Deserialize entities
			const auto& entitiesJson = json.at("entities");
			for (const auto& entityJson : entitiesJson) {
				auto e = _registry.create();
				auto name = entityJson.at("name").get<std::string>();
				_registry.emplace<NameComponent>(e, name);
				auto transform = entityJson.at("transform").get<TransformComponent>();
				_registry.emplace<TransformComponent>(e, transform);
				
				if (entityJson.contains("meshFilter")) {
					auto meshFilterId = entityJson.at("meshFilter").get<uint32_t>();
					_registry.emplace<MeshFilterComponent>(e, _assetBank.GetAsset<MeshAsset>(meshFilterId));
				}
			}
		}

	protected:
		std::shared_ptr<Scene> CreateInstance() override {
			return std::make_shared<Scene>();
		}
	private:
		entt::registry _registry;
	};
}
