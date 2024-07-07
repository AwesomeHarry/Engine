#pragma once
#include <string>
#include "Rendering/Platform/Buffer/BufferCommon.h"

namespace Engine::UIUtil {
	std::string LTypeToString(LType type) {
		switch (type) {
		case LType::Byte:
			return "Byte";
		case LType::UnsignedByte:
			return "Unsigned Byte";
		case LType::Short:
			return "Short";
		case LType::UnsignedShort:
			return "Unsigned Short";
		case LType::Int:
			return "Int";
		case LType::UnsignedInt:
			return "Unsigned Int";
		case LType::Float:
			return "Float";
		default:
			return "Unknown";
		}
	}
}