#include "Scene/Components/Native/Components.h"
#include "Util/Serializers/MathSerializer.h"
#include "Util/Serializers/FramebufferSerializer.h"
#include "Util/Serializers/FlagSetSerializer.h"

using namespace Engine;

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
}