#pragma once
#include "Core/Input/InputSystem.h"
#include "Scene/Components/Native/TransformComponent.h"
#include <glm/glm.hpp>

class OrbitCameraController {
public:
	OrbitCameraController()
		: _inputManager(nullptr) {}
	OrbitCameraController(Engine::InputManager* inputManager)
		: _inputManager(inputManager) {}
	~OrbitCameraController() {}

	using Key = Engine::Keycode;
	using Mouse = Engine::MouseButton;

	void OnUpdate(Engine::TransformComponent& transform, float ts) {
		glm::vec2 mouseDelta = GetMouseDelta();

		float yaw = 0, pitch = 0;

		if (_inputManager->IsButtonPressed(Mouse::Right)) {
			_inputManager->SetCursorMode(Engine::CursorMode::Disabled);

			if (_inputManager->IsKeyPressed(Key::LeftShift)) {
				_mode = CameraMode::Pan;
			}
			else {
				_mode = CameraMode::Orbit;
			}


			switch (_mode) {
			case CameraMode::Orbit:
				yaw = mouseDelta.x * mouseSensitivity;
				pitch = -mouseDelta.y * mouseSensitivity;
				break;
			case CameraMode::Pan:
				_origin += transform.GetRightDirection() * -mouseDelta.x * mouseSensitivity * 0.1f;
				_origin += transform.GetUpDirection() * -mouseDelta.y * mouseSensitivity * 0.1f;
				break;
			default:
				break;
			}

		}
		else {
			_inputManager->SetCursorMode(Engine::CursorMode::Normal);
		}

		// Smooth zoom
		_zoomLevel += (_targetZoomLevel - _zoomLevel) * zoomSmoothness * ts;

		// Always look at the origin
		transform.rotation += glm::vec3(pitch, yaw, 0.0f);
		transform.rotation.x = glm::clamp(transform.rotation.x, -89.0f, 89.0f);

		// Update camera position based on spherical coordinates
		transform.position.x = _origin.x + _zoomLevel * glm::cos(glm::radians(transform.rotation.y + 90.0f)) * glm::cos(glm::radians(transform.rotation.x));
		transform.position.y = _origin.y + _zoomLevel * glm::sin(glm::radians(transform.rotation.x));
		transform.position.z = _origin.z + _zoomLevel * glm::sin(glm::radians(transform.rotation.y + 90.0f)) * glm::cos(glm::radians(transform.rotation.x));
	}

	void OnScroll(float yOffset) {
		_targetZoomLevel -= yOffset * zoomSensitivity;
		_targetZoomLevel = glm::clamp(_targetZoomLevel, minZoomLevel, maxZoomLevel);
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
	float zoomSensitivity = 1.0f;
	float zoomSmoothness = 5.0f;
	float minZoomLevel = 1.0f;
	float maxZoomLevel = 100.0f;

private:
	Engine::InputManager* _inputManager;

	// Orbit Properties
	glm::vec3 _origin = { 0,0,0 };
	float _yaw = -90.0f;
	float _pitch = 0.0f;
	float _zoomLevel = 10.0f;
	float _targetZoomLevel = _zoomLevel;

	enum class CameraMode {
		Orbit, Pan
	};
	CameraMode _mode = CameraMode::Orbit;

	// Mouse Delta
	glm::vec2 _prevMousePosition = { 0, 0 };
};

#include <imgui.h>

class OrbitalCameraControllerUI_ImGui {
public:
	static void RenderUI(OrbitCameraController& cc) {
		if (ImGui::TreeNodeEx("OrbitalCameraController", ImGuiTreeNodeFlags_Framed)) {
			ImGui::SliderFloat("Mouse Sensitivity", &cc.mouseSensitivity, 0.01f, 1.0f);
			ImGui::SliderFloat("Zoom Sensitivity", &cc.zoomSensitivity, 0.01f, 10.0f);
			ImGui::SliderFloat("Zoom Smoothness", &cc.zoomSmoothness, 0.01f, 10.0f);
			ImGui::SliderFloat("Min Zoom Level", &cc.minZoomLevel, 0.1f, 50.0f);
			ImGui::SliderFloat("Max Zoom Level", &cc.maxZoomLevel, 10.0f, 200.0f);

			ImGui::TreePop();
		}
	}
};
