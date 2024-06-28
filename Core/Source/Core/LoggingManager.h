#pragma once

#include <memory>
#include "ISystemManager.h"

namespace Engine {
	class LoggingManager : public ISystemManager {
	public:
		LoggingManager() {}
		~LoggingManager() {}

		bool Initialize() override;
		void Update() override {}
		void Shutdown() override;
	};
}