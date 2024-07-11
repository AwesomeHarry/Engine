#pragma once
#include "Core/Input/InputSystem.h"
#include <ECS/Components/Native/TransformComponent.h>

class FPSCameraController {
public:
	FPSCameraController() : _inputManager(nullptr) {}
	FPSCameraController(Engine::InputManager* inputManager)
		: _inputManager(inputManager) {}
	~FPSCameraController() {}

	using Key = Engine::Keycode;
	using Mouse = Engine::MouseButton;
	void OnUpdate(Engine::TransformComponent& transform, float ts) {
		glm::vec2 mouseDelta = GetMouseDelta();

		if (_inputManager->IsButtonPressed(Engine::MouseButton::Right)) {
			_inputManager->SetCursorMode(Engine::CursorMode::Disabled);

			transform.rotation.y += mouseDelta.x * mouseSensitivity;
			transform.rotation.x -= mouseDelta.y * mouseSensitivity;
			transform.rotation.x = glm::clamp(transform.rotation.x, -89.0f, 89.0f);

			if (_inputManager->IsKeyPressed(Key::R))
				transform.rotation.z += cameraSpeed * ts * 3.0f;
			if (_inputManager->IsKeyPressed(Key::F))
				transform.rotation.z -= cameraSpeed * ts * 3.0f;
		}
		else {
			_inputManager->SetCursorMode(Engine::CursorMode::Normal);
		}

		glm::vec3 cameraFront = transform.GetForwardDirection();
		glm::vec3 cameraRight = transform.GetRightDirection();
		glm::vec3 cameraUp = transform.GetUpDirection();

		float speed = cameraSpeed * ts;

		if (_inputManager->IsKeyPressed(Key::LeftShift))
			speed *= cameraBoostMultiplier;

		if (_inputManager->IsKeyPressed(Key::W))
			transform.position += cameraFront * speed;
		if (_inputManager->IsKeyPressed(Key::S))
			transform.position -= cameraFront * speed;
		if (_inputManager->IsKeyPressed(Key::A))
			transform.position -= cameraRight * speed;
		if (_inputManager->IsKeyPressed(Key::D))
			transform.position += cameraRight * speed;
		if (_inputManager->IsKeyPressed(Key::E))
			transform.position += cameraUp * speed;
		if (_inputManager->IsKeyPressed(Key::Q))
			transform.position -= cameraUp * speed;
	}

	glm::vec2 GetMouseDelta() {
		glm::vec2 mousePosition = _inputManager->GetMousePosition();
		glm::vec2 diff = mousePosition - _prevMousePosition;
		_prevMousePosition = mousePosition;
		return { diff.x, -diff.y };
	}
public:
	// Camera Properties
	float mouseSensitivity = 0.25f;
	float cameraSpeed = 5.0f;
	float cameraBoostMultiplier = 3.0f;
private:
	Engine::InputManager* _inputManager;

	// Mouse Delta
	glm::vec2 _prevMousePosition = { 0,0 };
};

#include <imgui.h>

class FPSCameraControllerUI_ImGui {
public:
	static void RenderUI(FPSCameraController& cc) {
		if (ImGui::TreeNodeEx("FPSCameraController", ImGuiTreeNodeFlags_Framed)) {
			ImGui::SliderFloat("Mouse Sensitivity", &cc.mouseSensitivity, 0.01f, 1.0f);
			ImGui::SliderFloat("Camera Speed", &cc.cameraSpeed, 0.01f, 100.0f);
			ImGui::SliderFloat("Camera Boost Multiplier", &cc.cameraBoostMultiplier, 0.01f, 10.0f);

			ImGui::TreePop();
		}
	}
};