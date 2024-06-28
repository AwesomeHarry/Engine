#pragma once

#include "ECS/Scene/Scene.h"
#include "ECS/Scene/Entity.h"

namespace Engine {
	class IRenderPipeline {
	public:
		virtual void RenderScene(const Scene& scene) {};
	};
}