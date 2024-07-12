#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "ECS/Scene/Scene.h"
#include "Rendering/BaseRenderPipeline.h"
#include "Rendering/Platform/Buffer/UniformBufferObject.h"
#include "Rendering/Platform/Framebuffer.h"
#include "Rendering/RenderManager.h"
#include "ECS/Components/Native/Components.h"

#pragma region Skybox Shader
const char* skyboxVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
)";

const char* skyboxFragmentShader = R"(
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);
    //FragColor = vec4(TexCoords,1.0);
}
)";

#pragma endregion

class SRP : public Engine::BaseRenderPipeline {
private:
	struct CameraData { glm::mat4 projection; glm::mat4 view; };
	std::shared_ptr<Engine::UniformBufferObject> _cameraDataUbo;
	std::shared_ptr<Engine::Shader> _skyboxShader;
	std::shared_ptr<Engine::VertexArrayObject> _skyboxVao;
public:
	virtual void Initialize(std::shared_ptr<Engine::Framebuffer> mainFramebuffer) {
		Engine::BaseRenderPipeline::Initialize(mainFramebuffer);
		_cameraDataUbo = std::make_shared<Engine::UniformBufferObject>(sizeof(CameraData), 0, Engine::BufferUsage::Dynamic);
		_skyboxShader = std::make_shared<Engine::Shader>();
		_skyboxShader->AttachFragmentShader(skyboxFragmentShader);
		_skyboxShader->AttachVertexShader(skyboxVertexShader);
		_skyboxShader->Link();
	#pragma region Skybox Data
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
	#pragma endregion
		_skyboxVao = std::make_shared<Engine::VertexArrayObject>();
		auto skyboxVbo = std::make_shared<Engine::VertexBufferObject>(Engine::BufferUsage::Static);
		skyboxVbo->SetData(skyboxVertices, 36, {
			{ "aPos", Engine::LType::Float, 3 }
						   });
		_skyboxVao->AddVertexBuffer(skyboxVbo);
		_skyboxVao->Compute();
	}

	void RenderScene(Engine::Scene& scene) {
		/* Set render targets from cameras */
		/* RenderOpaqueObjects(scene, camera); */
		/* RenderTransparentObjects(scene, camera); */
		/* ApplyPostProcessing(); */
	}

	virtual void RenderScene(Engine::Scene& scene, Engine::Entity& cameraEntity) override {
		auto& camera = cameraEntity.GetComponent<Engine::CameraComponent>();
		auto& framebuffer = camera.renderTarget == Engine::CameraComponent::RenderTarget::Window ? _mainFb : camera.renderFramebuffer;

		framebuffer->Bind();

		Engine::RenderCommands::SetClearColor(camera.backgroundColor.r, camera.backgroundColor.g, camera.backgroundColor.b);
		Engine::RenderCommands::ClearBuffers(camera.clearFlags);

		CameraData cameraData;
		glm::vec2 viewportSize = { framebuffer->GetSpecification().width, framebuffer->GetSpecification().height };
		float aspect = viewportSize.x / viewportSize.y;
		cameraData.projection = camera.GetProjectionMatrix(aspect);
		cameraData.view = camera.CalculateViewMatrix(cameraEntity.GetTransform());

		_cameraDataUbo->Bind();
		_cameraDataUbo->SetData(&cameraData, sizeof(CameraData), 0);


		RenderOpaqueObjects(scene);
		RenderDebugMeshes(scene, viewportSize);

		// Render Skybox
		if (camera.backgroundType == Engine::CameraComponent::BackgroundType::Skybox) {
			_skyboxShader->Bind();
			_skyboxShader->SetUniform("skybox", 0);
			_skyboxShader->SetUniform("projection", cameraData.projection);
			_skyboxShader->SetUniform("view", glm::mat4(glm::mat3(cameraData.view)));
			camera.skyboxCubemap->Bind(0);

			glDepthMask(GL_FALSE);
			Engine::RenderCommands::RenderMesh(*_skyboxVao, *_skyboxShader);
			glDepthMask(GL_TRUE);
		}

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

				auto& material = *renderer.material;
				auto& mesh = *filter.mesh;

				glm::mat4 model = transform.GetTransformMatrix();
				material.GetShader().SetUniform("model", model);

				Engine::RenderCommands::RenderMesh(mesh, material);
			}
		}

	}

	void RenderDebugMeshes(Engine::Scene& scene, const glm::vec2& viewportSize) {
		auto& reg = scene.GetRegistry();

		/* Render Debug Shapes */
		auto view = reg.view<Engine::DebugShapeManager>();
		if (view.size() == 0) {
			ENGINE_WARN("No DebugShapeManager in scene!");
			return;
		}

		if (view.size() > 1)
			ENGINE_WARN("More than one DebugShapeManager in scene, only first will be used.");

		auto& dsm = view.get<Engine::DebugShapeManager>(view.front());

		if (!dsm.renderDebugShapes)
			return;

		RenderPoints(dsm, viewportSize);
		RenderLines(dsm);
		RenderQuads(dsm);

		dsm.Clear();
	}

	void RenderPoints(Engine::DebugShapeManager& dsm, const glm::vec2& viewportSize) {
		auto& shader = *dsm.pointShader;
		shader.SetUniform("viewportSize", viewportSize);

		auto& vao = *dsm.pointInfoVao;
		auto& vbo = *dsm.pointInfoVbo;

		size_t pointCount = dsm.pointData.size();
		if (pointCount == 0)
			return;

		vbo.SetData(dsm.pointData.data(), pointCount, {
			{ "aPos", Engine::LType::Float, 3 },
			{ "aRadius", Engine::LType::Float, 1 },
			{ "aColor", Engine::LType::Float, 4 }
					});

		Engine::RenderCommands::RenderPoints(vao, pointCount, shader);
	}

	void RenderLines(Engine::DebugShapeManager& dsm) {
		auto& shader = *dsm.lineShader;
		shader.SetUniform("thickness", 0.005f);

		auto& vao = *dsm.lineInfoVao;
		auto& vbo = *dsm.lineInfoVbo;

		size_t lineCount = dsm.lineData.size();
		if (lineCount == 0)
			return;

		vbo.SetData(dsm.lineData.data(), lineCount, {
			{ "aStartPos", Engine::LType::Float, 3 },
			{ "aEndPos", Engine::LType::Float, 3 },
			{ "aColor", Engine::LType::Float, 4 }
					});

		Engine::RenderCommands::RenderPoints(vao, lineCount, shader);
	}

	void RenderQuads(Engine::DebugShapeManager& dsm) {
		auto& shader = *dsm.quadShader;

		auto& vao = *dsm.quadInfoVao;
		auto& vbo = *dsm.quadInfoVbo;

		size_t quadCount = dsm.quadData.size();
		if (quadCount == 0)
			return;

		vbo.SetData(dsm.quadData.data(), quadCount, {
			{ "aP1", Engine::LType::Float, 3 },
			{ "aP2", Engine::LType::Float, 3 },
			{ "aP3", Engine::LType::Float, 3 },
			{ "aP4", Engine::LType::Float, 3 },
			{ "aColor", Engine::LType::Float, 4 }
					});

		Engine::RenderCommands::RenderPoints(vao, quadCount, shader);
	}
};