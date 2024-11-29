#pragma once
#include "BaseComponent.h"

#include <glm/vec3.hpp>
#include <glm/common.hpp>

#include "Rendering/Platform/Mesh.h"
#include <spdlog/stopwatch.h>

namespace Engine {
	struct BoundingBoxComponent : public BaseComponent {
		glm::vec3 min, max;

		BoundingBoxComponent() : min(0), max(0) {}
		~BoundingBoxComponent() {}

		void GrowToInclude(const glm::vec3& point) {
			min = glm::min(min, point);
			max = glm::max(max, point);
		}

		void GrowToInclude(const BoundingBoxComponent& other) {
			min = glm::min(min, other.min);
			max = glm::max(max, other.max);
		}

		void GrowToInclude(const Mesh& mesh) {
			spdlog::stopwatch sw;
			uint32_t sCount = mesh.GetSubmeshCount();
			for (uint32_t sIndex = 0; sIndex < sCount; sIndex++) {
				const auto& vbo = mesh.GetSubmesh(sIndex).GetVertexBuffer(0);
				const auto& points = vbo.GetData<glm::vec3>();

				for (int i = 0; i < points.size(); i++) {
					GrowToInclude(points[i]);
				}
			}
		}

		glm::vec3 GetCenter() { return (min + max) / 2.0f; }

		glm::vec3 GetSize() { return max - min; }
	};
}
