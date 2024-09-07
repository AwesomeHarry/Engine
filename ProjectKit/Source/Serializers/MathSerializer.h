#include <json.hpp>
#include <glm/glm.hpp>

namespace nlohmann {
	template<>
	struct adl_serializer<glm::vec2> {
		static void to_json(json& j, const glm::vec2& vec) {
			j = json{ vec.x, vec.y };
		}

		static void from_json(const json& j, glm::vec2& vec) {
			j.at(0).get_to(vec.x);
			j.at(1).get_to(vec.y);
		}
	};

	template<>
	struct adl_serializer<glm::vec3> {
		static void to_json(json& j, const glm::vec3& vec) {
			j = json{ vec.x,vec.y,vec.z };
		}

		static void from_json(const json& j, glm::vec3& vec) {
			j.at(0).get_to(vec.x);
			j.at(1).get_to(vec.y);
			j.at(2).get_to(vec.z);
		}
	};

	template<>
	struct adl_serializer<glm::vec4> {
		static void to_json(json& j, const glm::vec4& vec) {
			j = json{ vec.x,vec.y,vec.z,vec.w };
		}

		static void from_json(const json& j, glm::vec4& vec) {
			j.at(0).get_to(vec.x);
			j.at(1).get_to(vec.y);
			j.at(2).get_to(vec.z);
			j.at(3).get_to(vec.w);
		}
	};

	template<>
	struct adl_serializer<glm::mat2> {
		static void to_json(json& j, const glm::mat2& mat) {
			j = json{ mat[0][0],mat[0][1],mat[1][0],mat[1][1] };
		}

		static void from_json(const json& j, glm::mat2& mat) {
			j.at(0).get_to(mat[0][0]);
			j.at(1).get_to(mat[0][1]);
			j.at(2).get_to(mat[1][0]);
			j.at(3).get_to(mat[1][1]);
		}
	};

	template<>
	struct adl_serializer<glm::mat3> {
		static void to_json(json& j, const glm::mat3& mat) {
			j = json{ mat[0][0],mat[0][1],mat[0][2],mat[1][0],mat[1][1],mat[1][2],mat[2][0],mat[2][1],mat[2][2] };
		}

		static void from_json(const json& j, glm::mat3& mat) {
			j.at(0).get_to(mat[0][0]);
			j.at(1).get_to(mat[0][1]);
			j.at(2).get_to(mat[0][2]);
			j.at(3).get_to(mat[1][0]);
			j.at(4).get_to(mat[1][1]);
			j.at(5).get_to(mat[1][2]);
			j.at(6).get_to(mat[2][0]);
			j.at(7).get_to(mat[2][1]);
			j.at(8).get_to(mat[2][2]);
		}
	};

	template<>
	struct adl_serializer<glm::mat4> {
		static void to_json(json& j, const glm::mat4& mat) {
			j = json{ mat[0][0],mat[0][1],mat[0][2],mat[0][3],mat[1][0],mat[1][1],mat[1][2],mat[1][3],mat[2][0],mat[2][1],mat[2][2],mat[2][3],mat[3][0],mat[3][1],mat[3][2],mat[3][3] };
		}

		static void from_json(const json& j, glm::mat4& mat) {
			j.at(0).get_to(mat[0][0]);
			j.at(1).get_to(mat[0][1]);
			j.at(2).get_to(mat[0][2]);
			j.at(3).get_to(mat[0][3]);
			j.at(4).get_to(mat[1][0]);
			j.at(5).get_to(mat[1][1]);
			j.at(6).get_to(mat[1][2]);
			j.at(7).get_to(mat[1][3]);
			j.at(8).get_to(mat[2][0]);
			j.at(9).get_to(mat[2][1]);
			j.at(10).get_to(mat[2][2]);
			j.at(11).get_to(mat[2][3]);
			j.at(12).get_to(mat[3][0]);
			j.at(13).get_to(mat[3][1]);
			j.at(14).get_to(mat[3][2]);
			j.at(15).get_to(mat[3][3]);
		}
	};
}