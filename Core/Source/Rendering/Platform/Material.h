#pragma once

#include <string>
#include <memory>
#include <map>
#include <unordered_map>

#include <variant>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture2D.h"

#include <imgui.h>

namespace Engine {
	class Material : public std::enable_shared_from_this<Material> {
	public:
		Material(std::shared_ptr<Shader> shader);
		~Material();

		void Bind() const;
		void Unbind() const;

		void SetShader(std::shared_ptr<Shader> shader);
		Shader& GetShader();
		const Shader& GetShader() const;

		void AddTexture(std::shared_ptr<Texture2D> texture, const std::string& uniformName, uint32_t index);
		void UpdateTexture(const std::string& uniformName, std::shared_ptr<Texture2D> newTexture);
		Texture2D& GetTexture(uint32_t index);
		uint32_t GetTextureCount();

		void SetUniform(const std::string& name, bool value);
		void SetUniform(const std::string& name, int value);
		void SetUniform(const std::string& name, float value);
		void SetUniform(const std::string& name, const glm::vec2& value);
		void SetUniform(const std::string& name, const glm::vec3& value);
		void SetUniform(const std::string& name, const glm::vec4& value);
		void SetUniform(const std::string& name, const glm::mat2& value);
		void SetUniform(const std::string& name, const glm::mat3& value);
		void SetUniform(const std::string& name, const glm::mat4& value);

		template<typename T>
		T GetUniform(const std::string& name) const;

		std::shared_ptr<Material> Copy();
	private:
		std::shared_ptr<Shader> _shader;
		using UniformValue = std::variant<bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;
		struct Uniform {
			UniformValue value;
			bool isSampler;
		};
		std::map<std::string, Uniform> _uniformMap;
		std::unordered_map<uint32_t, std::shared_ptr<Texture2D>> _textureMap;

		friend class MaterialUtils;
	};

	class MaterialUtils {
	public:
		enum class WidgetType { None = 0, Drag, Input, Slider, Color };

		static void DrawUniformWidget(Material& material, const std::string& uniformName, WidgetType type = WidgetType::Drag, float speed = 1.0f, float min = 0.0f, float max = 0.0f) {
			if (type == WidgetType::None)
				return;

			auto& uniformMap = material._uniformMap;
			auto it = uniformMap.find(uniformName);

			if (it == uniformMap.end()) {
				ImGui::Text("Uniform %s not found", uniformName.c_str());
				return;
			}

			if (it->second.isSampler) {
				ImGui::Text("%s : %d", uniformName.c_str(), std::get<int>(it->second.value));
				return;
			}

			std::visit([&](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, bool>) {
					bool value = arg;
					if (ImGui::Checkbox(uniformName.c_str(), &value))
						material.SetUniform(uniformName, value);
				}
				else if constexpr (std::is_same_v<T, int>) {
					int value = arg;
					switch (type) {
					case WidgetType::Drag:
						if (ImGui::DragInt(uniformName.c_str(), &value, speed, (int)min, (int)max))
							material.SetUniform(uniformName, value);
						break;
					case WidgetType::Input:
						ImGui::InputInt(uniformName.c_str(), &value);
						if (ImGui::IsItemDeactivatedAfterEdit())
							material.SetUniform(uniformName, value);
						break;
					case WidgetType::Slider:
						if (ImGui::SliderInt(uniformName.c_str(), &value, (int)min, (int)max))
							material.SetUniform(uniformName, value);
						break;
					default:
						ImGui::Text("Unsupported type for uniform '%s'", uniformName.c_str());
						break;
					}
				}
				else if constexpr (std::is_same_v<T, float>) {
					float value = arg;
					switch (type) {
					case WidgetType::Drag:
						if (ImGui::DragFloat(uniformName.c_str(), &value, speed, min, max))
							material.SetUniform(uniformName, value);
						break;
					case WidgetType::Input:
						if (ImGui::InputFloat(uniformName.c_str(), &value))
							material.SetUniform(uniformName, value);
						break;
					case WidgetType::Slider:
						if (ImGui::SliderFloat(uniformName.c_str(), &value, min, max))
							material.SetUniform(uniformName, value);
						break;
					default:
						ImGui::Text("Unsupported type for uniform '%s'", uniformName.c_str());
						break;
					}
				}
				else if constexpr (std::is_same_v<T, glm::vec2>) {
					glm::vec2 value = arg;
					switch (type) {
					case WidgetType::Drag:
						if (ImGui::DragFloat2(uniformName.c_str(), &value.x, speed, min, max))
							material.SetUniform(uniformName, value);
						break;
					case WidgetType::Slider:
						if (ImGui::SliderFloat2(uniformName.c_str(), &value.x, min, max))
							material.SetUniform(uniformName, value);
						break;
					case WidgetType::Input:
						if (ImGui::InputFloat2(uniformName.c_str(), &value.x))
							material.SetUniform(uniformName, value);
						break;
					default:
						ImGui::Text("Unsupported type for uniform '%s'", uniformName.c_str());
						break;
					}
				}
				else if constexpr (std::is_same_v<T, glm::vec3>) {
					glm::vec3 value = arg;
					switch (type) {
					case WidgetType::Drag:
						if (ImGui::DragFloat3(uniformName.c_str(), &value.x, speed, min, max))
							material.SetUniform(uniformName, value);
						break;
					case WidgetType::Color:
						if (ImGui::ColorEdit3(uniformName.c_str(), &value.x))
							material.SetUniform(uniformName, value);
						break;
					case WidgetType::Slider:
						if (ImGui::SliderFloat3(uniformName.c_str(), &value.x, min, max))
							material.SetUniform(uniformName, value);
						break;
					case WidgetType::Input:
						if (ImGui::InputFloat3(uniformName.c_str(), &value.x))
							material.SetUniform(uniformName, value);
						break;
					default:
						ImGui::Text("Unsupported type for uniform '%s'", uniformName.c_str());
						break;
					}
				}
				else if constexpr (std::is_same_v<T, glm::vec4>) {
					glm::vec4 value = arg;
					switch (type) {
					case WidgetType::Drag:
						if (ImGui::DragFloat4(uniformName.c_str(), &value.x, speed, min, max))
							material.SetUniform(uniformName, value);
						break;
					case WidgetType::Color:
						if (ImGui::ColorEdit4(uniformName.c_str(), &value.x))
							material.SetUniform(uniformName, value);
						break;
					case WidgetType::Slider:
						if (ImGui::SliderFloat4(uniformName.c_str(), &value.x, min, max))
							material.SetUniform(uniformName, value);
						break;
					case WidgetType::Input:
						if (ImGui::InputFloat4(uniformName.c_str(), &value.x))
							material.SetUniform(uniformName, value);
						break;
					default:
						ImGui::Text("Unsupported type for uniform '%s'", uniformName.c_str());
						break;
					}
				}
			}, it->second.value);
		}

		static void DrawAllUniformWidgets(Material& material) {
			auto& uniformMap = material._uniformMap;
			if (ImGui::TreeNode("Material Uniforms")) {
				for (const auto& [uniformName, uniformValue] : uniformMap) {
					WidgetType widgetType = determineWidgetType(uniformName, uniformValue);
					DrawUniformWidget(material, uniformName, widgetType);
				}
				ImGui::TreePop();
			}
		}
	private:
		template<typename T>
		static WidgetType determineWidgetType(const std::string& uniformName, const T& uniformValue) {
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