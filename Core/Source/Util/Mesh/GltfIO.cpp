#include "GltfIO.h"
#include "Logging/Logging.h"

#include <vector>
#include <map>

using namespace Engine;

tinygltf::Model GltfIO::LoadModel(const std::string& path) {
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);

	if (!warn.empty()) {
		ENGINE_WARN("Warn: {}", warn);
	}

	if (!err.empty()) {
		ENGINE_ERROR("Error: {}", err);
		return {};
	}

	if (!ret) {
		ENGINE_ERROR("Failed to parse glTF");
		return {};
	}

	ENGINE_INFO("Successfully loaded glTF");

	return model;
}

std::shared_ptr<VertexArrayObject> GltfIO::LoadPrimitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive) {
	// Create map of layouts <Accessor(index), <VertexCount, VertexLayout>>
	std::map<uint32_t, std::pair<uint32_t, VertexLayout>> layoutDescriptors{};
	{
		// Create component for each attribute
		std::multimap<uint32_t, std::pair<uint32_t, VertexComponent>> componentMap{};

		for (const auto& [attributeName, index] : primitive.attributes) {
			const tinygltf::Accessor& accessor = model.accessors[index];

			VertexComponent component{
				attributeName,
				(LType)accessor.componentType,
				getNumComponents(accessor.type)
			};
			componentMap.emplace(accessor.bufferView, std::make_pair(accessor.count, component));
		}

		// Fill layout map
		for (auto it = componentMap.begin(); it != componentMap.end();) {
			uint32_t key = it->first;
			VertexLayout layout;

			auto range = componentMap.equal_range(key);
			for (auto rangeIt = range.first; rangeIt != range.second; rangeIt++) {
				VertexComponent component = rangeIt->second.second;
				layout.AddComponent(component);
			}

			uint32_t vertexCount = range.first->second.first;
			layoutDescriptors[key] = std::make_pair(vertexCount, layout);
			it = range.second;
		}
	}

	std::shared_ptr<VertexArrayObject> vao = std::make_shared<VertexArrayObject>();

	// Vertex Buffers
	for (const auto& [bufferViewIndex, layoutDescriptor] : layoutDescriptors) {
		const tinygltf::BufferView& bufferView = model.bufferViews[bufferViewIndex];
		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

		const unsigned char* data = &buffer.data[bufferView.byteOffset];

		std::shared_ptr<VertexBufferObject> vbo = std::make_shared<VertexBufferObject>();
		uint32_t vertexCount = layoutDescriptor.first;
		VertexLayout layout = layoutDescriptor.second;
		vbo->SetData(data, vertexCount, layout);
		vao->AddVertexBuffer(vbo);
	}

	// Index Buffers
	{
		const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
		const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

		const unsigned char* data = &buffer.data[bufferView.byteOffset + accessor.byteOffset];

		std::shared_ptr<IndexBufferObject> ibo = std::make_shared<IndexBufferObject>();
		ibo->SetData(data, (LType)accessor.componentType, (uint32_t)accessor.count);
		vao->SetIndexBuffer(ibo);
	}

	vao->Compute();
	vao->SetDrawMode((DrawMode)primitive.mode);

	return vao;
}

std::shared_ptr<Mesh> Engine::GltfIO::LoadMesh(const tinygltf::Model& model, uint32_t meshIndex) {
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

	if (meshIndex >= model.meshes.size()) {
		ENGINE_ERROR("Mesh index out of bounds");
		return nullptr;
	}

	auto& gltfMesh = model.meshes[meshIndex];
	for (auto& gltfPrimitive : gltfMesh.primitives) {
		auto va = LoadPrimitive(model, gltfPrimitive);
		mesh->AddSubmesh(va);
	}

	return mesh;
}

int GetTinyGLTFComponentType(LType type) {
	switch (type) {
	case LType::Byte:
		return TINYGLTF_COMPONENT_TYPE_BYTE;
	case LType::UnsignedByte:
		return TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;
	case LType::Short:
		return TINYGLTF_COMPONENT_TYPE_SHORT;
	case LType::UnsignedShort:
		return TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
	case LType::Int:
		return TINYGLTF_COMPONENT_TYPE_INT;
	case LType::UnsignedInt:
		return TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT;
	case LType::Float:
		return TINYGLTF_COMPONENT_TYPE_FLOAT;
	default:
		return -1;
	}
}

int GetTinyGLTFType(int componentCount) {
	switch (componentCount) {
	case 1:
		return TINYGLTF_TYPE_SCALAR;
	case 2:
		return TINYGLTF_TYPE_VEC2;
	case 3:
		return TINYGLTF_TYPE_VEC3;
	case 4:
		return TINYGLTF_TYPE_VEC4;
	default:
		return -1;
	}
}

int GetTinyGLTFDrawMode(DrawMode mode) {
	switch (mode) {
	case DrawMode::Points:
		return TINYGLTF_MODE_POINTS;
	case DrawMode::Lines:
		return TINYGLTF_MODE_LINE;
	case DrawMode::LineLoop:
		return TINYGLTF_MODE_LINE_LOOP;
	case DrawMode::LineStrip:
		return TINYGLTF_MODE_LINE_STRIP;
	case DrawMode::Triangles:
		return TINYGLTF_MODE_TRIANGLES;
	case DrawMode::TriangleStrip:
		return TINYGLTF_MODE_TRIANGLE_STRIP;
	case DrawMode::TriangleFan:
		return TINYGLTF_MODE_TRIANGLE_FAN;
	default:
		return -1;
	}
}

void GltfIO::ExportMeshToGltf(const Mesh& mesh, const std::string& path) {
	tinygltf::Model model;
	tinygltf::Asset& asset = model.asset;
	asset.version = "2.0";
	asset.generator = "Mesh to GLTF Exporter";

	for (uint32_t i = 0; i < mesh.GetSubmeshCount(); i++) {
		const VertexArrayObject& vao = mesh.GetSubmesh(i);
		tinygltf::Primitive primitive;

		// Vertex Buffers
		for (uint32_t j = 0; j < vao.GetVertexBufferCount(); j++) {
			const VertexBufferObject& vbo = vao.GetVertexBuffer(j);
			const VertexLayout& layout = vbo.GetLayout();

			tinygltf::Accessor accessor;
			tinygltf::BufferView bufferView;
			tinygltf::Buffer buffer;

			buffer.data = vbo.GetRawData();
			bufferView.buffer = (uint32_t)model.buffers.size();
			bufferView.byteOffset = 0;
			bufferView.byteLength = (uint32_t)buffer.data.size();

			for (const auto& component : layout.GetComponents()) {
				accessor.componentType = GetTinyGLTFComponentType(component.Type);
				accessor.count = vbo.GetCount();
				accessor.type = GetTinyGLTFType(component.Count);
				accessor.bufferView = (uint32_t)model.bufferViews.size();
				accessor.byteOffset = 0;

				model.accessors.emplace_back(accessor);
				model.bufferViews.emplace_back(bufferView);
				model.buffers.emplace_back(buffer);

				primitive.attributes[component.Name] = (uint32_t)model.accessors.size() - 1;
			}
		}

		// Index Buffer
		if (vao.HasIndices()) {
			const IndexBufferObject& ibo = vao.GetIndexBuffer();
			tinygltf::Accessor accessor;
			tinygltf::BufferView bufferView;
			tinygltf::Buffer buffer;

			buffer.data = ibo.GetRawData();
			bufferView.buffer = (uint32_t)model.buffers.size();
			bufferView.byteOffset = 0;
			bufferView.byteLength = (uint32_t)buffer.data.size();

			accessor.componentType = GetTinyGLTFComponentType(ibo.GetType());
			accessor.count = ibo.GetCount();
			accessor.type = TINYGLTF_TYPE_SCALAR;
			accessor.bufferView = (uint32_t)model.bufferViews.size();
			accessor.byteOffset = 0;

			model.accessors.emplace_back(std::move(accessor));
			model.bufferViews.emplace_back(std::move(bufferView));
			model.buffers.emplace_back(std::move(buffer));

			primitive.indices = (uint32_t)model.accessors.size() - 1;
		}

		primitive.mode = GetTinyGLTFDrawMode(vao.GetDrawMode());
		model.meshes.emplace_back();
		model.meshes.back().primitives.emplace_back(std::move(primitive));
	}

	tinygltf::TinyGLTF writer;
	writer.WriteGltfSceneToFile(&model, path, true, true, true, false);
}

uint32_t GltfIO::getNumComponents(uint32_t type) {
	return tinygltf::GetNumComponentsInType(type);
}

uint32_t GltfIO::getComponentByteSize(uint32_t componentType) {
	return tinygltf::GetComponentSizeInBytes(componentType);
}
