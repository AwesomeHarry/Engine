#pragma once
#include <imgui.h>
#include "Project/Scene/Entity.h"
#include "Project/Scene/Components/Native/Components.h"
#include "ComponentUI/MeshFilterUI_ImGui.h"
#include "ComponentUI/MeshRendererUI_ImGui.h"
#include "ComponentUI/TransformUI_ImGui.h"
#include "ComponentUI/CameraUI_ImGui.h"
#include "ComponentUI/DebugShapeManagerUI_ImGui.h"

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

			if (entity.HasComponent<DebugShapeManager>()) {
				auto& dsm = entity.GetComponent<DebugShapeManager>();
				DebugShapeManagerUI_ImGui::RenderUI(dsm);
			}

			if (entity.HasComponent<BoundingBoxComponent>()) {
				auto& bb = entity.GetComponent<BoundingBoxComponent>();
				if (ImGui::TreeNodeEx("Bounding Box", ImGuiTreeNodeFlags_Framed)) {
					ImGui::Text("Center: (%f, %f, %f)", bb.GetCenter().x, bb.GetCenter().y, bb.GetCenter().z);
					ImGui::Text("Size: (%f, %f, %f)", bb.GetSize().x, bb.GetSize().y, bb.GetSize().z);
					ImGui::Text("Min: (%f, %f, %f)", bb.min.x, bb.min.y, bb.min.z);
					ImGui::Text("Max: (%f, %f, %f)", bb.max.x, bb.max.y, bb.max.z);
					ImGui::TreePop();
				}
			}
		}
	};
}
