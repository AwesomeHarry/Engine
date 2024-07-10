#pragma once
#include <vector>
#include <memory>

#include "Rendering/GraphicsContext.h"
#include "Rendering/RenderCommands.h"

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

		inline void SetWireframeMode(WireframeMode mode) { _wireframeMode = mode; }
		inline void SetClearColor(float r, float g, float b, float a) { _clearColor[0] = r; _clearColor[1] = g; _clearColor[2] = b; _clearColor[3] = a; }
	private:
		void setContext();
	private:
		GraphicsContext _graphicsContext;

		WireframeMode _wireframeMode;
		float _clearColor[4] = { 0, 0, 0, 1 };
	};
}
