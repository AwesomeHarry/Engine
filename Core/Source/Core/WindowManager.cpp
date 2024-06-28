#include "WindowManager.h"

#include "Logging/Logging.h"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

using namespace Engine;

bool WindowManager::Initialize() {
	glfwSetErrorCallback([](int error, const char* description) {
		ENGINE_ERROR("GLFW Error {}: {}", error, description);
	});

	if (!glfwInit()) {
		ENGINE_ERROR("Failed to initialize GLFW");
		return false;
	}
	return true;
}

void WindowManager::Update() {
	for (auto& it : _windowMap) {
		auto window = it.second;
		window->Update();
	}
}

void WindowManager::Shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
}

bool WindowManager::IsWindowOpen() {
	for (auto& window : _windowMap) {
		if (window.second->IsOpen())
			return true;
	}
	return false;
}

std::shared_ptr<Window> WindowManager::CreateNewWindow(const WindowCreateSettings& windowCreateSettings, const GraphicsContext& graphicsContext) {
	auto it = _windowMap.find(windowCreateSettings.Title);
	if (it != _windowMap.end()) {
		ENGINE_ERROR("Failed to create window with title {} - window already exists", windowCreateSettings.Title);
		return nullptr;
	}

	auto window = std::make_shared<Window>(windowCreateSettings.Title, windowCreateSettings.Width, windowCreateSettings.Height, graphicsContext);
	_windowMap[windowCreateSettings.Title] = window;
	Dispatch(WindowCreatedEvent(window));

	window->Open();
	Dispatch(WindowOpenedEvent(window));

	return window;
}

std::shared_ptr<Window> WindowManager::OpenWindow(const std::string& name) {
	auto window = GetWindow(name);
	if (window) {
		window->Open();
		Dispatch(WindowOpenedEvent(window));
	}
	return window;
}

std::shared_ptr<Window> WindowManager::CloseWindow(const std::string& name) {
	auto window = GetWindow(name);
	if (window) {
		window->Close();
		Dispatch(WindowClosedEvent(window));
	}
	return window;
}

std::shared_ptr<Window> WindowManager::GetWindow(const std::string& name) {
	auto it = _windowMap.find(name);
	if (it == _windowMap.end()) {
		ENGINE_ERROR("Window with name {} does not exist", name);
		return nullptr;
	}

	return it->second;
}