#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "Rendering/IRenderPipeline.h"
#include "Rendering/Platform/Buffer/UniformBufferObject.h"
#include "Rendering/Platform/Framebuffer.h"
#include "Rendering/RenderManager.h"
#include "ECS/Components/Components.h"

#pragma region Shaders

const char* debugShaderVSrc = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;
layout (std140) uniform CameraData {
	mat4 projection;
	mat4 view;
};
uniform mat4 model;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)glsl";

const char* debugShaderFSrc = R"glsl(
#version 330 core
out vec4 FragColor;
uniform vec4 color;
void main() {
    FragColor = color;
}
)glsl";

#pragma endregion

class SRP : public Engine::IRenderPipeline {
private:
	struct CameraData { glm::mat4 projection; glm::mat4 view; };
	CameraData _cameraData;
	std::shared_ptr<Engine::UniformBufferObject> _cameraDataUbo;
	std::shared_ptr<Engine::Framebuffer> _sceneFb;
	std::shared_ptr<Engine::Shader> _debugShader;
public:
	virtual void Initialize() {
		_cameraData.projection = glm::ortho(-1, 1, -1, 1);
		_cameraDataUbo = std::make_shared<Engine::UniformBufferObject>(sizeof(CameraData), 0);

		Engine::Framebuffer::FramebufferSpec fbSpec;
		fbSpec.width = 1280;
		fbSpec.height = 720;
		fbSpec.attachments = { Engine::ImageFormat::RGB8 };
		fbSpec.includeDepthStencil = true;

		_sceneFb = std::make_shared<Engine::Framebuffer>(fbSpec);

		_debugShader = std::make_shared<Engine::Shader>();
		_debugShader->AttachVertexShader(debugShaderVSrc);
		_debugShader->AttachFragmentShader(debugShaderFSrc);
		_debugShader->Link();
		_debugShader->BindUniformBlock("CameraData", 0);
	}

	virtual void RenderScene(Engine::Scene& scene) override {
		/* Set render targets from cameras */
		{}

		/* RenderOpaqueObjects(scene, camera); */
		/* RenderTransparentObjects(scene, camera); */

		/* ApplyPostProcessing(); */

		auto& reg = scene.GetRegistry();

		/* Handle Cameras */
		{
			auto view = reg.view<const Engine::TransformComponent, Engine::CameraComponent>();
			for (auto entity : view) {
				auto& transform = view.get<Engine::TransformComponent>(entity);
				auto& camera = view.get<Engine::CameraComponent>(entity);

				if (camera.isMainCamera) {
					float aspect = (float)_sceneFb->GetSpecification().width / (float)_sceneFb->GetSpecification().height;
					_cameraData.projection = camera.GetProjectionMatrix(aspect);
					_cameraData.view = camera.CalculateViewMatrix(transform);
					Engine::RenderCommands::SetClearColor(camera.backgroundColor.r, camera.backgroundColor.g, camera.backgroundColor.b);
				}
			}
		}

		_sceneFb->Bind();

		Engine::RenderCommands::ClearBuffers({ Engine::BufferBit::Color, Engine::BufferBit::Depth });

		_cameraDataUbo->Bind();
		_cameraDataUbo->SetData(&_cameraData, sizeof(CameraData), 0);

		/* Render Meshes */
		{
			auto view = reg.view<const Engine::TransformComponent, const Engine::MeshFilterComponent, const Engine::MeshRendererComponent>();
			for (auto entity : view) {
				auto& filter = view.get<Engine::MeshFilterComponent>(entity);
				auto& renderer = view.get<Engine::MeshRendererComponent>(entity);
				auto& transform = view.get<Engine::TransformComponent>(entity);

				auto& shader = *renderer.shader;
				auto& mesh = *filter.mesh;

				glm::mat4 model = MakeModelMatrix(transform.position, transform.rotation, transform.scale);
				shader.SetUniform("model", model);

				Engine::RenderCommands::RenderMesh(mesh, shader);
			}
		}

		/* Render Debug Shapes */
		{
			auto view = reg.view<Engine::DebugShapeManager>();
			if (view.size() != 0) {
				if (view.size() > 1)
					ENGINE_WARN("More than one DebugShapeManager in scene, only first will be used.");

				// Render each drawCall
				auto& dsm = view.get<Engine::DebugShapeManager>(view.front());

				/* Points */
				for (auto& point : dsm.points) {
					auto& mesh = dsm.GetMesh();
					auto& shader = dsm.GetPointShader();

					glm::mat4 model = glm::translate(glm::mat4(1.0f), point.pos);
					shader.SetUniform("model", model);
					shader.SetUniform("color", point.color);
					shader.SetUniform("radius", 0.1f);
					shader.SetUniform("segments", 20);

					Engine::RenderCommands::RenderMesh(mesh, shader);
				}

				/* Lines */
				auto& shader = *dsm.lineShader;
				shader.SetUniform("thickness", 0.01f);

				auto& vao = *dsm.lineInfoVao;
				auto& vbo = *dsm.lineInfoVbo;

				size_t lineCount = dsm.lineData.size();
				vbo.SetData(dsm.lineData.data(), lineCount, {
					{ "aStartPos", Engine::LType::Float, 3 },
					{ "aEndPos", Engine::LType::Float, 3 },
					{ "aColor", Engine::LType::Float, 4 }
							});

				Engine::RenderCommands::RenderPoints(vao, lineCount, shader);

				// Clear draw list
				dsm.points.clear();
				dsm.lineData.clear();
			}
		}

		_sceneFb->Unbind();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Scene");
		auto s = ImGui::GetContentRegionAvail();
		if (_sceneFb->GetSpecification().width != (uint32_t)s.x || _sceneFb->GetSpecification().height != (uint32_t)s.y)
			_sceneFb->Resize(s.x, s.y);
		ImGui::Image((ImTextureID)(intptr_t)_sceneFb->GetColorAttachment(0)->GetID(), s, { 0,1 }, { 1,0 });
		ImGui::End();
		ImGui::PopStyleVar();
	}

	glm::mat4 MakeModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, scale);
		return modelMatrix;
	}
};
