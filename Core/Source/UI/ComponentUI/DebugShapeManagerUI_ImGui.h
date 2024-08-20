#pragma once
#include "Scene/Components/Native/DebugShapeManager.h"
#include <imgui.h>

namespace Engine {
	class DebugShapeManagerUI_ImGui {
	public:
		static void RenderUI(DebugShapeManager& dsm) {
			if (ImGui::TreeNodeEx("Debug Shape Manager", ImGuiTreeNodeFlags_Framed)) {
				ImGui::Checkbox("Render Debug Shapes", &dsm.renderDebugShapes);
				ImGui::TreePop();
			}
		}
	};
}