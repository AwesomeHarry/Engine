#pragma once

namespace Engine {
	enum class LType {
		Byte = 0x1400,
		UnsignedByte = 0x1401,
		Short = 0x1402,
		UnsignedShort = 0x1403,
		Int = 0x1404,
		UnsignedInt = 0x1405,
		Float = 0x1406
	};

	static size_t GetLTypeSize(LType size) {
		switch (size) {
		case LType::Byte: 
		case LType::UnsignedByte: 
			return sizeof(char);
		case LType::Short:
		case LType::UnsignedShort:
			return sizeof(short);
		case LType::Int:
		case LType::UnsignedInt:
			return sizeof(int);
		case LType::Float:
			return sizeof(float);
		default:
			return 0;
		}
	}

	enum class BufferUsage {
		Static = 0x88E4,
		Dynamic = 0x88E8,
		Stream = 0x88E0
	};
}