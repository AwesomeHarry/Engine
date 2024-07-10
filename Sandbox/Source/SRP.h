#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "ECS/Scene/Scene.h"
#include "Rendering/BaseRenderPipeline.h"
#include "Rendering/Platform/Buffer/UniformBufferObject.h"
#include "Rendering/Platform/Framebuffer.h"
#include "Rendering/RenderManager.h"
#include "ECS/Components/Native/Components.h"

class SRP : public Engine::BaseRenderPipeline {
private:
	struct CameraData { glm::mat4 projection; glm::mat4 view; };
	std::shared_ptr<Engine::UniformBufferObject> _cameraDataUbo;
public:
	virtual void Initialize(std::shared_ptr<Engine::Framebuffer> mainFramebuffer) {
		Engine::BaseRenderPipeline::Initialize(mainFramebuffer);
		_cameraDataUbo = std::make_shared<Engine::UniformBufferObject>(sizeof(CameraData), 0, Engine::BufferUsage::Dynamic);
	}

	void RenderScene(Engine::Scene& scene) {
		/* Set render targets from cameras */
		{}

		/* RenderOpaqueObjects(scene, camera); */
		/* RenderTransparentObjects(scene, camera); */

		/* ApplyPostProcessing(); */

	}

	virtual void RenderScene(Engine::Scene& scene, Engine::Entity& cameraEntity) override {
		auto& camera = cameraEntity.GetComponent<Engine::CameraComponent>();
		auto& framebuffer = camera.renderTarget == Engine::CameraComponent::RenderTarget::Window ? _mainFb : camera.renderFramebuffer;

		framebuffer->Bind();

		Engine::RenderCommands::SetClearColor(camera.backgroundColor.r, camera.backgroundColor.g,camera.backgroundColor.b	);
		Engine::RenderCommands::ClearBuffers(camera.clearFlags);

		CameraData cameraData;
		float aspect = (float)framebuffer->GetSpecification().width / (float)framebuffer->GetSpecification().height;
		cameraData.projection = camera.GetProjectionMatrix(aspect);
		cameraData.view = camera.CalculateViewMatrix(cameraEntity.GetTransform());

		_cameraDataUbo->Bind();
		_cameraDataUbo->SetData(&cameraData, sizeof(CameraData), 0);


		RenderOpaqueObjects(scene);
		RenderDebugMeshes(scene);

		framebuffer->Unbind();
	}

	void RenderOpaqueObjects(Engine::Scene& scene) {
		auto& reg = scene.GetRegistry();

		/* Render Meshes */
		{
			auto view = reg.view<const Engine::TransformComponent, const Engine::MeshFilterComponent, const Engine::MeshRendererComponent>();
			for (auto entity : view) {
				auto& filter = view.get<Engine::MeshFilterComponent>(entity);
				auto& renderer = view.get<Engine::MeshRendererComponent>(entity);
				auto& transform = view.get<Engine::TransformComponent>(entity);

				auto& shader = *renderer.shader;
				auto& mesh = *filter.mesh;

				glm::mat4 model = transform.GetTransformMatrix();
				shader.SetUniform("model", model);

				Engine::RenderCommands::RenderMesh(mesh, shader);
			}
		}

	}

	void RenderDebugMeshes(Engine::Scene& scene) {
		auto& reg = scene.GetRegistry();

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
	}
};