#pragma once
#include <imgui.h>
#include "Project/Scene/Components/Native/TransformComponent.h"

namespace Engine {
	class TransformUI_ImGui {
	public:
		static void RenderUI(TransformComponent& transform) {
			if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {

				ImGui::DragFloat3("Position", &transform.position[0], 0.1f);
				ImGui::DragFloat3("Rotation", &transform.rotation[0], 0.1f);
				ImGui::DragFloat3("Scale", &transform.scale[0], 0.1f);

				ImGui::TreePop();
			}
		}
	};
}