#include "Window.h"
#include <GLFW/glfw3.h>
#include "Core/Input/InputCommon.h"

using namespace Engine;

Window::Window(const WindowSpec& windowSpec)
	: _title(windowSpec.Title), _width(windowSpec.Width), _height(windowSpec.Height), _handle(nullptr) {}

bool Window::Initialize() {
	glfwSetErrorCallback([](int error, const char* description) {
		ENGINE_ERROR("[Window] GLFW Error {}: {}", error, description);
	});

	if (!glfwInit()) {
		ENGINE_ERROR("[Window::Initialize] Failed to initialize GLFW");
		return false;
	}

	glfwMakeContextCurrent(_handle);

	return true;
}

void Window::Open() {
	if (isOpen) {
		ENGINE_WARN("[Window::Open] Trying to open window that is already open - {}", GetTitle());
		return;
	}

	// Create window
	if (createWindow()) {
		ENGINE_INFO("[Window::Open] Window created - {}", GetTitle());
	}
	else {
		ENGINE_ERROR("[Window::Open] Failed to create window - {}", GetTitle());
		Close();
		return;
	}

	isOpen = true;

	Dispatch(WindowResizeEvent(GetWidth(), GetHeight()));
}

bool Window::createWindow() {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_handle = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
	if (!_handle) {
		ENGINE_ERROR("[Window::createWindow] Failed to create window");
		return false;
	}

	glfwMakeContextCurrent(_handle);
	glfwSetWindowUserPointer(_handle, this);
	setupCallbacks();

	return true;
}

void Window::setupCallbacks() {
	// Set callbacks
	glfwSetFramebufferSizeCallback(_handle, [](GLFWwindow* window, int width, int height) {
		auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
		windowInstance->Dispatch(WindowResizeEvent(width, height));
	});

	glfwSetKeyCallback(_handle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
		windowInstance->Dispatch(WindowKeyEvent((Keycode)key, (Action)action, mods));
	});

	glfwSetMouseButtonCallback(_handle, [](GLFWwindow* window, int button, int action, int mods) {
		auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
		windowInstance->Dispatch(WindowMouseClickedEvent((MouseButton)button, (Action)action, mods));
	});

	glfwSetCursorPosCallback(_handle, [](GLFWwindow* window, double x, double y) {
		auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
		windowInstance->Dispatch(WindowMouseMovedEvent(x, y));
	});

	glfwSetScrollCallback(_handle, [](GLFWwindow* window, double x, double y) {
		auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
		windowInstance->Dispatch(WindowMouseScrolledEvent(x, y));
	});

	Subscribe<WindowResizeEvent>([&](const WindowResizeEvent& e) {
		_width = e.width;
		_height = e.height;
	});
}

void Window::Close() {
	isOpen = false;
	glfwDestroyWindow(_handle);

	ENGINE_INFO("[Window::Close] Window closed - {}", GetTitle());
}

void Window::Shutdown() {
	glfwTerminate();
}

void Window::Update() {
	if (!isOpen)
		return;

	if (glfwWindowShouldClose(_handle)) {
		isOpen = false;
		return;
	}

	glfwPollEvents();
	glfwSwapBuffers(_handle);
}

void Window::attachLayer(std::shared_ptr<Layer> layer) {
	//layer->_attachedWindow = this;
	//layer->_renderer = _renderer.get();
}
