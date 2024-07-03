#pragma once
#include <memory>

#include "BaseComponent.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"

namespace Engine {
	class Entity;

	struct MeshRendererComponent : public BaseComponent {
		std::shared_ptr<Shader> shader;

		MeshRendererComponent() {}
		MeshRendererComponent(const std::shared_ptr<Shader>& shader)
			: shader(shader) {}

		// Check to make sure entity also has mesh filter component
		virtual void OnComponentAdded(Entity& entity) override;
	};
}