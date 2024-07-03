#pragma once
#include <imgui.h>
#include "ECS/Scene/Entity.h"
#include "ComponentUI/MeshFilterUI_ImGui.h"
#include "ComponentUI/MeshRendererUI_ImGui.h"
#include "ComponentUI/TransformUI_ImGui.h"
#include "ComponentUI/CameraUI_ImGui.h"

namespace Engine {
	float v = 0;
	class EntityPropertiesUI_ImGui {
	public:

		static void RenderUI(Entity entity) {
			if (!entity)
				return;

			const std::string& name = entity.GetName();
			ImGui::Text("Entity Name: %s", name.c_str());

			TransformComponent& transform = entity.GetTransform();
			TransformUI_ImGui::RenderUI(transform);

			if (entity.HasComponent<MeshFilterComponent>()) {
				auto& meshFilter = entity.GetComponent<MeshFilterComponent>();
				MeshFilterUI_ImGui::RenderUI(meshFilter);
			}

			if (entity.HasComponent<MeshRendererComponent>()) {
				auto& meshRenderer = entity.GetComponent<MeshRendererComponent>();
				MeshRendererUI_ImGui::RenderUI(meshRenderer);
			}

			if (entity.HasComponent<CameraComponent>()) {
				auto& camera = entity.GetComponent<CameraComponent>();
				CameraUI_ImGui::RenderUI(camera);
			}
		}
	};
}