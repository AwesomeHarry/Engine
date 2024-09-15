#pragma once

#include "BaseComponent.h"
#include "Project/Assets/MeshAsset.h"

namespace Engine {
	class Entity;

	struct MeshFilterComponent : public BaseComponent {
		std::shared_ptr<MeshAsset> meshAsset = nullptr;

		MeshFilterComponent() {}
		MeshFilterComponent(const std::shared_ptr<MeshAsset>& meshAsset)
			: meshAsset(meshAsset) {}
	};
}