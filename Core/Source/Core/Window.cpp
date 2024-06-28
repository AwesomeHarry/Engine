#include "Window.h"

using namespace Engine;

Window::Window(const std::string& title, int width, int height, const GraphicsContext& graphicsContext)
	: _title(title), _width(width), _height(height), _handle(nullptr) {
	_renderer = std::make_unique<RenderManager>(graphicsContext);
}

void Window::Open() {
	if (isOpen)
		ENGINE_WARN("Trying to open window that is already open - {}", _title);

	// Create window and initialize renderer
	if (createWindow() && _renderer->Initialize(*this)) {
		ENGINE_INFO("Window created - {}", _title);
	}
	else {
		ENGINE_ERROR("Failed to create window - {}", _title);
		Close();
		return;
	}

	isOpen = true;
	for (auto& layer : _layerStack) {
		attachLayer(layer);
		layer->OnAttach();
	}

	Dispatch(WindowOpenEvent());
	Dispatch(WindowResizeEvent(_width, _height));
}

bool Window::createWindow() {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_handle = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
	if (!_handle) {
		ENGINE_ERROR("Failed to create window");
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
}

void Window::Close() {
	if (!isOpen)
		ENGINE_WARN("Trying to close window that is already closed - {}", _title);

	Dispatch(WindowCloseEvent());

	for (auto& layer : _layerStack)
		layer->OnDetach();

	isOpen = false;
	_renderer->Shutdown();
	glfwMakeContextCurrent(_handle);
	glfwDestroyWindow(_handle);

	ENGINE_INFO("Window closed - {}", _title);
}

void Window::Update() {
	if (!isOpen)
		return;

	if (shouldClose()) {
		Close();
		return;
	}

	glfwMakeContextCurrent(_handle);
	glfwPollEvents();

	_renderer->BeginFrame();

	for (auto& layer : _layerStack) {
		attachLayer(layer);
		layer->OnUpdate(0.2f);
	}

	_renderer->EndFrame();

	glfwSwapBuffers(_handle);
}

void Window::PushLayer(std::shared_ptr<Layer> layer) {
	_layerStack.push_back(layer);

	if (isOpen) {
		attachLayer(layer);
		layer->OnAttach();
	}
}

void Window::attachLayer(std::shared_ptr<Layer> layer) {
	layer->_attachedWindow = this;
	layer->_renderer = _renderer.get();
}

void Window::PopLayer(std::shared_ptr<Layer> layer) {
	auto it = std::find(_layerStack.begin(), _layerStack.end(), layer);
	if (it != _layerStack.end()) {
		(*it)->OnDetach();
		_layerStack.erase(it);
	}
}
