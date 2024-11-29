#pragma once
#include <imgui.h>
#include "Project/Scene/Components/Native/MeshRendererComponent.h"
#include "UI/MaterialUI_ImGui.h"

namespace Engine {
	class MeshRendererUI_ImGui {
	public:
		static void RenderUI(MeshRendererComponent& meshRenderer) {
			if (ImGui::TreeNodeEx("Mesh Renderer", ImGuiTreeNodeFlags_Framed)) {
				auto& materialAsset = *meshRenderer.materialAsset;

				if (!materialAsset.IsLoaded()) {
					ImGui::Text("Material not loaded.");
					return;
				}

				auto& material = *materialAsset.GetInternal();

				ImGui::Text("Uniforms:");

				Engine::UI::MaterialUI::DrawAllUniformWidgets(material);

				ImGui::Separator();

				ImGui::Text("Uniform Blocks:");
				auto& shader = material.GetShader();
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