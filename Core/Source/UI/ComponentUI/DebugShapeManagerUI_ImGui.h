#pragma once
#include "ECS/Components/DebugShapeManager.h"
#include <imgui.h>

namespace Engine {
	class DebugShapeManagerUI_ImGui {
	public:
		static void RenderUI(DebugShapeManager& dsm) {
			if (ImGui::TreeNodeEx("Debug Shape Manager", ImGuiTreeNodeFlags_Framed)) {
				for (const auto& call : dsm.drawCalls) {
					ImGui::Text("{}", call.first->GetName().c_str());
				}
				ImGui::TreePop();
			}
		}
	};
}