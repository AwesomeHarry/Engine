#pragma once
#include <imgui.h>
#include "ECS/Components/Native/MeshRendererComponent.h"
#include "UI/MaterialUI_ImGui.h"

namespace Engine {
	class MeshRendererUI_ImGui {
	public:
		static void RenderUI(MeshRendererComponent& meshRenderer) {
			if (ImGui::TreeNodeEx("Mesh Renderer", ImGuiTreeNodeFlags_Framed)) {
				auto& materialInstance = *meshRenderer.materialInstance;
				ImGui::Text("Uniforms:");

				Engine::UI::MaterialUI::DrawAllUniformWidgets(materialInstance);

				ImGui::Separator();

				ImGui::Text("Uniform Blocks:");
				auto& shader = materialInstance.GetShader();
				auto& uniformBlocks = shader.GetUniformBlocks();
				for (auto& [name, location] : uniformBlocks) {
					ImGui::Text(name.c_str());
					ImGui::SameLine();
					ImGui::Text("%d", location);
				}

				ImGui::TreePop();
			}
		}
	};
}