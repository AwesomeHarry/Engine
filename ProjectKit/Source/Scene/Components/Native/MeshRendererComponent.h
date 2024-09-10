#pragma once
#include <memory>

#include "BaseComponent.h"
#include "Rendering/Platform/Material.h"

namespace Engine {
	class Entity;

	struct MeshRendererComponent : public BaseComponent {
		std::shared_ptr<Material> material;

		MeshRendererComponent() {}
		MeshRendererComponent(const std::shared_ptr<Material>& material)
			: material(material) {}

		// Check to make sure entity also has mesh filter component
		virtual void OnComponentAdded(Entity& entity) override;
	};
}
