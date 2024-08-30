#include <json.hpp>
#include <string>

#include "Util/Mesh/GltfIO.h"
#include "Scene/Components/Native/Components.h"

using namespace Engine;

namespace Engine {
	NLOHMANN_JSON_SERIALIZE_ENUM(ShaderStage, {
		{ShaderStage::Vertex, "Vertex"},
		{ShaderStage::Fragment, "Fragment"},
		{ShaderStage::Geometry, "Geometry"}
								 });
}

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
	struct adl_serializer<glm::vec2> {
		static void to_json(json& j, const glm::vec2& vec) {
			j = json{
				{"x",vec.x},
				{"y",vec.y}
			};
		}

		static void from_json(const json& j, glm::vec2& vec) {
			j.at("x").get_to(vec.x);
			j.at("y").get_to(vec.y);
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
	struct adl_serializer<glm::vec4> {
		static void to_json(json& j, const glm::vec4& vec) {
			j = json{
				{"x",vec.x},
				{"y",vec.y},
				{"z",vec.z},
				{"w",vec.w}
			};
		}

		static void from_json(const json& j, glm::vec4& vec) {
			j.at("x").get_to(vec.x);
			j.at("y").get_to(vec.y);
			j.at("z").get_to(vec.z);
			j.at("w").get_to(vec.w);
		}
	};

	template<>
	struct adl_serializer<glm::mat2> {
		static void to_json(json& j, const glm::mat2& mat) {
			j = json{
				{"m00",mat[0][0]},
				{"m01",mat[0][1]},
				{"m10",mat[1][0]},
				{"m11",mat[1][1]}
			};
		}

		static void from_json(const json& j, glm::mat2& mat) {
			j.at("m00").get_to(mat[0][0]);
			j.at("m01").get_to(mat[0][1]);
			j.at("m10").get_to(mat[1][0]);
			j.at("m11").get_to(mat[1][1]);
		}
	};

	template<>
	struct adl_serializer<glm::mat3> {
		static void to_json(json& j, const glm::mat3& mat) {
			j = json{
				{"m00",mat[0][0]},
				{"m01",mat[0][1]},
				{"m02",mat[0][2]},
				{"m10",mat[1][0]},
				{"m11",mat[1][1]},
				{"m12",mat[1][2]},
				{"m20",mat[2][0]},
				{"m21",mat[2][1]},
				{"m22",mat[2][2]}
			};
		}

		static void from_json(const json& j, glm::mat3& mat) {
			j.at("m00").get_to(mat[0][0]);
			j.at("m01").get_to(mat[0][1]);
			j.at("m02").get_to(mat[0][2]);
			j.at("m10").get_to(mat[1][0]);
			j.at("m11").get_to(mat[1][1]);
			j.at("m12").get_to(mat[1][2]);
			j.at("m20").get_to(mat[2][0]);
			j.at("m21").get_to(mat[2][1]);
			j.at("m22").get_to(mat[2][2]);
		}
	};

	template<>
	struct adl_serializer<glm::mat4> {
		static void to_json(json& j, const glm::mat4& mat) {
			j = json{
				{"m00",mat[0][0]},
				{"m01",mat[0][1]},
				{"m02",mat[0][2]},
				{"m03",mat[0][3]},
				{"m10",mat[1][0]},
				{"m11",mat[1][1]},
				{"m12",mat[1][2]},
				{"m13",mat[1][3]},
				{"m20",mat[2][0]},
				{"m21",mat[2][1]},
				{"m22",mat[2][2]},
				{"m23",mat[2][3]},
				{"m30",mat[3][0]},
				{"m31",mat[3][1]},
				{"m32",mat[3][2]},
				{"m33",mat[3][3]}
			};
		}

		static void from_json(const json& j, glm::mat4& mat) {
			j.at("m00").get_to(mat[0][0]);
			j.at("m01").get_to(mat[0][1]);
			j.at("m02").get_to(mat[0][2]);
			j.at("m03").get_to(mat[0][3]);
			j.at("m10").get_to(mat[1][0]);
			j.at("m11").get_to(mat[1][1]);
			j.at("m12").get_to(mat[1][2]);
			j.at("m13").get_to(mat[1][3]);
			j.at("m20").get_to(mat[2][0]);
			j.at("m21").get_to(mat[2][1]);
			j.at("m22").get_to(mat[2][2]);
			j.at("m23").get_to(mat[2][3]);
			j.at("m30").get_to(mat[3][0]);
			j.at("m31").get_to(mat[3][1]);
			j.at("m32").get_to(mat[3][2]);
			j.at("m33").get_to(mat[3][3]);
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
			for (const auto& attachment : fbSpec.attachments) {
				j["attachments"].push_back(attachment);
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
}
