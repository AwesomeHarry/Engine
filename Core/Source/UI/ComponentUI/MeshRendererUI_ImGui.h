#pragma once
#include <imgui.h>
#include "ECS/Components/MeshRendererComponent.h"

namespace Engine {
	class MeshRendererUI_ImGui {
	public:
		static void RenderUI(MeshRendererComponent& meshRenderer) {
			if (ImGui::TreeNodeEx("Mesh Renderer", ImGuiTreeNodeFlags_Framed)) {
				auto& shader = meshRenderer.shader;
				ImGui::Text("Uniforms:");

				auto& uniforms = shader->GetUniforms();
				for (auto& [name, location] : uniforms) {
					ImGui::Text(name.c_str());
					ImGui::SameLine();
					ImGui::Text("%d", location);
				}

				ImGui::Text("Uniform Blocks:");
				auto& uniformBlocks = shader->GetUniformBlocks();
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