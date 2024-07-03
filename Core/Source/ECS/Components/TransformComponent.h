#pragma once
#include "BaseComponent.h"
#include <glm/vec3.hpp>

namespace Engine {
	struct TransformComponent : public BaseComponent {
		glm::vec3 position = { 0.0f,0.0f,0.0f };
		glm::vec3 rotation = { 0.0f,0.0f,0.0f };
		glm::vec3 scale = { 1.0f,1.0f,1.0f };

		TransformComponent() = default;
		TransformComponent(const glm::vec3& position)
			: position(position) {}
	};
}