#pragma once

#include <json.hpp>
#include "Serializers/MathSerializer.h"
#include "Rendering/Platform/Material.h"

using namespace Engine;

namespace nlohmann {
	template<>
	struct adl_serializer<UniformValue> {
		static void to_json(json& j, const UniformValue& value) {
			std::visit([&j](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, bool>)
					j["type"] = "bool";
				else if constexpr (std::is_same_v<T, int>)
					j["type"] = "int";
				else if constexpr (std::is_same_v<T, float>)
					j["type"] = "float";
				else if constexpr (std::is_same_v<T, glm::vec2>)
					j["type"] = "vec2";
				else if constexpr (std::is_same_v<T, glm::vec3>)
					j["type"] = "vec3";
				else if constexpr (std::is_same_v<T, glm::vec4>)
					j["type"] = "vec4";
				else if constexpr (std::is_same_v<T, glm::mat2>)
					j["type"] = "mat2";
				else if constexpr (std::is_same_v<T, glm::mat3>)
					j["type"] = "mat3";
				else if constexpr (std::is_same_v<T, glm::mat4>)
					j["type"] = "mat4";

				j["value"] = arg;

			}, value);
		}

		static void from_json(const json& j, UniformValue& value) {
			std::string type = j.at("type").get<std::string>();
			if (type == "bool")
				value = j.at("value").get<bool>();
			else if (type == "int")
				value = j.at("value").get<int>();
			else if (type == "float")
				value = j.at("value").get<float>();
			else if (type == "vec2")
				value = j.at("value").get<glm::vec2>();
			else if (type == "vec3")
				value = j.at("value").get<glm::vec3>();
			else if (type == "vec4")
				value = j.at("value").get<glm::vec4>();
			else if (type == "mat2")
				value = j.at("value").get<glm::mat2>();
			else if (type == "mat3")
				value = j.at("value").get<glm::mat3>();
			else if (type == "mat4")
				value = j.at("value").get<glm::mat4>();
		}
	};
}