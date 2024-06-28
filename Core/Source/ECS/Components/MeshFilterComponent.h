#pragma once

#include "BaseComponent.h"
#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"

namespace Engine {
	class Entity;

	struct MeshFilterComponent : public BaseComponent {
		std::shared_ptr<Mesh> mesh;

		MeshFilterComponent() {}
		MeshFilterComponent(const std::shared_ptr<Mesh>& mesh)
			: mesh(mesh) {}
	};

	struct MeshRendererComponent : public BaseComponent {
		std::shared_ptr<Shader> shader;

		MeshRendererComponent() {}
		MeshRendererComponent(const std::shared_ptr<Shader>& shader)
			: shader(shader) {}

		// Check to make sure entity also has mesh filter component
		virtual void OnComponentAdded(Entity& entity) override;

		void RenderMesh(const Mesh& mesh);
	};
}