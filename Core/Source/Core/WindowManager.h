#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <spdlog/spdlog.h>

#include "ISystemManager.h"
#include "Util/EventSystem/EventDispatcher.h"
#include "Window.h"

namespace Engine {
	struct WindowCreateSettings {
		std::string Title;
		int Width;
		int Height;
	};

	class WindowManagerEvent : public Event {};
	class WindowCreatedEvent : public WindowManagerEvent {
	public:
		WindowCreatedEvent(std::shared_ptr<Window> window) : _window(window) {}
		std::shared_ptr<Window> GetWindow() const { return _window; }
		EVENT_CLASS_TYPE(WindowCreatedEvent)
	private:
		std::shared_ptr<Window> _window;
	};
	class WindowOpenedEvent : public WindowManagerEvent {
	public:
		WindowOpenedEvent(std::shared_ptr<Window> window) : _window(window) {}
		std::shared_ptr<Window> GetWindow() const { return _window; }
		EVENT_CLASS_TYPE(WindowOpenedEvent)
	private:
		std::shared_ptr<Window> _window;
	};
	class WindowClosedEvent : public WindowManagerEvent {
	public:
		WindowClosedEvent(std::shared_ptr<Window> window) : _window(window) {}
		std::shared_ptr<Window> GetWindow() const { return _window; }
		EVENT_CLASS_TYPE(WindowClosedEvent)
	private:
		std::shared_ptr<Window> _window;
	};

	class WindowManager : public ISystemManager, public EventDispatcher<WindowManagerEvent> {
	public:
		WindowManager() {}
		~WindowManager() {}

		bool Initialize() override;
		void Update() override;
		void Shutdown() override;

		bool IsWindowOpen();

		std::shared_ptr<Window> CreateNewWindow(const WindowCreateSettings& windowCreateSettings, const GraphicsContext& graphicsContext);
		std::shared_ptr<Window> OpenWindow(const std::string& name);
		std::shared_ptr<Window> CloseWindow(const std::string& name);

		std::shared_ptr<Window> GetWindow(const std::string& name);
	private:
		std::map<std::string, std::shared_ptr<Window>> _windowMap;
	};
}
