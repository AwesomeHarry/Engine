#pragma once
#include "ECS/Scene/Scene.h"

namespace Engine {
	class IRenderPipeline {
	public:
		virtual void Initialize() {}
		virtual void RenderScene(const Scene& scene) = 0;
	};
}