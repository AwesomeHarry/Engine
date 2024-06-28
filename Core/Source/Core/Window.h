#pragma once

#include <GLFW/glfw3.h>
#include "Logging/Logging.h"
#include "Util/EventSystem/EventDispatcher.h"

#include "Rendering/GraphicsContext.h"
#include "Rendering/RenderManager.h"
#include "Layer.h"

namespace Engine {
	class WindowEvent : public Event {};
	class WindowOpenEvent : public WindowEvent { EVENT_CLASS_TYPE(WindowOpenEvent) };
	class WindowCloseEvent : public WindowEvent { EVENT_CLASS_TYPE(WindowCloseEvent) };
	class WindowResizeEvent : public WindowEvent { 
	public:
		WindowResizeEvent(int width, int height) : _width(width), _height(height) {}
		inline int GetWidth() const { return _width; }
		inline int GetHeight() const { return _height; }
		EVENT_CLASS_TYPE(WindowResizeEvent)
	private:
		int _width, _height;
	};

	class Window : public EventDispatcher<WindowEvent> {
	public:
		Window(const std::string& title, int width, int height, const GraphicsContext& graphicsContext);
		~Window() {}

		void Open();
		void Close();

		void Update();

		void PushLayer(std::shared_ptr<Layer> layer);
		void PopLayer(std::shared_ptr<Layer> layer);

		inline void MakeCurrent() { glfwMakeContextCurrent(_handle); }

		inline bool IsOpen() const { return isOpen; }

		inline const std::string& GetTitle() const { return _title; }
		inline int GetWidth() const { return _width; }
		inline int GetHeight() const { return _height; }
		inline const std::vector<std::shared_ptr<Layer>>& GetLayers() const { return _layerStack; }

		inline GLFWwindow* GetHandle() const { return _handle; }
		inline RenderManager& GetRenderManager() { return *_renderer; }
	private:
		bool createWindow();
		void setupCallbacks();
		void attachLayer(std::shared_ptr<Layer> layer);

		inline bool shouldClose() { return glfwWindowShouldClose(_handle); }
	private:
		std::string _title;
		int _width, _height;

		GLFWwindow* _handle;

		bool isOpen = false;

		std::unique_ptr<RenderManager> _renderer;

		std::vector<std::shared_ptr<Layer>> _layerStack;
	};
}