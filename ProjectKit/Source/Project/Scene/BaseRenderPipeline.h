#pragma once
#include <memory>

namespace Engine {
	class Scene;
	class Entity;
	class Framebuffer;

	class BaseRenderPipeline {
	protected:
		std::shared_ptr<Engine::Framebuffer> _mainFb;
	public:
		Engine::Framebuffer& GetMainFramebuffer() { return *_mainFb; }

		virtual void Initialize(std::shared_ptr<Engine::Framebuffer> mainFramebuffer) {
			_mainFb = mainFramebuffer;
		}
		virtual void RenderScene(Scene& scene, Engine::Entity& cameraEntity) = 0;
		virtual void OnResize(uint32_t newWidth, uint32_t newHeight) {}
	};
}