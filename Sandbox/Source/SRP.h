#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "Rendering/IRenderPipeline.h"
#include "Rendering/Platform/Buffer/UniformBufferObject.h"
#include "Rendering/Platform/Framebuffer.h"
#include "Rendering/RenderManager.h"
#include "ECS/Components/Components.h"

class SRP : public Engine::IRenderPipeline {
private:
	struct CameraData { glm::mat4 projection; glm::mat4 view; };
	CameraData _cameraData;
	std::shared_ptr<Engine::UniformBufferObject> _cameraDataUbo;
	std::shared_ptr<Engine::Framebuffer> _sceneFb;
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

				auto& dsm = view.get<Engine::DebugShapeManager>(view.front());

				/* Points */
				{
					auto& shader = *dsm.pointShader;
					shader.SetUniform("segments", 20);

					auto& vao = *dsm.pointInfoVao;
					auto& vbo = *dsm.pointInfoVbo;

					size_t pointCount = dsm.pointData.size();
					vbo.SetData(dsm.pointData.data(), pointCount, {
						{ "aPos", Engine::LType::Float, 3 },
						{ "aRadius", Engine::LType::Float, 1 },
						{ "aColor", Engine::LType::Float, 4 }
								});

					Engine::RenderCommands::RenderPoints(vao, pointCount, shader);
				}

				/* Lines */
				{
					auto& shader = *dsm.lineShader;
					shader.SetUniform("thickness", 0.005f);

					auto& vao = *dsm.lineInfoVao;
					auto& vbo = *dsm.lineInfoVbo;

					size_t lineCount = dsm.lineData.size();
					vbo.SetData(dsm.lineData.data(), lineCount, {
						{ "aStartPos", Engine::LType::Float, 3 },
						{ "aEndPos", Engine::LType::Float, 3 },
						{ "aColor", Engine::LType::Float, 4 }
								});

					Engine::RenderCommands::RenderPoints(vao, lineCount, shader);
				}

				/* Cubes */
				{
					auto& shader = *dsm.quadShader;

					auto& vao = *dsm.quadInfoVao;
					auto& vbo = *dsm.quadInfoVbo;

					size_t cubeCount = dsm.quadData.size();
					vbo.SetData(dsm.quadData.data(), cubeCount, {
						{ "aP1", Engine::LType::Float, 3 },
						{ "aP2", Engine::LType::Float, 3 },
						{ "aP3", Engine::LType::Float, 3 },
						{ "aP4", Engine::LType::Float, 3 },
						{ "aColor", Engine::LType::Float, 4 }
								});

					Engine::RenderCommands::RenderPoints(vao, cubeCount, shader);
				}

				dsm.Clear();
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
