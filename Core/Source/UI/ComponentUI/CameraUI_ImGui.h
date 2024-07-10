#pragma once
#include <imgui.h>
#include "ECS/Components/Native/CameraComponent.h"
#include <string>

namespace Engine {
	std::string CameraTypeToString(CameraType type) {
		switch (type) {
		case CameraType::Orthographic: return "Orthographic";
		case CameraType::Perspective: return "Perspective";
		default: return "Unknown";
		}
	}

	std::string BufferBitToString(BufferBit bit) {
		switch (bit) {
		case BufferBit::Color: return "Color";
		case BufferBit::Depth: return "Depth";
		case BufferBit::Stencil: return "Stencil";
		default: return "Unknown";
		}
	}

	std::string RenderTargetToString(CameraComponent::RenderTarget target) {
		switch (target) {
		case CameraComponent::RenderTarget::Window: return "Window";
		case CameraComponent::RenderTarget::Texture: return "Texture";
		default: return "Unknown";
		}
	}

	class CameraUI_ImGui {
	public:
		static void RenderUI(CameraComponent& camera) {
			if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_Framed)) {
				if (ImGui::CollapsingHeader("Projection")) {
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
				}

				if (ImGui::CollapsingHeader("Rendering")) {
					//ImGui::Text("Render Pipeline: %s", camera.renderPipeline ? camera.renderPipeline->GetName().c_str() : "None");
					ImGui::Text("Clear Flags: %s", camera.clearFlags.size() == 0 ? "None" : "");
					for (auto& flag : camera.clearFlags) {
						ImGui::Text("  %s", BufferBitToString(flag));
					}
					ImGui::DragFloat("Priority", &camera.priority, 0.1f);
				}

				if (ImGui::CollapsingHeader("Environment")) {
					//ImGui::Text("Background Type: %s", BackgroundTypeToString(camera.backgroundType));
					ImGui::ColorEdit3("Background Color", &camera.backgroundColor[0]);
				}

				if (ImGui::CollapsingHeader("Output")) {
					ImGui::Text("Render Target: %s", RenderTargetToString(camera.renderTarget));
					//ImGui::Text("Render Framebuffer: %s", camera.renderFramebuffer ? camera.renderFramebuffer->GetName().c_str() : "None");
				}

				ImGui::TreePop();
			}
		}
	};
}