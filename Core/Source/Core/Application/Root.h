#pragma once

#include <memory>

#include "Core/Logging/LoggingManager.h"
#include "Core/Application/Window.h"
#include "Rendering/RenderManager.h"
#include "Core/Input/InputSystem.h"

#include "Core/Application/Layer.h"

namespace Engine {
	class Root {
	public:
		Root() {}
		~Root() {}

		bool Initialize(const WindowSpec& windowSpec, const GraphicsContext& graphicsContext);

		void PushLayer(std::shared_ptr<Layer> layer);
		void PopLayer(std::shared_ptr<Layer> layer);
		
		void Run();
		void Shutdown();

		inline Window& GetWindow() const { return *_window; }
		inline RenderManager& GetRenderer() const { return *_renderManager; }
		inline const std::vector<std::shared_ptr<Layer>>& GetLayers() const { return _layerStack; }
	private:
		std::unique_ptr<LoggingManager> _loggingManager;
		std::unique_ptr<Window> _window;
		std::unique_ptr<RenderManager> _renderManager;
		std::unique_ptr<InputManager> _inputManager;

		std::vector<std::shared_ptr<Layer>> _layerStack;
	};
}
