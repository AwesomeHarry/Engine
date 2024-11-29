#pragma once
#include <imgui.h>
#include <string>

#include "Project/Scene/Components/Native/CameraComponent.h"

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

	std::string BackgroundTypeToString(CameraComponent::BackgroundType type) {
		switch (type) {
		case CameraComponent::BackgroundType::Color: return "Color";
		case CameraComponent::BackgroundType::Skybox: return "Skybox";
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

					ImGui::Text("Clear Flags:");
					ImGui::Indent();
					bool colorBufferBit = camera.clearFlags[BufferBit::Color];
					if (ImGui::Checkbox("Color", &colorBufferBit))
						camera.clearFlags.set(BufferBit::Color, colorBufferBit);
					bool depthBufferBit = camera.clearFlags[BufferBit::Depth];
					if (ImGui::Checkbox("Depth", &depthBufferBit))
						camera.clearFlags.set(BufferBit::Depth, depthBufferBit);
					bool stencilBufferBit = camera.clearFlags[BufferBit::Stencil];
					if (ImGui::Checkbox("Stencil", &stencilBufferBit))
						camera.clearFlags.set(BufferBit::Stencil, stencilBufferBit);
					ImGui::Unindent();

					ImGui::DragFloat("Priority", &camera.priority, 0.1f);
				}

				if (ImGui::CollapsingHeader("Environment")) {
					ImGui::Text("Background Type: %s", BackgroundTypeToString(camera.backgroundType).c_str());
					if (camera.backgroundType == CameraComponent::BackgroundType::Skybox) {
						//ImGui::Text("Skybox Cubemap: %s", camera.skyboxCubemap ? camera.skyboxCubemap->GetName().c_str() : "None");
						ImGui::DragFloat("Skybox Exposure", &camera.skyboxExposure, 0.1f);
					}
					else if (camera.backgroundType == CameraComponent::BackgroundType::Color)
						ImGui::ColorEdit3("Background Color", &camera.backgroundColor[0]);
				}

				if (ImGui::CollapsingHeader("Output")) {
					ImGui::Text("Render Target: %s", RenderTargetToString(camera.renderTarget).c_str());
					//ImGui::Text("Render Framebuffer: %s", camera.renderFramebuffer ? camera.renderFramebuffer->GetName().c_str() : "None");
				}

				ImGui::TreePop();
			}
		}
	};
}