#include "Root.h"

#include <spdlog/stopwatch.h>
#include "Logging/Logging.h"

using namespace Engine;

bool Root::Initialize() {
	spdlog::stopwatch sw;

	// Create systems
	_loggingManager = std::make_unique<LoggingManager>();
	_windowManager = std::make_unique<WindowManager>();

	// Initialize in order of dependency
	_loggingManager->Initialize();
	if (!_windowManager->Initialize()) {
		ENGINE_ERROR("Failed to initialize window manager");
		return false;
	}

	ENGINE_INFO("Systems initialized in {:.4}s", sw);

	return true;
}

void Root::Run() {
	// Update in order of dependency
	while (_windowManager->IsWindowOpen()) {
		_windowManager->Update();
	}
}

void Root::Shutdown() {
	// Shutdown in reverse order of initialization
	_windowManager->Shutdown();
	_loggingManager->Shutdown();
}