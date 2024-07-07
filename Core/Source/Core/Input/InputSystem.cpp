#include "InputSystem.h"
#include "Logging/Logging.h"
#include <GLFW/glfw3.h>

using namespace Engine;

void InputManager::Initialize(Window& window) {
	window.Subscribe<WindowKeyEvent>([&](const WindowKeyEvent& e) {
		if (_keyBtnValues.find(e.key) == _keyBtnValues.end())
			_keyBtnValues[e.key] = InputValue{ e.action,Action::Released };
		else {
			auto& v = _keyBtnValues[e.key];
			v.previousAction = v.currentAction;
			v.currentAction = e.action;
		}
	});

	window.Subscribe<WindowMouseClickedEvent>([&](const WindowMouseClickedEvent& e) {
		if (_mouseBtnValues.find(e.button) == _mouseBtnValues.end())
			_mouseBtnValues[e.button] = InputValue{ e.action,Action::Released };
		else {
			auto& v = _mouseBtnValues[e.button];
			v.previousAction = v.currentAction;
			v.currentAction = e.action;
		}
	});

	window.Subscribe<WindowMouseMovedEvent>([&](const WindowMouseMovedEvent& e) {
		_mousePosition = { e.xPos,e.yPos };
	});

	window.Subscribe<WindowMouseScrolledEvent>([&](const WindowMouseScrolledEvent& e) {
		_mouseScroll = { e.xOffset,e.yOffset };
	});
}

Action InputManager::GetInputState(Keycode keycode) {
	return _keyBtnValues[keycode].currentAction;
}

bool InputManager::IsKeyPressed(Keycode keycode) {
	if (_blockKeyInput)
		return false;

	auto& v = _keyBtnValues[keycode];
	return v.currentAction == Action::Pressed || v.currentAction == Action::Repeat;
}

bool InputManager::IsKeyReleased(Keycode keycode) {
	return !IsKeyPressed(keycode);
}

Action InputManager::GetInputState(MouseButton mouseButton) {
	return _mouseBtnValues[mouseButton].currentAction;
}

bool InputManager::IsButtonPressed(MouseButton mouseButton) {
	if (_blockMouseInput)
		return false;

	auto& v = _mouseBtnValues[mouseButton];
	return v.currentAction == Action::Pressed || v.currentAction == Action::Repeat;
}

bool InputManager::IsButtonReleased(MouseButton mouseButton) {
	return !IsButtonPressed(mouseButton);
}

glm::vec2 InputManager::GetMousePosition() {
	return _mousePosition;
}

glm::vec2 InputManager::GetMouseScroll() {
	return _mouseScroll;
}

void InputManager::SetCursorMode(CursorMode cursorMode) {
	if (cursorMode == _cursorMode)
		return;

	_cursorMode = cursorMode;
	glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, (int)cursorMode);
}
