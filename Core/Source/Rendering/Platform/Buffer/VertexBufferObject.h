#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "BufferCommon.h"

namespace Engine {
	struct VertexComponent {
		std::string Name;
		LType Type;
		uint32_t Count;

		uint32_t GetByteSize() const {
			return (uint32_t)GetLTypeSize(Type) * Count;
		}
	};

	class VertexLayout {
	public:
		VertexLayout() = default;
		VertexLayout(std::initializer_list<VertexComponent> components)
			: _components(components) {}

		void AddComponent(const VertexComponent& component) {
			_components.push_back(component);
		}

		uint32_t GetStride() const {
			uint32_t stride = 0;
			for (const auto& component : _components)
				stride += component.GetByteSize();
			return stride;
		}

		const std::vector<VertexComponent>& GetComponents() const {
			return _components;
		}

		const VertexComponent& GetComponent(const std::string& name) const {
			for (const auto& component : _components) {
				if (component.Name == name)
					return component;
			}
			return VertexComponent{};
		}
	private:
		std::vector<VertexComponent> _components;
	};

	class VertexBufferObject {
	public:
		static const uint64_t DEFAULT_INITIAL_CAPACITY = 1000 * sizeof(float) * 4;

		VertexBufferObject(BufferUsage usage = BufferUsage::Static, uint64_t initialCapacity = DEFAULT_INITIAL_CAPACITY);
		~VertexBufferObject();

		VertexBufferObject(const VertexBufferObject&) = delete;
		VertexBufferObject& operator=(const VertexBufferObject&) = delete;

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, uint32_t count, const VertexLayout& layout);
		void UpdateSubData(const void* data, uint64_t offset, uint64_t size);

		inline const VertexLayout& GetLayout() const { return _layout; }
		inline uint32_t GetCount() const { return _count; }
		inline uint64_t GetCapacity() const { return _capacity; }

		std::vector<uint8_t> GetRawData() const;

		template <typename T>
		std::vector<T> GetData() const {
			std::vector<uint8_t> rawData = GetRawData();
			size_t elementCount = rawData.size() / sizeof(T);
			std::vector<T> data(elementCount);
			std::memcpy(data.data(), rawData.data(), rawData.size());
			return data;
		}
	private:
		uint32_t _id;
		BufferUsage _usage;
		VertexLayout _layout;
		uint32_t _count;
		uint64_t _capacity;
	};
}