#pragma once

#include "Core/Window.h"
#include <imgui.h>

namespace Engine {
	class WindowInfoUI_ImGui {
	public:
		static void RenderUI(const Window& window) {
			ImGui::Text("Title: %s", window.GetTitle().c_str());
			ImGui::Text("Width: %d", window.GetWidth());
			ImGui::Text("Height: %d", window.GetHeight());
		}
	};
}