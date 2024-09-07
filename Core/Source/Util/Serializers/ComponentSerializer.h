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
}