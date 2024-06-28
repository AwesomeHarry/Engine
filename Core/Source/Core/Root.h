#pragma once

#include <memory>

#include "LoggingManager.h"
#include "WindowManager.h"

namespace Engine {
	class Root {
	public:
		Root() {}
		~Root() {}

		bool Initialize();
		void Run();
		void Shutdown();

		WindowManager& GetWindowManager() const { return *_windowManager; }
	private:
		std::unique_ptr<LoggingManager> _loggingManager;
		std::unique_ptr<WindowManager> _windowManager;
	};
}