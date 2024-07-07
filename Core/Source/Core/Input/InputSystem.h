#pragma once
#include <unordered_map>

#include "Core/Application/Window.h"
#include "InputCommon.h"

#include <glm/vec2.hpp>

namespace Engine {
	struct InputValue {
		Action currentAction;
		Action previousAction;
	};

	class InputManager {
	public:
		InputManager() = default;
		~InputManager() = default;

		void Initialize(Window& window);

		Action GetInputState(Keycode keycode);
		bool IsKeyPressed(Keycode keycode);
		bool IsKeyReleased(Keycode keycode);

		void SetBlockKeyInput(bool block) { _blockKeyInput = block; }
		bool BlockKeyInput() const { return _blockKeyInput; }

		Action GetInputState(MouseButton mouseButton);
		bool IsButtonPressed(MouseButton mouseButton);
		bool IsButtonReleased(MouseButton mouseButton);

		void SetBlockMouseInput(bool block) { _blockMouseInput = block; }
		bool BlockMouseInput() const { return _blockMouseInput; }

		glm::vec2 GetMousePosition();
		glm::vec2 GetMouseScroll();

		void SetCursorMode(CursorMode cursorMode);
		CursorMode GetCursorMode() const { return _cursorMode; }
	private:
		std::unordered_map<Keycode, InputValue> _keyBtnValues;
		std::unordered_map<MouseButton, InputValue> _mouseBtnValues;

		glm::vec2 _mousePosition;
		glm::vec2 _mouseScroll;
		CursorMode _cursorMode = CursorMode::Normal;

		bool _blockKeyInput = false;
		bool _blockMouseInput = false;
	};
}