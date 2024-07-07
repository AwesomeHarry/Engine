#pragma once

#include <memory>

namespace Engine {
	class LoggingManager {
	public:
		LoggingManager() {}
		~LoggingManager() {}

		bool Initialize();
		void Shutdown();
	};
}