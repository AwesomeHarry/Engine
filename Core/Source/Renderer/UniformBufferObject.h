#pragma once
#include <cstdint>

namespace Engine {
	class UniformBufferObject {
	public:
		UniformBufferObject(uint32_t size, uint32_t bindingPoint);
		~UniformBufferObject();

		void Bind();
		void Unbind();
		void SetData(const void* data, uint32_t size, uint32_t offset = 0);
	private:
		uint32_t _id;
		uint32_t _bindingPoint;
	};
}