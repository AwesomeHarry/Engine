#pragma once
#include "Core/Window.h"
#include <imgui.h>

namespace Engine {
	class WindowInfoUI {
	public:
		WindowInfoUI(const Window& window)
			: _window(window) {}

		virtual void RenderUI() = 0;
	protected:
		const Window& _window;
	};
}