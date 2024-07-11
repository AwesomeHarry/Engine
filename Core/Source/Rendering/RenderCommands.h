#pragma once
#include <cstdint>
#include <vector>
#include <Util/FlagSet.h>
#include "BufferBit.h"

namespace Engine {
	enum class WireframeMode : uint32_t {
		Fill = 0x1B02,
		Line = 0x1B01,
		Point = 0x1B00
	};

	class Mesh;
	class Shader;
	class VertexArrayObject;

	class RenderCommands {
	public:
		static void SetClearColor(float r, float g, float b, float a = 1.0f);
		static void ClearBuffers(const FlagSet<BufferBit>& bufferBits);
		static void SetViewport(int x, int y, int width, int height);

		static void SetWireframe(WireframeMode wireframeMode);

		static void RenderMesh(const Mesh& mesh, const Shader& shader);
		static void RenderPoints(const VertexArrayObject& vertexArray, uint64_t count, const Shader& shader);
	};
}