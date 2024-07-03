#pragma once

#include "Logging/Logging.h"
#include "Util/EventSystem/EventDispatcher.h"

#include "Rendering/GraphicsContext.h"
#include "Rendering/RenderManager.h"
#include "Layer.h"

struct GLFWwindow;
namespace Engine {
	class WindowEvent : public Event {};
	class WindowResizeEvent : public WindowEvent { 
	public:
		WindowResizeEvent(int width, int height) : _width(width), _height(height) {}
		inline int GetWidth() const { return _width; }
		inline int GetHeight() const { return _height; }
		EVENT_CLASS_TYPE(WindowResizeEvent)
	private:
		int _width, _height;
	};

	struct WindowSpec {
		std::string Title;
		uint32_t Width, Height;
	};

	class Window : public EventDispatcher<WindowEvent> {
	public:
		Window(const WindowSpec& windowSpec);
		~Window() {}

		bool Initialize();
		void Open();
		void Close();
		void Shutdown();

		void Update();

		inline bool IsOpen() const { return isOpen; }
		inline const std::string& GetTitle() const { return _title; }
		inline int GetWidth() const { return _width; }
		inline int GetHeight() const { return _height; }

		inline GLFWwindow* GetHandle() const { return _handle; }
	private:
		bool createWindow();
		void setupCallbacks();
		void attachLayer(std::shared_ptr<Layer> layer);
	private:
		std::string _title;
		uint32_t _width, _height;
		bool isOpen = false;
		GLFWwindow* _handle;
	};
}