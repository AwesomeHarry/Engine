#pragma once
#include <glm/vec3.hpp>
#include <glm/common.hpp>

struct BoundingBox {
	glm::vec3 min, max;

	BoundingBox() : min(0), max(0) {}
	~BoundingBox() {}

	void GrowToInclude(const glm::vec3& point) {
		min = glm::min(min, point);
		max = glm::max(max, point);
	}

	void GrowToInclude(const BoundingBox& other) {
		min = glm::min(min, other.min);
		max = glm::max(max, other.max);
	}

	std::vector<glm::vec3> GetCorners() {
		std::vector<glm::vec3> corners(8);
		corners[0] = glm::vec3(min.x, min.y, min.z);
		corners[1] = glm::vec3(min.x, min.y, max.z);
		corners[2] = glm::vec3(min.x, max.y, min.z);
		corners[3] = glm::vec3(min.x, max.y, max.z);
		corners[4] = glm::vec3(max.x, min.y, min.z);
		corners[5] = glm::vec3(max.x, min.y, max.z);
		corners[6] = glm::vec3(max.x, max.y, min.z);
		corners[7] = glm::vec3(max.x, max.y, max.z);
		return corners;
	};

	glm::vec3 GetCenter() {
		return (min + max) / 2.0f;
	}

	glm::vec3 GetSize() {
		return max - min;
	}
};