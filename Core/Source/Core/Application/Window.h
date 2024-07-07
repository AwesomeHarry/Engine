#pragma once

#include "Logging/Logging.h"
#include "Util/EventSystem/EventDispatcher.h"

#include "Rendering/GraphicsContext.h"
#include "Rendering/RenderManager.h"
#include "Layer.h"

struct GLFWwindow;
namespace Engine {
	enum class Keycode;
	enum class MouseButton;
	enum class Action;

	class WindowEvent : public Event {};
	class WindowResizeEvent : public WindowEvent {
	public:
		WindowResizeEvent(uint32_t width, uint32_t height) : width(width), height(height) {}
		EVENT_CLASS_TYPE(WindowResizeEvent)
		uint32_t width, height;
	};
	class WindowKeyEvent : public WindowEvent {
	public:
		WindowKeyEvent(Keycode key, Action action, uint32_t mods) : key(key), action(action), mods(mods) {}
		EVENT_CLASS_TYPE(WindowKeyEvent)
		Keycode key;
		Action action;
		uint32_t mods;
	};
	class WindowMouseClickedEvent : public WindowEvent {
	public:
		WindowMouseClickedEvent(MouseButton button, Action action, uint32_t mods) : button(button), action(action), mods(mods) {}
		EVENT_CLASS_TYPE(WindowMouseClickedEvent)
		MouseButton button;
		Action action;
		uint32_t mods;
	};
	class WindowMouseMovedEvent : public WindowEvent {
	public:
		WindowMouseMovedEvent(double xPos, double yPos) : xPos(xPos), yPos(yPos) {}
		EVENT_CLASS_TYPE(WindowMouseMovedEvent)
		double xPos, yPos;
	};
	class WindowMouseScrolledEvent : public WindowEvent {
	public:
		WindowMouseScrolledEvent(double xOffset, double yOffset) : xOffset(xOffset), yOffset(yOffset) {}
		EVENT_CLASS_TYPE(WindowMouseScrolledEvent)
		double xOffset, yOffset;
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
		inline float GetAspect() const { return (float)_width / (float)_height; }

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