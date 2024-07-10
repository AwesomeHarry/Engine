#pragma once
#include "BaseComponent.h"
#include "Util/Math/Transform.h"
#include <vector>

namespace Engine {
	struct TransformComponent : public BaseComponent, Transform {
		TransformComponent() = default;
		TransformComponent(const glm::vec3& position)
			: Transform{ position } {}

		void SetChild(TransformComponent* child) {
			child->_parent = this;
			_children.push_back(child);
		}

		TransformComponent& GetChild(int index) { return *_children[index]; }
		const TransformComponent& GetChild(int index) const { return *_children[index]; }

		TransformComponent& GetParent() { return *_parent; }
		const TransformComponent& GetParent() const { return *_parent; }

		glm::mat4 GetWorldMatrix() const {
			if (_parent)
				return _parent->GetWorldMatrix() * GetTransformMatrix();

			return GetTransformMatrix();
		}

	private:
		std::vector<TransformComponent*> _children = {};
		TransformComponent* _parent = nullptr;
	};
}