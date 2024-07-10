#pragma once

namespace Engine {
	enum class BufferBit {
		Color = 0x00004000,
		Depth = 0x00000100,
		Stencil = 0x00000400
	};
}
