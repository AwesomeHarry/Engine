#pragma once
#include <memory>

#include "BaseComponent.h"
#include "Project/Assets/MaterialAsset.h"

namespace Engine {
	class Entity;

	struct MeshRendererComponent : public BaseComponent {
		std::shared_ptr<MaterialAsset> materialAsset;

		MeshRendererComponent() {}
		MeshRendererComponent(const std::shared_ptr<MaterialAsset>& materialAsset)
			: materialAsset(materialAsset) {}

		// Check to make sure entity also has mesh filter component
		virtual void OnComponentAdded(Entity& entity) override;
	};
}
