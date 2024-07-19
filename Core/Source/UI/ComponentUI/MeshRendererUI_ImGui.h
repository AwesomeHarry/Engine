#pragma once
#include <imgui.h>
#include "ECS/Components/Native/MeshRendererComponent.h"

namespace Engine {
	class MeshRendererUI_ImGui {
	public:
		static void RenderUI(MeshRendererComponent& meshRenderer) {
			if (ImGui::TreeNodeEx("Mesh Renderer", ImGuiTreeNodeFlags_Framed)) {
				auto& material = *meshRenderer.material;
				auto& shader = material.GetShader();
				ImGui::Text("Uniforms:");

				Engine::MaterialUtils::DrawAllUniformWidgets(material);

				ImGui::Separator();

				ImGui::Text("Uniform Blocks:");
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