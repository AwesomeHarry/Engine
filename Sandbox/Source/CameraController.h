#pragma once
#include "Core/Input/InputSystem.h"
#include <glm/glm.hpp>

class CameraController {
public:
	CameraController() : _inputManager(nullptr) {}
	CameraController(Engine::InputManager* inputManager)
		: _inputManager(inputManager) {}
	~CameraController() {}

	using Key = Engine::Keycode;
	using Mouse = Engine::MouseButton;
	void OnUpdate(float ts) {
		glm::vec2 mousePosition = _inputManager->GetMousePosition();
		glm::vec2 mouseDelta = mousePosition - _prevMousePosition;
		_prevMousePosition = mousePosition;

		if (_inputManager->IsButtonPressed(Mouse::Right)) {
			_inputManager->SetCursorMode(Engine::CursorMode::Disabled);

			_targetYaw -= mouseDelta.x * 0.1f;
			_targetPitch += mouseDelta.y * 0.1f;
			_targetPitch = glm::clamp(_targetPitch, -89.0f, 89.0f);
		}
		else {
			_inputManager->SetCursorMode(Engine::CursorMode::Normal);
		}

		// Apply smoothing to the yaw and pitch
		_yaw = glm::mix(_yaw, _targetYaw, _rotationSmoothness * ts);
		_pitch = glm::mix(_pitch, _targetPitch, _rotationSmoothness * ts);

		float yr = glm::radians(_yaw);
		float pr = glm::radians(_pitch);

		glm::vec3 direction;
		direction.x = glm::cos(pr) * glm::sin(yr);
		direction.y = glm::sin(pr);
		direction.z = glm::cos(pr) * glm::cos(yr);
		glm::vec3 forward = -glm::normalize(direction);

		_position = _origin - forward * _distance;

		if (glm::abs(_scrollVelocity) > 0.1f) {
			_scrollVelocity -= _scrollVelocity * _scrollDamp;
			_fov -= _scrollVelocity;
			_fov = glm::clamp(_fov, _scrollMinFov, _scrollMaxFov);
		}
	}

	void OnScroll(float offset) {
		_scrollVelocity += offset * _scrollSensitivity;
		_scrollVelocity = glm::clamp(_scrollVelocity, -_scrollMaxVelocity, _scrollMaxVelocity);

		_fov += offset;
	}

	inline glm::vec3 GetPosition() const { return _position; }
	inline glm::vec3 GetRotation() const { return { -_pitch,_yaw,0.0f }; }
	inline float GetFOV() const { return _fov; }
private:
	Engine::InputManager* _inputManager;

	glm::vec2 _prevMousePosition{ 0,0 };
	glm::vec3 _position{ 0,0,5 };

	float _pitch = 0.0f, _yaw = 0.0f;
	float _targetPitch = 0.0f, _targetYaw = 0.0f;
	glm::vec3 _origin{ 0,0,0 };
	float _fov = 30.0f;
	float _distance = 5.0f;

	float _scrollSensitivity = 1.0f;
	float _scrollMinFov = 10.0f, _scrollMaxFov = 170.0f;
	float _scrollDamp = 0.1f;
	float _scrollVelocity = 0.0f;
	float _scrollMaxVelocity = 5.0f;

	float _rotationSmoothness = 25.0f;

	friend class CameraControllerUI_ImGui;
};

#include <imgui.h>

class CameraControllerUI_ImGui {
public:
	static void RenderUI(CameraController& cc) {
		ImGui::DragFloat("Scroll Sensitivity", &cc._scrollSensitivity, 0.1f);
		ImGui::DragFloatRange2("Scroll FOV Range", &cc._scrollMinFov, &cc._scrollMaxFov, 0.1f);
		ImGui::DragFloat("Scroll Damp", &cc._scrollDamp, 0.1f);
		ImGui::Text("Scroll Velocity: %.2f", cc._scrollVelocity);
		ImGui::DragFloat("Max Scroll Velocity", &cc._scrollMaxVelocity);

		ImGui::DragFloat("Rotation Smoothness", &cc._rotationSmoothness);

		ImGui::DragFloat("Distance", &cc._distance);
	}
};
