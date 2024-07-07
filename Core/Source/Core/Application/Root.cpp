#include "Root.h"

#include <spdlog/stopwatch.h>
#include "Logging/Logging.h"

using namespace Engine;

bool Root::Initialize(const WindowSpec& windowSpec, const GraphicsContext& graphicsContext) {
	spdlog::stopwatch sw;

	// Create systems
	_loggingManager = std::make_unique<LoggingManager>();

	_window = std::make_unique<Window>(windowSpec);
	_renderManager = std::make_unique<RenderManager>(graphicsContext);
	_inputManager = std::make_unique<InputManager>();

	// Initialize in order of dependency
	_loggingManager->Initialize();

	if (!_window->Initialize())
		return false;

	_inputManager->Initialize(*_window);

	// Log debug
	ENGINE_INFO("Systems initialized in {:.4f}s", sw);
	return true;
}

void Root::PushLayer(std::shared_ptr<Layer> layer) {
	_layerStack.push_back(layer);

	layer->_window = _window.get();
	layer->_renderManager = _renderManager.get();
	layer->_inputManager = _inputManager.get();

	if (_window->IsOpen())
		layer->OnAttach();
}

void Root::PopLayer(std::shared_ptr<Layer> layer) {
	auto it = std::find(_layerStack.begin(), _layerStack.end(), layer);
	if (it != _layerStack.end()) {
		(*it)->OnDetach();
		_layerStack.erase(it);
	}
}

void Root::Run() {
	_window->Open();
	if (!_renderManager->Initialize(*_window)) {
		ENGINE_ERROR("Failed to initialize renderer!");
		return;
	}

	for (auto& layer : _layerStack)
		layer->OnAttach();

	/* Setup deltaTime */
	using clock = std::chrono::high_resolution_clock;
	auto previousTime = clock::now();

	while (_window->IsOpen()) {
		_renderManager->BeginFrame();

		auto currentTime = clock::now();
		std::chrono::duration<float> deltaTimeDuration = currentTime - previousTime;
		float deltaTime = deltaTimeDuration.count();
		previousTime = currentTime;

		for (auto& layer : _layerStack) {
			layer->OnUpdate(deltaTime);
		}

		_renderManager->EndFrame();
		_window->Update();
	}

	for (auto& layer : _layerStack)
		layer->OnDetach();

	_window->Close();
}

void Root::Shutdown() {
	// Shutdown in reverse order of initialization
	_renderManager->Shutdown();
	_window->Shutdown();
	_loggingManager->Shutdown();
}