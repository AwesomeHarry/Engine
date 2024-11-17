#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Core/Application/Root.h"

#include "Project/Scene/Scene.h"
#include "Project/Scene/Entity.h"

#include "Rendering/Platform/Buffer/UniformBufferObject.h"
#include "Rendering/Platform/Texture2D.h"
#include "Rendering/Platform/TextureCubeMap.h"

#include "Util/Mesh/GltfIO.h"

#include "UI/WindowInfoUI_ImGui.h"
#include "UI/SceneHeirarchyUI_ImGui.h"
#include "UI/EntityPropertiesUI_ImGui.h"
#include "UI/MaterialUI_ImGui.h"

#include "FPSCameraController.h"
#include "OrbitCameraController.h"

#include "SRP.h"

#include "Project/Project.h"
#include "Project/AssetSystem.h"
#include "Project/Assets/ShaderAsset.h"
#include "Project/Assets/TextureAsset.h"
#include "Project/Assets/MeshAsset.h"
#include "Project/Assets/MaterialAsset.h"
#include "Project/Assets/SceneAsset.h"

class SandboxLayer : public Engine::Layer {
private:
	std::unique_ptr<Engine::Project> _project;
	std::shared_ptr<Engine::SceneAsset> _sceneAsset;
	std::shared_ptr<SRP> _standardRenderPipeline;

	std::shared_ptr<Engine::Material> _pbrMaterial;

	Engine::Entity _camera;
	FPSCameraController _fpsCameraController;
	OrbitCameraController _orbitCameraController;

	bool _renderWireframe = false;
public:
	void OnAttach() override {
		/* Create Project */
		_project = std::make_unique<Engine::Project>("TestProject", std::filesystem::current_path() / "Projects/");

		/* Get Assets */
		_sceneAsset = Engine::AssetRef(Engine::GUID("d8385d392e703a6f378d9da2e943a73c")).Resolve<Engine::SceneAsset>(_project->GetAssetBank());

		/* Create Standard Render Pipeline */
		_standardRenderPipeline = std::make_shared<SRP>();
		{
			Engine::Framebuffer::FramebufferSpec fbSpec;
			fbSpec.width = 1280;
			fbSpec.height = 720;
			fbSpec.attachments = { Engine::ImageFormat::RGB8 };
			fbSpec.includeDepthStencil = true;

			auto mainFramebuffer = std::make_shared<Engine::Framebuffer>(fbSpec);
			_standardRenderPipeline->Initialize(mainFramebuffer);
		}

		/* Set Render Pipeline */
		_camera = _sceneAsset->GetInternal()->GetEntity("Camera");
		_camera.GetComponent<Engine::CameraComponent>().renderPipeline = _standardRenderPipeline;

		/* Create Spheres */
		auto sphereMeshAsset = Engine::AssetRef(Engine::GUID("a3a5f8998cef2f901f84d716491dbb45")).Resolve<Engine::MeshAsset>(_project->GetAssetBank());
		auto sphereMaterialAsset = Engine::AssetRef(Engine::GUID("da42cc67d876c4dd408c17b052483920")).Resolve<Engine::MaterialAsset>(_project->GetAssetBank());
		auto scene = _sceneAsset->GetInternal();

		for (int i = 0; i < 10; i++) {
			auto sphere = scene->CreateEntity("Sphere_"+std::to_string(i));
			sphere.AddComponent<Engine::MeshFilterComponent>(sphereMeshAsset);
			auto& mr = sphere.AddComponent<Engine::MeshRendererComponent>(sphereMaterialAsset);
			sphere.GetTransform().position.x = i * 2.0f - 10.0f;
		}

		/* Create Irradiance Map */
		std::shared_ptr<Engine::TextureCubemap> irradianceCubemap;
		{
			glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
			glm::mat4 captureViews[] = {
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
			};

			auto skyboxAsset = _camera.GetComponent<Engine::CameraComponent>().skyboxCubemap;
			auto skyboxCubemap = skyboxAsset->GetInternal();

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
			auto cubeVa = std::make_shared<Engine::VertexArrayObject>();
			auto skyboxVbo = std::make_shared<Engine::VertexBufferObject>(Engine::BufferUsage::Static);
			skyboxVbo->SetData(skyboxVertices, 36, {
				{ "aPos", Engine::LType::Float, 3 }
							   });
			cubeVa->AddVertexBuffer(skyboxVbo);
			cubeVa->Compute();
			auto cubeMesh = std::make_shared<Engine::Mesh>();
			cubeMesh->AddSubmesh(cubeVa);

			auto irradianceShader = Engine::Shader::Utils::FromFile("Resources/Shaders/SkyboxConvolution.vert", "Resources/Shaders/SkyboxConvolution.frag");
			auto irradianceMaterial = std::make_shared<Engine::Material>(irradianceShader);
			irradianceMaterial->SetTexture("environmentMap", skyboxCubemap);
			irradianceMaterial->SetUniform("projection", captureProjection);

			uint32_t captureSize = 32;

			auto textureSpec = Engine::TextureSpec{
				captureSize,
				captureSize,
				Engine::ImageFormat::RGB16F
			};
			irradianceCubemap = std::make_shared<Engine::TextureCubemap>(textureSpec);

			auto framebufferSpec = Engine::Framebuffer::FramebufferSpec{
				captureSize,
				captureSize,
				{ Engine::ImageFormat::RGB16F },
				true
			};
			auto captureFrambuffer = std::make_shared<Engine::Framebuffer>(framebufferSpec);

			captureFrambuffer->Bind();
			for (uint32_t i = 0; i < 6; i++) {
				//captureFrambuffer->ModifyColorAttachment(0, (Engine::TextureTarget)((int)Engine::TextureTarget::CubemapPosX + i), *cubemap);
				irradianceMaterial->SetUniform("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceCubemap->GetInstanceID(), 0);

				Engine::RenderCommands::ClearBuffers(Engine::BufferBit::Color | Engine::BufferBit::Depth);
				Engine::RenderCommands::RenderMesh(*cubeMesh, *irradianceMaterial);
			}
			captureFrambuffer->Unbind();
		}

		// Set Irradiance Map
		auto materialAsset = Engine::AssetRef(Engine::GUID("da42cc67d876c4dd408c17b052483920")).Resolve<Engine::MaterialAsset>(_project->GetAssetBank());
		_pbrMaterial = materialAsset->GetInternal();
		_pbrMaterial->SetTexture("irradianceMap", irradianceCubemap);
		
		// Camera Controls
		_fpsCameraController = FPSCameraController(_inputManager);
		_orbitCameraController = OrbitCameraController(_inputManager);
		_window->Subscribe<Engine::WindowMouseScrolledEvent>([&](const Engine::WindowMouseScrolledEvent& e) {
			_orbitCameraController.OnScroll((float)e.yOffset);
		});

		/* Set Custom Uniform Widgets */
		Engine::UI::MaterialUI::CustomUniformWidgets["albedo"] = { Engine::UI::MaterialUI::WidgetType::Color };
		Engine::UI::MaterialUI::CustomUniformWidgets["roughness"] = { Engine::UI::MaterialUI::WidgetType::Drag, 0.025f, 0.0f, 1.0f };
		Engine::UI::MaterialUI::CustomUniformWidgets["metallic"] = { Engine::UI::MaterialUI::WidgetType::Drag, 0.025f, 0.0f, 1.0f };
		Engine::UI::MaterialUI::CustomUniformWidgets["ao"] = { Engine::UI::MaterialUI::WidgetType::Drag, 0.025f, 0.0f, 1.0f };
		Engine::UI::MaterialUI::CustomUniformWidgets["lightColor"] = { Engine::UI::MaterialUI::WidgetType::Color };
	}

	void OnDetach() override {
		_project->SaveAllAssets();
	}

	void OnUpdate(float ts) override {
		/* Update anything as required */
		_orbitCameraController.OnUpdate(_camera.GetTransform(), ts);

		/* Render Axis */
		_sceneAsset->GetInternal()->GetDebugRenderer().DrawLine({{0,0,0}, {1,0,0}, {1,0,0,1}});
		_sceneAsset->GetInternal()->GetDebugRenderer().DrawLine({ { 0,0,0 }, { 0,1,0 }, { 0,1,0,1 } });
		_sceneAsset->GetInternal()->GetDebugRenderer().DrawLine({ { 0,0,0 }, { 0,0,1 }, { 0,0,1,1 } });

		/* Update Scene */
		_sceneAsset->GetInternal()->UpdateScene(ts);

		_pbrMaterial->SetUniform("camPos", _camera.GetTransform().position);

		/* Render Scene */
		_sceneAsset->GetInternal()->RenderScene();

		/* Render UI */
		{
			//ImGui::Image((ImTextureID)(intptr_t)test_texture->GetInstanceID(), { 100,100 }, { 0,1 },{1,0});

			/* Viewport */
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::Begin("Scene");
				auto s = ImGui::GetContentRegionAvail();
				auto& mainFramebuffer = _standardRenderPipeline->GetMainFramebuffer();
				if (mainFramebuffer.GetSpecification().width != s.x ||
					mainFramebuffer.GetSpecification().height != s.y)
					_standardRenderPipeline->OnResize((uint32_t)s.x, (uint32_t)s.y);
				ImGui::Image((ImTextureID)(intptr_t)mainFramebuffer.GetColorAttachment(0)->GetInstanceID(), s, { 0,1 }, { 1,0 });
				ImGui::End();
				ImGui::PopStyleVar();
			}

			ImGui::Image((ImTextureID)(intptr_t)_standardRenderPipeline->_windowFramebuffer->GetColorAttachment(0)->GetInstanceID(), ImGui::GetContentRegionAvail(), { 0,1 }, { 1,0 });
			//ImGui::Image((ImTextureID)(intptr_t)_standardRenderPipeline->GetMainFramebuffer().GetDepthAttachment()->GetID(), ImGui::GetContentRegionAvail(), { 0,1 }, { 1,0 });

			ImGui::Begin("Window");
			Engine::WindowInfoUI_ImGui::RenderUI(*_window);
			ImGui::End();

			ImGui::Begin("Properties");
			Engine::EntityPropertiesUI_ImGui::RenderUI(Engine::SceneHeirarchyUI_ImGui::GetSelectedEntity());
			ImGui::End();

			ImGui::Begin("Settings");
			if (ImGui::Checkbox("Wireframe", &_renderWireframe)) {
				_renderManager->SetWireframeMode(_renderWireframe ? Engine::WireframeMode::Line : Engine::WireframeMode::Fill);
			}
			ImGui::Text("Delta Time: %.6fms", ts);
			ImGui::Text("FPS: %.1f", 1.0f / ts);
			glm::vec2 mp = _inputManager->GetMousePosition();
			ImGui::Text("Mouse Position: %.1f,%.1f", mp.x, mp.y);

			ImGui::Separator();
			FPSCameraControllerUI_ImGui::RenderUI(_fpsCameraController);

			ImGui::End();

			ImGui::Begin("Scene Info");
			Engine::SceneHeirarchyUI_ImGui::RenderUI(*_sceneAsset->GetInternal());
			ImGui::End();
		}
	}
};

int main(int argc, char** argv) {
	auto root = std::make_unique<Engine::Root>();

	Engine::WindowSpec windowSpec{};
	windowSpec.Title = "Sandbox";
	windowSpec.Width = 1280;
	windowSpec.Height = 720;

	Engine::GraphicsContext graphicsContext{};
	graphicsContext.EnableDepthTest = true;
	graphicsContext.EnableBlend = true;
	graphicsContext.EnableCullFace = true;
	graphicsContext.CullBack = true;

	if (!root->Initialize(windowSpec, graphicsContext))
		return 1;

	auto layer = std::make_shared<SandboxLayer>();
	root->PushLayer(layer);

	root->Run();
	root->Shutdown();
	return 0;
}
