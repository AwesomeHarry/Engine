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
uniform float exposure = 1.0f;

const float gamma = 2.2;

void main()
{
    vec3 hdrColor = texture(skybox, TexCoords).rgb;
	hdrColor *= exposure;
    FragColor = vec4(hdrColor, 1.0);

    //vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    //mapped = pow(mapped, vec3(1.0 / gamma));
}
)";

#pragma endregion

#pragma region Post Process Shader

const char* postProcessVertexShader = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
out vec2 vTex;
void main()
{
	vTex = aPos;
	gl_Position = vec4(aPos * 2.0 - vec2(1.0), 0.0, 1.0);
}
)";

const char* postProcessFragmentShader = R"(
#version 330 core
out vec4 FragColor;
in vec2 vTex;
uniform sampler2D screenMap;
uniform float exposure;
void main()
{
	vec3 inColor = texture(screenMap, vTex).rgb;
	vec3 exposureMapped = vec3(1.0) - exp(-inColor * exposure);
	vec3 gammaMapped = pow(exposureMapped, vec3(1.0 / 2.2));

	FragColor = vec4(gammaMapped, 1.0);
}
)";

#pragma endregion

class SRP : public Engine::BaseRenderPipeline {
public:
	std::shared_ptr<Engine::Framebuffer> _windowFramebuffer;
	std::shared_ptr<Engine::Mesh> _fullscreenQuadMesh;
	std::shared_ptr<Engine::Material> _postProcessMaterial;

private:
	struct CameraData { glm::mat4 projection; glm::mat4 view; };
	std::shared_ptr<Engine::UniformBufferObject> _cameraDataUbo;
	std::shared_ptr<Engine::Shader> _skyboxShader;
	std::shared_ptr<Engine::VertexArrayObject> _skyboxVao;
public:
	virtual void Initialize(std::shared_ptr<Engine::Framebuffer> mainFramebuffer) {
		Engine::BaseRenderPipeline::Initialize(mainFramebuffer);

		Engine::Framebuffer::FramebufferSpec fbSpec;
		fbSpec.width = 1280;
		fbSpec.height = 720;
		fbSpec.attachments = { Engine::ImageFormat::RGB16F };
		fbSpec.includeDepthStencil = true;
		_windowFramebuffer = std::make_shared<Engine::Framebuffer>(fbSpec);

		float fullscreenQuadVertices[] = { 0,0, 1,0, 0,1, 1,1 };
		auto fullscreenQuadVbo = std::make_shared<Engine::VertexBufferObject>(Engine::BufferUsage::Static);
		fullscreenQuadVbo->SetData(fullscreenQuadVertices, 4, { { "POSITION",Engine::LType::Float,2 } });
		auto fullscreenQuadVao = std::make_shared<Engine::VertexArrayObject>();
		fullscreenQuadVao->AddVertexBuffer(fullscreenQuadVbo);
		fullscreenQuadVao->Compute();
		fullscreenQuadVao->SetDrawMode(Engine::DrawMode::TriangleStrip);
		_fullscreenQuadMesh = std::make_shared<Engine::Mesh>("Fullscreen Quad");
		_fullscreenQuadMesh->AddSubmesh(fullscreenQuadVao);

		auto postProcessShader = std::make_shared<Engine::Shader>();
		postProcessShader->AttachVertexShader(postProcessVertexShader);
		postProcessShader->AttachFragmentShader(postProcessFragmentShader);
		postProcessShader->Link();
		_postProcessMaterial = std::make_shared<Engine::Material>(postProcessShader);
		_postProcessMaterial->SetUniform("exposure", 1.0f);
		_postProcessMaterial->AddTexture(_windowFramebuffer->GetColorAttachment(0), "screenMap", 0);

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

	virtual void OnResize(uint32_t newWidth, uint32_t newHeight) override {
		_mainFb->Resize(newWidth, newHeight);
		_windowFramebuffer->Resize(newWidth, newHeight);
		_postProcessMaterial->UpdateTexture("screenMap", _windowFramebuffer->GetColorAttachment(0));
	}

	/* Set render target from camera */
	/* RenderOpaqueObjects(scene, camera); */
	/* RenderTransparentObjects(scene, camera); */
	/* ApplyPostProcessing(); */

	virtual void RenderScene(Engine::Scene& scene, Engine::Entity& cameraEntity) override {
		auto& camera = cameraEntity.GetComponent<Engine::CameraComponent>();
		auto& cameraTransform = cameraEntity.GetTransform();
		auto& framebuffer = camera.renderTarget == Engine::CameraComponent::RenderTarget::Window ? _windowFramebuffer : camera.renderFramebuffer;
		CameraData cameraData;
		glm::vec2 viewportSize = { framebuffer->GetSpecification().width, framebuffer->GetSpecification().height };
		float aspect = viewportSize.x / viewportSize.y;
		cameraData.projection = camera.GetProjectionMatrix(aspect);
		cameraData.view = camera.CalculateViewMatrix(cameraTransform);

		framebuffer->Bind();

		glEnable(GL_DEPTH_TEST);
		Engine::RenderCommands::SetClearColor(camera.backgroundColor.r, camera.backgroundColor.g, camera.backgroundColor.b);
		Engine::RenderCommands::ClearBuffers(camera.clearFlags);

		_cameraDataUbo->Bind();
		_cameraDataUbo->SetData(&cameraData, sizeof(CameraData), 0);

		RenderOpaqueObjects(scene, cameraTransform);
		RenderDebugMeshes(scene, viewportSize);


		// Render Skybox
		if (camera.backgroundType == Engine::CameraComponent::BackgroundType::Skybox) {
			_skyboxShader->Bind();
			_skyboxShader->SetUniform("skybox", 0);
			_skyboxShader->SetUniform("projection", cameraData.projection);
			_skyboxShader->SetUniform("view", glm::mat4(glm::mat3(cameraData.view)));
			_skyboxShader->SetUniform("exposure", camera.skyboxExposure);
			camera.skyboxCubemap->Bind(0);

			glDepthMask(GL_FALSE);
			Engine::RenderCommands::RenderMesh(*_skyboxVao, *_skyboxShader);
			glDepthMask(GL_TRUE);
		}

		framebuffer->Unbind();

		// Apply Post Processing
		Engine::MaterialUtils::DrawUniformWidget(*_postProcessMaterial, "exposure");

		_mainFb->Bind();
		Engine::RenderCommands::SetClearColor(0.2f, 0.5f, 0.1f);
		glDisable(GL_DEPTH_TEST);
		Engine::RenderCommands::ClearBuffers(FlagSet(Engine::BufferBit::Color));
		Engine::RenderCommands::RenderMesh(*_fullscreenQuadMesh, *_postProcessMaterial);
		_mainFb->Unbind();
	}

	void RenderOpaqueObjects(Engine::Scene& scene, Engine::TransformComponent& cameraTransform) {
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

				material.SetUniform("model", transform.GetTransformMatrix());

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