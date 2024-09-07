#pragma once
#include <memory>

#include "BaseComponent.h"
#include "Scene/Assets/MaterialAsset.h"

namespace Engine {
	class Entity;

	struct MeshRendererComponent : public BaseComponent {
		std::shared_ptr<MaterialAsset> materialAsset;
		std::shared_ptr<MaterialAssetInstanceOverride> materialInstance;

		MeshRendererComponent() {}
		MeshRendererComponent(const std::shared_ptr<MaterialAsset>& materialAsset)
			: materialAsset(materialAsset), 
			materialInstance(std::dynamic_pointer_cast<MaterialAssetInstanceOverride>(materialAsset->GetInstance()->CreateInstance())) {}

		// Check to make sure entity also has mesh filter component
		virtual void OnComponentAdded(Entity& entity) override;
	};
}
