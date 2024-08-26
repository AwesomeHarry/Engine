#pragma once
#include "BaseComponent.h"

#include <memory>
#include <Util/FlagSet.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Rendering/BaseRenderPipeline.h"
#include "Rendering/BufferBit.h"
#include "Rendering/Platform/Framebuffer.h"
#include "Rendering/Platform/Texture2D.h"
#include "Rendering/Platform/TextureCubemap.h"

#include "TransformComponent.h"

namespace Engine {

	enum class CameraType { Orthographic, Perspective };
	struct OrthographicCamera { float size = 1.0f; };
	struct PerspectiveCamera { float fov = 45.0f; };

	struct CameraComponent : public BaseComponent {
	#pragma region Projection
		CameraType type = CameraType::Orthographic;
		OrthographicCamera orthographicSpec{};
		PerspectiveCamera perspectiveSpec{};
		float nearPlane = 0.1f;
		float farPlane = 100.0f;
	#pragma endregion

	#pragma region Rendering
		std::shared_ptr<BaseRenderPipeline> renderPipeline = nullptr;
		FlagSet<BufferBit> clearFlags = BufferBit::Color | BufferBit::Depth;
		float priority = 0.0f;
	#pragma endregion

	#pragma region Environment
		enum class BackgroundType { Color, Skybox };
		BackgroundType backgroundType = BackgroundType::Color;
		glm::vec3 backgroundColor = glm::vec3(0.06f, 0.06f, 0.06f);
		std::shared_ptr<TextureCubemap> skyboxCubemap = nullptr;
		float skyboxExposure = 1.0f;
	#pragma endregion

	#pragma region Output
		enum class RenderTarget { Window, Texture };
		RenderTarget renderTarget = RenderTarget::Window;
		std::shared_ptr<Framebuffer> renderFramebuffer = nullptr;
	#pragma endregion

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
		};

		glm::mat4 CalculateViewMatrix(const TransformComponent& transform) const {
			glm::mat4 rotationMatrix = glm::eulerAngleYXZ(-glm::radians(transform.rotation.y), -glm::radians(transform.rotation.x), glm::radians(transform.rotation.z));
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform.position);
			glm::mat4 view = glm::inverse(translationMatrix * rotationMatrix);
			return view;
		}
	};
}
