#pragma once
#include <cstdint>
#include "Buffer.h"

namespace Engine {
	class IndexBufferObject {
	public:
		IndexBufferObject(BufferUsage usage = BufferUsage::Static);
		~IndexBufferObject();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, LType type, uint64_t count);

		inline uint64_t GetCount() const { return _count; }
		inline LType GetType() const { return _type; }
	private:
		uint32_t _id;
		BufferUsage _usage;
		LType _type;
		uint64_t _count;
	};
}