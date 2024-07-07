#pragma once
#include <vector>
#include <memory>

#include "Rendering/GraphicsContext.h"
#include "Rendering/RenderCommands.h"
#include "Rendering/IRenderPipeline.h"

struct ImGuiContext;
namespace Engine {
	class Window;
	class Scene;

	class RenderManager {
	public:
		RenderManager(const GraphicsContext& graphicsContext);
		~RenderManager() {}

		bool Initialize(Window& window);
		void Shutdown();

		void BeginFrame();
		void EndFrame();

		void RenderScene(Scene& scene);

		template<typename T, typename... Args>
		void SetPipeline(Args... args) {
			if (!std::is_base_of<IRenderPipeline, T>()) {
				ENGINE_ERROR("The render pipline must inherit IRenderPipeline");
				return;
			}
			_renderPipeline = std::make_unique<T>(std::forward<Args>(args)...);
		}

		inline void SetWireframeMode(WireframeMode mode) { _wireframeMode = mode; }
		inline void SetClearColor(float r, float g, float b, float a) { _clearColor[0] = r; _clearColor[1] = g; _clearColor[2] = b; _clearColor[3] = a; }
	private:
		void setContext();
	private:
		GraphicsContext _graphicsContext;
		std::unique_ptr<IRenderPipeline> _renderPipeline;

		WireframeMode _wireframeMode;
		float _clearColor[4] = { 0, 0, 0, 1 };
	};
}
