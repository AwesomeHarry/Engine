#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine {
	struct Transform {
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		const glm::vec3& GetPosition() const { return position; }
		const glm::vec3& GetRotation() const { return rotation; }
		const glm::vec3& GetScale() const { return scale; }

		glm::vec3 GetForwardDirection() const {
			glm::mat4 rotationMatrix = glm::eulerAngleYXZ(-glm::radians(rotation.y), -glm::radians(rotation.x), glm::radians(rotation.z));
			return rotationMatrix * glm::vec4(0, 0, -1, 0);
		}

		glm::vec3 GetRightDirection() const {
			glm::mat4 rotationMatrix = glm::eulerAngleYXZ(-glm::radians(rotation.y), -glm::radians(rotation.x), glm::radians(rotation.z));
			return rotationMatrix * glm::vec4(1, 0, 0, 0);
		}

		glm::vec3 GetUpDirection() const {
			glm::mat4 rotationMatrix = glm::eulerAngleYXZ(-glm::radians(rotation.y), -glm::radians(rotation.x), glm::radians(rotation.z));
			return rotationMatrix * glm::vec4(0, 1, 0, 0);
		}

		glm::mat4 GetTransformMatrix() const {
			glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::radians(rotation)));
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
			return translationMatrix * rotationMatrix * scaleMatrix;
		}
	};
}