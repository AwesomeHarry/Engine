#pragma once

#include "Project/Scene/Components/Native/Components.h"
#include "Serializers/MathSerializer.h"
#include "Serializers/FramebufferSerializer.h"
#include "Serializers/FlagSetSerializer.h"

using namespace Engine;

namespace nlohmann {
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
	struct adl_serializer<MeshFilterComponent> {
		static void to_json(json& j, const MeshFilterComponent& component) {
			j = json{
				{"id", component.meshAsset->GetGUID()}
			};
		}
	};

	template<>
	struct adl_serializer<MeshRendererComponent> {
		static void to_json(json& j, const MeshRendererComponent& component) {
			j = json{
				{"material", component.materialAsset->GetGUID()}
			};
		}
	};

	template<>
	struct adl_serializer<CameraComponent> {
		static void to_json(json& j, const CameraComponent& component) {
			j = json{
				{"projectionType", component.type},
				{"orthographicSize", component.orthographicSpec.size},
				{"perspectiveFOV", component.perspectiveSpec.fov},
				{"nearPlane", component.nearPlane},
				{"farPlane", component.farPlane},

				{"clearFlags", component.clearFlags},
				{"priority", component.priority},

				{"backgroundType", component.backgroundType},
				{"backgroundColor", component.backgroundColor},
				{"skyboxExposure", component.skyboxExposure},
			};

			if (component.skyboxCubemap)
				j["backgroundCubemap"] = component.skyboxCubemap->GetGUID();
			else
				j["backgroundCubemap"] = 0;
		}
	};
}