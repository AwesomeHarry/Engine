#pragma once

#include "BaseComponent.h"
#include "Rendering/Platform/Mesh.h"

namespace Engine {
	class Entity;

	struct MeshFilterComponent : public BaseComponent {
		std::shared_ptr<Mesh> mesh;

		MeshFilterComponent() {}
		MeshFilterComponent(const std::shared_ptr<Mesh>& mesh)
			: mesh(mesh) {}
	};
}