#pragma once
#include <imgui.h>
#include "Scene/Scene.h"
#include "Scene/Entity.h"

namespace Engine {
	class SceneHeirarchyUI_ImGui {
	private:
		static Entity _selectedEntity;
	public:
		static void RenderUI(Scene& scene) {
			auto& reg = scene.GetRegistry();
			auto view = reg.view<NameComponent>();
			for (auto e : view) {
				Entity entity(e, &scene);

				auto& name = entity.GetName();

				ImGui::PushID((uint32_t)entity.GetInstanceID());

				ImGuiTreeNodeFlags flags = ((_selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
				flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

				bool nodeOpened = ImGui::TreeNodeEx((void*)(intptr_t)(uint32_t)entity, flags, name.c_str());
				if (ImGui::IsItemClicked(0) && !ImGui::IsItemToggledOpen())
					_selectedEntity = entity;

				if (nodeOpened) {
					ImGui::TreePop();
				}

				ImGui::PopID();
			}

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
				_selectedEntity = {};
		}
		static Entity GetSelectedEntity() { return _selectedEntity; }
	};

	Entity SceneHeirarchyUI_ImGui::_selectedEntity = {};
}