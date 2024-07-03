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

				const char* items[] = { "Orthographic", "Perspective" };
				int currentItemIndex = (int)camera.type;
				if (ImGui::Combo("Camera Type", &currentItemIndex, items, IM_ARRAYSIZE(items))) {
					auto newType = (CameraType)currentItemIndex;
					if (newType != camera.type) {
						camera.type = newType;
						if (newType == CameraType::Orthographic)
							camera.SetOrthographic(-1, 1, -1, 1, 0.1f, 100.0f);
						else if (newType == CameraType::Perspective)
							camera.SetPerspective(30.0f, 1.0f, 0.1f, 100.0f);
					}
				}

				if (camera.type == CameraType::Orthographic) {
					auto& camSpec = std::get<OrthographicCamera>(camera.camera);
					ImGui::DragFloat("Left", &camSpec.left, 0.05f);
					ImGui::DragFloat("Right", &camSpec.right, 0.05f);
					ImGui::DragFloat("Top", &camSpec.top, 0.05f);
					ImGui::DragFloat("Bottom", &camSpec.bottom, 0.05f);
				}
				else if (camera.type == CameraType::Perspective) {
					auto& camSpec = std::get<PerspectiveCamera>(camera.camera);
					ImGui::DragFloat("Fov", &camSpec.fovY, 0.5f);
					ImGui::DragFloat("Aspect Ration", &camSpec.aspectRatio, 0.1f);
				}

				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f);

				ImGui::Checkbox("Is Main Camera", &camera.isMainCamera);

				ImGui::TreePop();
			}
		}
	};
}