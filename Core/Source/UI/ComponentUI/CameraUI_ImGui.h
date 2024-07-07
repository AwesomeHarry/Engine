#pragma once
#include <imgui.h>
#include "ECS/Components/CameraComponent.h"
#include <string>

namespace Engine {
	std::string CameraTypeToString(CameraType type) {
		switch (type) {
		case CameraType::Orthographic: return "Orthographic";
		case CameraType::Perspective: return "Perspective";
		default: return "Unknown";
		}
	}

	class CameraUI_ImGui {
	public:
		static void RenderUI(CameraComponent& camera) {
			if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_Framed)) {

				constexpr char* items[] = { "Orthographic", "Perspective" };
				int currentItemIndex = (int)camera.type;
				if (ImGui::Combo("Camera Type", &currentItemIndex, items, IM_ARRAYSIZE(items)))
					camera.type = (CameraType)currentItemIndex;

				if (camera.type == CameraType::Orthographic) {
					auto& camSpec = camera.orthographicSpec;
					ImGui::DragFloat("Size", &camSpec.size, 0.05f);
				}
				else if (camera.type == CameraType::Perspective) {
					auto& camSpec = camera.perspectiveSpec;
					ImGui::DragFloat("Fov", &camSpec.fov, 0.5f);
				}

				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f);

				ImGui::Checkbox("Is Main Camera", &camera.isMainCamera);

				ImGui::ColorEdit3("Background Color", &camera.backgroundColor[0]);

				ImGui::TreePop();
			}
		}
	};
}