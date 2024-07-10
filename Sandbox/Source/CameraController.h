#pragma once
#include <glm/glm.hpp>
#include "Core/Input/InputSystem.h"
#include "ECS/Components/Native/TransformComponent.h"

class CameraController {
public:
	CameraController() : _inputManager(nullptr) {}
	CameraController(Engine::InputManager* inputManager)
		: _inputManager(inputManager) {}
	~CameraController() {}

	using Key = Engine::Keycode;
	using Mouse = Engine::MouseButton;
	void OnUpdate(Engine::TransformComponent& tc, float ts) {
		glm::vec2 mousePosition = _inputManager->GetMousePosition();
		glm::vec2 mouseDelta = mousePosition - _prevMousePosition;
		_prevMousePosition = mousePosition;

		if (_inputManager->IsButtonPressed(Mouse::Right)) {
			_inputManager->SetCursorMode(Engine::CursorMode::Disabled);

			float newRotationX = -mouseDelta.y * _freeLookSensitivity;
			float newRotationY = mouseDelta.x * _freeLookSensitivity;
			tc.rotation += glm::vec3(newRotationX, newRotationY, 0);

			bool fastMove = _inputManager->IsKeyPressed(Key::LeftShift);
			float moveSpeed = fastMove ? _moveSpeed * _speedMultiplier : _moveSpeed;

			glm::vec3 forward = glm::vec3(0, 0, -1);
			glm::mat4 rotationMatrix = glm::mat4(1.0f);
			rotationMatrix = glm::rotate(rotationMatrix, glm::radians(tc.rotation.x), glm::vec3(1, 0, 0));
			rotationMatrix = glm::rotate(rotationMatrix, glm::radians(-tc.rotation.y), glm::vec3(0, 1, 0));
			rotationMatrix = glm::rotate(rotationMatrix, glm::radians(-tc.rotation.z), glm::vec3(0, 0, 1));
			forward = glm::vec3(rotationMatrix * glm::vec4(forward, 0));
			ENGINE_TRACE("{},{},{}", forward.x, forward.y, forward.z);

			forward = tc.GetForwardDirection();

			if (_inputManager->IsKeyPressed(Key::W))
				tc.position += forward * moveSpeed * ts;
			if (_inputManager->IsKeyPressed(Key::S))
				tc.position -= forward * moveSpeed * ts;
			if (_inputManager->IsKeyPressed(Key::A)) {}
			//tc.position -= tc.GetRight() * moveSpeed * ts;
			if (_inputManager->IsKeyPressed(Key::D)) {}
			//tc.position += tc.GetRight() * moveSpeed * ts;
			if (_inputManager->IsKeyPressed(Key::E))
				tc.rotation.z += 50.0f * ts;
			//tc.position += glm::vec3(0,1,0) * moveSpeed * ts;
			if (_inputManager->IsKeyPressed(Key::Q))
				tc.rotation.z -= 50.0f * ts;
			//tc.position -= glm::vec3(0,1,0) * moveSpeed * ts;
		}
		else {
			_inputManager->SetCursorMode(Engine::CursorMode::Normal);
		}
	}

	void OnScroll(float offset) {}
private:
	Engine::InputManager* _inputManager;

	glm::vec2 _prevMousePosition = { 0,0 };

	float _freeLookSensitivity = 0.1f;
	float _moveSpeed = 10.0f;
	float _speedMultiplier = 4.0f;

	friend class CameraControllerUI_ImGui;
};

#include <imgui.h>

class CameraControllerUI_ImGui {
public:
	static void RenderUI(CameraController& cc) {}
};
