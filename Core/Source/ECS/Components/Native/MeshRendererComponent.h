#pragma once
#include <memory>

#include "BaseComponent.h"
#include "Rendering/Platform/Material.h"
#include "Rendering/Platform/Mesh.h"

namespace Engine {
	class Entity;

	struct MeshRendererComponent : public BaseComponent {
		std::shared_ptr<Material> sharedMaterial;
		std::shared_ptr<MaterialInstance> materialInstance;

		MeshRendererComponent() {}
		MeshRendererComponent(const std::shared_ptr<Material>& material)
			: sharedMaterial(material), materialInstance(material->CreateInstance()) {}

		// Check to make sure entity also has mesh filter component
		virtual void OnComponentAdded(Entity& entity) override;
	};
}