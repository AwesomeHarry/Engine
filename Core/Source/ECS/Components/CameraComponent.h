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
		float left;
		float right;
		float bottom;
		float top;
	};

	struct PerspectiveCamera {
		float fovY;
		float aspectRatio;
	};

	struct CameraComponent : public BaseComponent {
		CameraType type;
		std::variant<OrthographicCamera, PerspectiveCamera> camera;
		float nearPlane;
		float farPlane;
		bool isMainCamera = true;

		CameraComponent()
			: type(CameraType::Orthographic), camera(OrthographicCamera{ -1.0f, 1.0f, -1.0f, 1.0f }), nearPlane(0.1f), farPlane(100.0f) {}

		void SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
			type = CameraType::Orthographic;
			camera = OrthographicCamera{ left, right, bottom, top };
		}

		void SetPerspective(float fovY, float aspectRatio, float nearPlane, float farPlane) {
			type = CameraType::Perspective;
			camera = PerspectiveCamera{ fovY, aspectRatio };
		}

		glm::mat4 GetProjectionMatrix() const {
			if (type == CameraType::Orthographic) {
				const auto& ortho = std::get<OrthographicCamera>(camera);
				return glm::ortho(ortho.left, ortho.right, ortho.bottom, ortho.top, nearPlane, farPlane);
			}
			else if (type == CameraType::Perspective) {
				const auto& persp = std::get<PerspectiveCamera>(camera);
				return glm::perspective(glm::radians(persp.fovY), persp.aspectRatio, nearPlane, farPlane);
			}
			return glm::mat4(1.0f);
		}

		glm::mat4 CalculateViewMatrix(const TransformComponent& transform) const {
			glm::vec3 radians = glm::radians(transform.rotation);
			glm::quat orientation = glm::quat(radians);
			glm::mat4 view = glm::mat4_cast(glm::conjugate(orientation));
			view = glm::translate(view, transform.position);
			return view;
		}
	};
}