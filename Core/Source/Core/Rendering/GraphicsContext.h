#pragma once

#include <cstdint>

#include "IRenderPipeline.h"

namespace Engine {
	enum class BlendFactor : uint32_t {
		ZERO = 0,
		ONE = 1,
		SRC_COLOR = 0x0300,
		ONE_MINUS_SRC_COLOR = 0x0301,
		SRC_ALPHA = 0x0302,
		ONE_MINUS_SRC_ALPHA = 0x0303,
		DST_COLOR = 0x0306,
		ONE_MINUS_DST_COLOR = 0x0307,
		DST_ALPHA = 0x0304,
		ONE_MINUS_DST_ALPHA = 0x0305,
	};

	struct GraphicsContext {
		bool EnableDepthTest = false;
		bool EnableBlend = false;
		BlendFactor SrcBlendFactor = BlendFactor::ONE;
		BlendFactor DstBlendFactor = BlendFactor::ZERO;
		bool EnableCullFace = false;
		bool CullFront = false;
		bool CullBack = false;
	};
}