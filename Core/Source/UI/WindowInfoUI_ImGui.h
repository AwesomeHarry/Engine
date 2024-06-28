#pragma once

#include "WindowInfoUI.h"

namespace Engine {
	class WindowInfoUI_ImGui : public WindowInfoUI {
	public:
		WindowInfoUI_ImGui(const Window& window)
			: WindowInfoUI(window) {}
		~WindowInfoUI_ImGui() = default;

		virtual void RenderUI() override {
			ImGui::Text("Title: %s", _window.GetTitle().c_str());
			ImGui::Text("Width: %d", _window.GetWidth());
			ImGui::Text("Height: %d", _window.GetHeight());
			ImGui::Text("Layer Count: %d", _window.GetLayers().size());
		}
	};
}