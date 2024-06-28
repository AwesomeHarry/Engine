#pragma once
#include <vector>

#include "Core/Rendering/GraphicsContext.h"
#include "Core/Rendering/RenderCommands.h"
#include "Core/Rendering/IRenderPipeline.h"

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

		void RenderScene(const Scene& scene);

		inline void SetRenderMode(WireframeMode mode) { _wireframeMode = mode; }
		inline void SetClearColor(float r, float g, float b, float a) { _clearColor[0] = r; _clearColor[1] = g; _clearColor[2] = b; _clearColor[3] = a; }
	private:
		void setContext();
	private:
		GraphicsContext _graphicsContext;
		ImGuiContext* _imguiContext;

		WireframeMode _wireframeMode;
		float _clearColor[4] = { 0, 0, 0, 1 };
	};
}
