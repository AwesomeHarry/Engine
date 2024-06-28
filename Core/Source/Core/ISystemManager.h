#pragma once

namespace Engine {
	class ISystemManager {
	public:
		virtual bool Initialize() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;
	};
}
