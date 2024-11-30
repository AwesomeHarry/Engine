#pragma once
#include <imgui.h>
#include "Project/Scene/Components/Native/MeshRendererComponent.h"
#include "UI/MaterialUI_ImGui.h"

namespace Engine {
	std::string TextureTypeToString(TextureType type) {
		switch (type) {
		case TextureType::Tex2D: return "Texture2D";
		case TextureType::TexCubemap: return "TextureCubemap";
		}

		return "Unknown";
	}

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

				ImGui::Separator();

				ImGui::Text("Textures:");
				auto& textureNames = material.GetTextureNames();
				for (auto& textureName : textureNames) {
					auto& uniformTexture = material.GetTexture(textureName);
					auto& texture = *uniformTexture.texture;
					auto textureType = texture.GetType();

					ImGui::Text(textureName.c_str());
					ImGui::SameLine();
					ImGui::Text("%d", uniformTexture.bindingPoint);
					ImGui::SameLine();
					ImGui::Text("Type: %s", TextureTypeToString(textureType).c_str());

					if (textureType == TextureType::Tex2D)
						ImGui::Image((void*)(intptr_t)texture.GetInstanceID(), ImVec2(64, 64));
				}

				ImGui::TreePop();
			}
		}
	};
}