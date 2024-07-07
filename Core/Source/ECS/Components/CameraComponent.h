#pragma once
#include "BaseComponent.h"

#include <variant>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TransformComponent.h"

namespace Engine {
	enum class CameraType { Orthographic, Perspective };

	struct OrthographicCamera {
		float size = 1.0f;
	};

	struct PerspectiveCamera {
		float fov = 30.0f;
	};

	struct CameraComponent : public BaseComponent {
		CameraType type = CameraType::Orthographic;

		OrthographicCamera orthographicSpec{};
		PerspectiveCamera perspectiveSpec{};

		float nearPlane = 0.1f;
		float farPlane = 100.0f;
		bool isMainCamera = true;

		glm::vec3 backgroundColor = glm::vec3(0.06f, 0.06f, 0.06f);

		CameraComponent() {}

		glm::mat4 GetProjectionMatrix(float aspectRatio) const {
			switch (type) {
			case CameraType::Orthographic:
			{
				float orthoHeight = orthographicSpec.size;
				float orthoWidth = orthographicSpec.size * aspectRatio;
				return glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, nearPlane, farPlane);
			}
			case CameraType::Perspective:
				return glm::perspective(glm::radians(perspectiveSpec.fov), aspectRatio, nearPlane, farPlane);
			}

			return glm::mat4(1.0f);
		}

		glm::mat4 CalculateViewMatrix(const TransformComponent& transform) const {
			glm::vec3 radians = glm::radians(transform.rotation);
			glm::quat orientation = glm::quat(radians);
			glm::mat4 view = glm::mat4_cast(glm::conjugate(orientation));
			view = glm::translate(view, -transform.position);
			return view;
		}
	};
}