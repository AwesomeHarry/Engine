#pragma once

namespace Engine {
	class Window;
	class RenderManager;

	class Layer {
	protected:
		Window* _attachedWindow = nullptr;
		RenderManager* _renderer = nullptr;
	public:
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float ts) {}

		friend class Window;
	};
}