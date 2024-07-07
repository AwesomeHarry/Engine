#pragma once
#include <memory>

namespace Engine {
	class Window;
	class RenderManager;
	class InputManager;

	class Layer {
	protected:
		Window* _window = nullptr;
		RenderManager* _renderManager = nullptr;
		InputManager* _inputManager = nullptr;
	public:
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float ts) {}

		friend class Root;
	};
}