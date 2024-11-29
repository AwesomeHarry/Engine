#pragma once
#include <cstdint>
#include <vector>
#include "BufferCommon.h"

namespace Engine {
	class IndexBufferObject {
	public:
		IndexBufferObject(BufferUsage usage = BufferUsage::Static);
		~IndexBufferObject();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, LType type, uint32_t count);

		std::vector<uint8_t> GetRawData() const;

		inline uint32_t GetCount() const { return _count; }
		inline LType GetType() const { return _type; }
	private:
		uint32_t _id;
		BufferUsage _usage;
		LType _type;
		uint32_t _count;
	};
}