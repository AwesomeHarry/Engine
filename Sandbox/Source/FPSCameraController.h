#pragma once
#include "Core/Input/InputSystem.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class FPSCameraController {
public:
    FPSCameraController() : _inputManager(nullptr) {}
    FPSCameraController(Engine::InputManager* inputManager)
        : _inputManager(inputManager) {}
    ~FPSCameraController() {}

    using Key = Engine::Keycode;
    using Mouse = Engine::MouseButton;

    void OnUpdate(float ts) {
        bool isControlActive = _inputManager->IsButtonPressed(Mouse::Right);

        if (isControlActive) {
            _inputManager->SetCursorMode(Engine::CursorMode::Disabled);

            // Mouse look
            glm::vec2 mousePosition = _inputManager->GetMousePosition();
            glm::vec2 mouseDelta = mousePosition - _prevMousePosition;
            _yaw -= mouseDelta.x * _mouseSensitivity;
            _pitch += mouseDelta.y * _mouseSensitivity;
            _pitch = glm::clamp(_pitch, -89.0f, 89.0f);

            ENGINE_TRACE("{},{}", mouseDelta.x, mouseDelta.y);

            // Update camera orientation
            float yr = glm::radians(_yaw);
            float pr = glm::radians(_pitch);

            glm::vec3 front;
            front.x = glm::cos(pr) * glm::sin(yr);
            front.y = -glm::sin(pr);
            front.z = glm::cos(pr) * glm::cos(yr);
            _front = -glm::normalize(front);
            _right = glm::normalize(glm::cross(_front, glm::vec3(0, 1, 0)));
            _up = glm::normalize(glm::cross(_right, _front));

            // Movement
            float speed = _moveSpeed;
            if (_inputManager->IsKeyPressed(Key::LeftShift)) {
                speed *= _speedMultiplier;
            }

            if (_inputManager->IsKeyPressed(Key::W)) _position += _front * speed * ts;
            if (_inputManager->IsKeyPressed(Key::S)) _position -= _front * speed * ts;
            if (_inputManager->IsKeyPressed(Key::D)) _position += _right * speed * ts;
            if (_inputManager->IsKeyPressed(Key::A)) _position -= _right * speed * ts;
            if (_inputManager->IsKeyPressed(Key::E)) _position += glm::vec3(0, 1, 0) * speed * ts;
            if (_inputManager->IsKeyPressed(Key::Q)) _position -= glm::vec3(0, 1, 0) * speed * ts;
        }
        else {
            _inputManager->SetCursorMode(Engine::CursorMode::Normal);
        }

        _prevMousePosition = _inputManager->GetMousePosition();
    }

    inline glm::vec3 GetPosition() const { return _position; }
    inline glm::vec3 GetFront() const { return _front; }
    inline glm::vec3 GetUp() const { return _up; }
    inline glm::vec3 GetRight() const { return _right; }

    glm::vec3 GetRotation() const {
        return glm::vec3(-_pitch, _yaw, 0.0f);
    }

private:
    Engine::InputManager* _inputManager;
    glm::vec2 _prevMousePosition{ 0, 0 };
    glm::vec3 _position{ 0, 0, 5 };
    glm::vec3 _front{ 0, 0, 1 };
    glm::vec3 _up{ 0, 1, 0 };
    glm::vec3 _right{ 1, 0, 0 };
    float _yaw = 0.0F;
    float _pitch = 0.0f;
    float _moveSpeed = 5.0f;
    float _mouseSensitivity = 0.1f;
    float _speedMultiplier = 2.0f;

    friend class FPSCameraControllerUI_ImGui;
};

#include <imgui.h>

class FPSCameraControllerUI_ImGui {
public:
    static void RenderUI(FPSCameraController& cc) {
        ImGui::DragFloat("Move Speed", &cc._moveSpeed, 0.1f);
        ImGui::DragFloat("Mouse Sensitivity", &cc._mouseSensitivity, 0.01f);
        ImGui::DragFloat("Speed Multiplier", &cc._speedMultiplier, 0.1f);

        ImGui::Text("Front: (%.2f, %.2f, %.2f)", cc._front.x, cc._front.y, cc._front.z);
        ImGui::Text("Right: (%.2f, %.2f, %.2f)", cc._right.x, cc._right.y, cc._right.z);
        ImGui::Text("Up: (%.2f, %.2f, %.2f)", cc._up.x, cc._up.y, cc._up.z);

        ImGui::Text("Position: (%.2f, %.2f, %.2f)", cc._position.x, cc._position.y, cc._position.z);
        ImGui::Text("Yaw: %.2f, Pitch: %.2f", cc._yaw, cc._pitch);
    }
};