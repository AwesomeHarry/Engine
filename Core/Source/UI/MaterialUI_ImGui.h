#pragma once

#include <string>
#include <unordered_map>
#include "Rendering/Platform/Material.h"

#include <imgui.h>

namespace Engine::UI {
	class MaterialUI {
	public:
		enum class WidgetType { None = 0, Drag, Input, Slider, Color };
		struct UniformWidgetDesc {
			WidgetType widgetType = WidgetType::Drag;
			float speed = 0.1f;
			float min = 0.0f;
			float max = 0.0f;
		};

		static std::unordered_map<std::string, UniformWidgetDesc> CustomUniformWidgets;

		static void DrawUniformWidget(MaterialInstance& materialInstance, const std::string& uniformName, const UniformWidgetDesc& widgetDesc) {
			if (widgetDesc.widgetType == WidgetType::None)
				return;

			auto overridenUniformOpt = materialInstance.GetUniform(uniformName);
			bool isOverriden = materialInstance.IsUniformOverridden(uniformName);
			Material& baseMaterial = *materialInstance.GetBaseMaterial();
			auto baseUniformOpt = baseMaterial.GetUniform(uniformName);

			if (!baseUniformOpt.has_value()) {
				ImGui::Text("Uniform %s not found in base material", uniformName.c_str());
				return;
			}

			auto uniformValue = isOverriden ? overridenUniformOpt.value() : baseUniformOpt.value();

			std::visit([&](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, bool>) {
					bool value = arg;
					if (ImGui::Checkbox(uniformName.c_str(), &value))
						materialInstance.SetUniform(uniformName, value);
				}
				else if constexpr (std::is_same_v<T, int>) {
					int value = arg;
					switch (widgetDesc.widgetType) {
					case WidgetType::Drag:
						if (ImGui::DragInt(uniformName.c_str(), &value, widgetDesc.speed, (int)widgetDesc.min, (int)widgetDesc.max))
							materialInstance.SetUniform(uniformName, value);
						break;
					case WidgetType::Input:
						ImGui::InputInt(uniformName.c_str(), &value);
						if (ImGui::IsItemDeactivatedAfterEdit())
							materialInstance.SetUniform(uniformName, value);
						break;
					case WidgetType::Slider:
						if (ImGui::SliderInt(uniformName.c_str(), &value, (int)widgetDesc.min, (int)widgetDesc.max))
							materialInstance.SetUniform(uniformName, value);
						break;
					default:
						ImGui::Text("Unsupported type for uniform '%s'", uniformName.c_str());
						break;
					}
				}
				else if constexpr (std::is_same_v<T, float>) {
					float value = arg;
					switch (widgetDesc.widgetType) {
					case WidgetType::Drag:
						if (ImGui::DragFloat(uniformName.c_str(), &value, widgetDesc.speed, widgetDesc.min, widgetDesc.max))
							materialInstance.SetUniform(uniformName, value);
						break;
					case WidgetType::Input:
						if (ImGui::InputFloat(uniformName.c_str(), &value))
							materialInstance.SetUniform(uniformName, value);
						break;
					case WidgetType::Slider:
						if (ImGui::SliderFloat(uniformName.c_str(), &value, widgetDesc.min, widgetDesc.max))
							materialInstance.SetUniform(uniformName, value);
						break;
					default:
						ImGui::Text("Unsupported type for uniform '%s'", uniformName.c_str());
						break;
					}
				}
				else if constexpr (std::is_same_v<T, glm::vec2>) {
					glm::vec2 value = arg;
					switch (widgetDesc.widgetType) {
					case WidgetType::Drag:
						if (ImGui::DragFloat2(uniformName.c_str(), &value.x, widgetDesc.speed, widgetDesc.min, widgetDesc.max))
							materialInstance.SetUniform(uniformName, value);
						break;
					case WidgetType::Slider:
						if (ImGui::SliderFloat2(uniformName.c_str(), &value.x, widgetDesc.min, widgetDesc.max))
							materialInstance.SetUniform(uniformName, value);
						break;
					case WidgetType::Input:
						if (ImGui::InputFloat2(uniformName.c_str(), &value.x))
							materialInstance.SetUniform(uniformName, value);
						break;
					default:
						ImGui::Text("Unsupported type for uniform '%s'", uniformName.c_str());
						break;
					}
				}
				else if constexpr (std::is_same_v<T, glm::vec3>) {
					glm::vec3 value = arg;
					switch (widgetDesc.widgetType) {
					case WidgetType::Drag:
						if (ImGui::DragFloat3(uniformName.c_str(), &value.x, widgetDesc.speed, widgetDesc.min, widgetDesc.max))
							materialInstance.SetUniform(uniformName, value);
						break;
					case WidgetType::Color:
						if (ImGui::ColorEdit3(uniformName.c_str(), &value.x))
							materialInstance.SetUniform(uniformName, value);
						break;
					case WidgetType::Slider:
						if (ImGui::SliderFloat3(uniformName.c_str(), &value.x, widgetDesc.min, widgetDesc.max))
							materialInstance.SetUniform(uniformName, value);
						break;
					case WidgetType::Input:
						if (ImGui::InputFloat3(uniformName.c_str(), &value.x))
							materialInstance.SetUniform(uniformName, value);
						break;
					default:
						ImGui::Text("Unsupported type for uniform '%s'", uniformName.c_str());
						break;
					}
				}
				else if constexpr (std::is_same_v<T, glm::vec4>) {
					glm::vec4 value = arg;
					switch (widgetDesc.widgetType) {
					case WidgetType::Drag:
						if (ImGui::DragFloat4(uniformName.c_str(), &value.x, widgetDesc.speed, widgetDesc.min, widgetDesc.max))
							materialInstance.SetUniform(uniformName, value);
						break;
					case WidgetType::Color:
						if (ImGui::ColorEdit4(uniformName.c_str(), &value.x))
							materialInstance.SetUniform(uniformName, value);
						break;
					case WidgetType::Slider:
						if (ImGui::SliderFloat4(uniformName.c_str(), &value.x, widgetDesc.min, widgetDesc.max))
							materialInstance.SetUniform(uniformName, value);
						break;
					case WidgetType::Input:
						if (ImGui::InputFloat4(uniformName.c_str(), &value.x))
							materialInstance.SetUniform(uniformName, value);
						break;
					default:
						ImGui::Text("Unsupported type for uniform '%s'", uniformName.c_str());
						break;
					}
				}
			}, uniformValue);

			if (isOverriden) {
				ImGui::SameLine();
				if (ImGui::Button("Reset")) {
					materialInstance.ResetUniform(uniformName);
				}
			}
		}

		static void DrawAllUniformWidgets(MaterialInstance& materialInstance) {
			auto uniforms = materialInstance.GetBaseMaterial()->GetUniformNames();
			if (ImGui::TreeNode("Material Instance Uniforms")) {
				for (const auto& uniformName : uniforms) {
					UniformWidgetDesc widgetDesc;
					if (CustomUniformWidgets.find(uniformName) != CustomUniformWidgets.end())
						widgetDesc = CustomUniformWidgets[uniformName];
					else
						widgetDesc = { determineWidgetType(uniformName, materialInstance.GetUniform(uniformName).value()) };

					DrawUniformWidget(materialInstance, uniformName, widgetDesc);
				}
				ImGui::TreePop();
			}

			if (ImGui::Button("Reset All")) {
				for (const auto& uniformName : uniforms) {
					materialInstance.ResetUniform(uniformName);
				}
			}
		}
	private:
		template<typename T>
		static WidgetType determineWidgetType(const std::string& uniformName, const T& uniformValue) {
			// Hide camera uniforms
			if (uniformName.find("camera") != std::string::npos ||
				uniformName.find("Camera") != std::string::npos) {
				return WidgetType::None;
			}

			// Check if the name suggests it's a color
			if (uniformName.find("color") != std::string::npos ||
				uniformName.find("Color") != std::string::npos) {
				return WidgetType::Color;
			}

			// For vector types, prefer DRAG
			if constexpr (std::is_same_v<T, glm::vec2> ||
						  std::is_same_v<T, glm::vec3> ||
						  std::is_same_v<T, glm::vec4>) {
				return WidgetType::Drag;
			}

			// For scalars, prefer SLIDER
			if constexpr (std::is_same_v<T, float> || std::is_same_v<T, int>) {
				return WidgetType::Drag;
			}

			// Default to DEFAULT for other types
			return WidgetType::Drag;
		}
	};
}