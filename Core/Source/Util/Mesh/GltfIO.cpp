#include "GltfIO.h"
#include "Logging/Logging.h"

#include <vector>
#include <map>

using namespace Engine;

tinygltf::Model GltfIO::LoadFile(const std::string& path) {
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
		ibo->SetData(data, (LType)accessor.componentType, accessor.count);
		vao->SetIndexBuffer(ibo);
	}

	vao->Compute();

	return vao;
}

//const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
//const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
//const unsigned char* data = &buffer.data[bufferView.byteOffset + accessor.byteOffset];

uint32_t Engine::GltfIO::getNumComponents(uint32_t type) {
	return tinygltf::GetNumComponentsInType(type);
}

uint32_t Engine::GltfIO::getComponentByteSize(uint32_t componentType) {
	return tinygltf::GetComponentSizeInBytes(componentType);
}
