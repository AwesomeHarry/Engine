#include <iostream>
#include <glad/glad.h>
#include <imgui.h>

#include "Core/Root.h"
#include "ECS/Scene/Scene.h"
#include "ECS/Scene/Entity.h"
#include "ECS/Components/Components.h"

#include "Renderer/Shader.h"
#include "Renderer/VertexArrayObject.h"
#include "Util/Mesh/GltfIO.h"

#include "UI/WindowInfoUI_ImGui.h"

const char* vertexShaderSource = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;
void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)glsl";

class SandboxLayer : public Engine::Layer {
private:
	std::shared_ptr<Engine::Scene> _scene;

	float _color[3] = { 0.06f, 0.06f, 0.06f };
	std::shared_ptr<Engine::Shader> _shader;

	std::shared_ptr<Engine::VertexArrayObject> _vao;

	std::unique_ptr<Engine::WindowInfoUI_ImGui> _windowUI;

	bool _renderWireframe = false;
public: 
	void OnAttach() override {
		_windowUI = std::make_unique<Engine::WindowInfoUI_ImGui>(*_attachedWindow);

		_scene = std::make_unique<Engine::Scene>();

		_shader = std::make_unique<Engine::Shader>(vertexShaderSource, fragmentShaderSource);

		/* Manual Mesh */
		std::shared_ptr<Engine::Mesh> mesh = std::make_shared<Engine::Mesh>("Quad");
		{
			#pragma region Data

			float vertices[] = {
				-0.5f,	-0.5f,	0,0,0,
				0.5f,	-0.5f,	0,1,0,
				-0.5f,	0.5f,	0,0,1,
				0.5f,	0.5f,	0,1,1
			};

			unsigned short indices[] = {
				0, 1, 2,
				2, 1, 3
			};

			#pragma endregion

			// Create vertex array using raw data
			std::shared_ptr<Engine::VertexArrayObject> vertexArray;
			auto vertexBuffer = std::make_shared<Engine::VertexBufferObject>();
			vertexBuffer->SetData(vertices, 4, {
				{ "POSITION", Engine::LType::Float, 3},
				{ "TEXCOORDS", Engine::LType::Float, 2}
								  });

			auto indexBuffer = std::make_shared<Engine::IndexBufferObject>();
			indexBuffer->SetData(indices, Engine::LType::UnsignedShort, 6);

			vertexArray = std::make_shared<Engine::VertexArrayObject>();
			vertexArray->AddVertexBuffer(vertexBuffer);
			vertexArray->SetIndexBuffer(indexBuffer);
			vertexArray->Compute();

			mesh->AddSubmesh(vertexArray);
		}

		/* Add mesh to scene */
		Engine::Entity entity = _scene->CreateEntity();
		entity.AddComponent<Engine::MeshFilterComponent>(mesh);
		entity.AddComponent<Engine::MeshRendererComponent>(_shader);

		/* Gltf Mesh*/
		{
			auto model = Engine::GltfIO::LoadFile("Resources/Suzanne/glTF/Suzanne.gltf");
			_vao = Engine::GltfIO::LoadPrimitive(model, model.meshes[0].primitives[0]);

			/*_vao = std::make_shared<Engine::VertexArrayObject>();
			auto vbo = std::make_shared<Engine::VertexBufferObject>();
			auto ibo = std::make_shared<Engine::IndexBufferObject>();

			if (model.meshes[0].primitives[0].indices != -1) {
				auto indAccessor = model.accessors[model.meshes[0].primitives[0].indices];
				const tinygltf::BufferView& bufferView = model.bufferViews[indAccessor.bufferView];
				const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

				const unsigned char* data = &buffer.data[bufferView.byteOffset + indAccessor.byteOffset];

				//const unsigned char* conv = reinterpret_cast<const unsigned char*>(data);
				//for (int i = 0; i < indAccessor.count; i++)
				//	ENGINE_TRACE(conv[i]);

				int stride = indAccessor.ByteStride(bufferView);
				// Assums tightly packed
				if (bufferView.byteStride == 0)
					ibo->SetData(data, (Engine::LType)indAccessor.componentType, indAccessor.count);
				//else
				//	ibo->SetData(data, stride * indAccessor.count);

				_vao->SetIndexBuffer(ibo);
			}
			{
				auto posAccessor = model.accessors[model.meshes[0].primitives[0].attributes["POSITION"]];

				const tinygltf::BufferView& bufferView = model.bufferViews[posAccessor.bufferView];
				const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

				const unsigned char* data = &buffer.data[bufferView.byteOffset + posAccessor.byteOffset];

				uint32_t numComponents = tinygltf::GetNumComponentsInType(posAccessor.type);
				int stride = posAccessor.ByteStride(bufferView);

				// TODO: Fix pointer (interleaved doesnt work atm)
				const void* pointer = (const void*)posAccessor.byteOffset;

				//const float* conv = reinterpret_cast<const float*>(data);
				//for (int i = 0; i < posAccessor.count * numComponents; i += 6)
				//	ENGINE_TRACE("[{}] \t {}, {}, {}", i, conv[i], conv[i + 1], conv[i + 2]);

				Engine::VertexLayout layout{
					{ "POSITION", (Engine::LType)posAccessor.componentType,numComponents}
				};
				//vbo->SetData(data, stride * posAccessor.count, layout);
				vbo->SetData(data, posAccessor.count, layout);
				_vao->AddVertexBuffer(vbo);
			}
			_vao->Compute();*/
		}
	}

	void OnUpdate(float ts) override {
		_scene->UpdateScene(ts);
		_scene->RenderScene();

		ImGui::Begin("Window");
		_windowUI->RenderUI();
		ImGui::End();

		//_shader->Bind();
		//_vertexArray->Bind();
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		//
		//_vao->Bind();
		//if (_vao->HasIndices())
		//	glDrawElements(GL_TRIANGLES, _vao->GetCount(), (GLenum)_vao->GetIndexBuffer().GetType(), 0);
		//else
		//	glDrawArrays(GL_TRIANGLES, 0, _vao->GetCount());

		/*
		 *	UI
		 */

		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Clear Color", &_color[0]);
		_renderer->SetClearColor(_color[0], _color[1], _color[2], 1);

		if (ImGui::Checkbox("Wireframe", &_renderWireframe)) {
			_renderer->SetRenderMode(_renderWireframe ? Engine::WireframeMode::Line : Engine::WireframeMode::Fill);
		}
		ImGui::End();
	}
};

int main(int argc, char** argv) {
	auto root = std::make_unique<Engine::Root>();
	if (!root->Initialize())
		return 1;

	Engine::WindowManager& windowManager = root->GetWindowManager();

	Engine::GraphicsContext graphicsContext = {};
	graphicsContext.EnableDepthTest = true;
	graphicsContext.CullBack = false;

	auto window = windowManager.CreateNewWindow({ "Sandbox", 1280, 720 }, graphicsContext);

	window->Subscribe<Engine::WindowResizeEvent>([](const Engine::WindowResizeEvent& event) {
		ENGINE_INFO("Window resized to {}x{}", event.GetWidth(), event.GetHeight());
	});

	auto layer = std::make_shared<SandboxLayer>();
	window->PushLayer(layer);

	root->Run();
	root->Shutdown();
	return 0;
}
