#pragma once

#include "BaseComponent.h"
#include "Scene/Assets/MeshAsset.h"

namespace Engine {
	class Entity;

	struct MeshFilterComponent : public BaseComponent {
		std::shared_ptr<MeshAsset> meshAsset;

		MeshFilterComponent() {}
		MeshFilterComponent(const std::shared_ptr<MeshAsset>& meshAsset)
			: meshAsset(meshAsset) {}
	};
}