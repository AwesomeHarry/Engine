#pragma once

#include <string>
#include <sstream>

#include <tiny_gltf.h>

#include "Rendering/Platform/Buffer/BufferCommon.h"

#include "Rendering/Platform/Buffer/VertexArrayObject.h"
#include "Rendering/Platform/Buffer/VertexBufferObject.h"
#include "Rendering/Platform/Buffer/IndexBufferObject.h"

#include "Rendering/Platform/Mesh.h"

namespace Engine {
	class GltfIO {
	public:
		static tinygltf::Model LoadFile(const std::string& path);
		static std::shared_ptr<VertexArrayObject> LoadPrimitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive);
		static void ExportMeshToGltf(const Mesh& mesh, const std::string& path);
	private:
		static uint32_t getNumComponents(uint32_t type);
		static uint32_t getComponentByteSize(uint32_t componentType);

		template<typename T>
		static std::string dumpBuffer(const unsigned char* data, int count, int numPer, int componentByteSize) {
			std::stringstream ss;
			ss << "Buffer Contents:\n";

			auto cur = &data[0];
			for (size_t i = 0; i < count; i++) {
				ss << "[" << i << "]" << " ";
				for (size_t j = 0; j < numPer; j++, cur += componentByteSize) {
					T val;
					std::memcpy(&val, cur, componentByteSize);
					ss << val << " ";
				}
				ss << "\n";
			}

			return ss.str();
		}
	};
}