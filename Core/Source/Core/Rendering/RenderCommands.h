#pragma once
#include <cstdint>
#include <initializer_list>

namespace Engine {
	enum class BufferBit : uint32_t {
		Color = 0x00004000,
		Depth = 0x00000100,
		Stencil = 0x00000400
	};

	enum class WireframeMode : uint32_t {
		Fill = 0x1B02,
		Line = 0x1B01,
		Point = 0x1B00
	};

	class RenderCommands {
	public:
		static void SetClearColor(float r, float g, float b, float a);
		static void ClearBuffers(const std::initializer_list<BufferBit>& bufferBits);
		static void SetWireframe(WireframeMode wireframeMode);
	};
}