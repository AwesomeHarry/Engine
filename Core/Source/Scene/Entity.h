#pragma once

#include <entt/entt.hpp>
#include "Logging/Logging.h"

#include "Scene/Scene.h"

#include "Scene/Components/Native/NameComponent.h"
#include "Scene/Components/Native/TransformComponent.h"

namespace Engine {
	class Entity {
	public:
		Entity()
			: _id(entt::null), _scene(nullptr) {}
		Entity(entt::entity handle, Scene* scene)
			: _id(handle), _scene(scene) {}

		template<typename ComponentType, typename ...Args>
		ComponentType& AddComponent(Args&&... args) {
			if (HasComponent<ComponentType>()) {
				ENGINE_ERROR("Failed to add component of type {} to {}", typeid(ComponentType).name(), GetName());
				return GetComponent<ComponentType>();
			}

			ComponentType& component = _scene->_registry.emplace<ComponentType>(_id, std::forward<Args>(args)...);
			component.OnComponentAdded(*this);
			return component;
		}

		template<typename ComponentType>
		bool HasComponent() {
			return _scene->_registry.all_of<ComponentType>(_id);
		}

		template<typename ComponentType>
		ComponentType& GetComponent() {
			ENGINE_ASSERT(HasComponent<ComponentType>(), "Entity does not have component.");
			return _scene->_registry.get<ComponentType>(_id);
		}

		template<typename ComponentType>
		void RemoveComponent() {
			ENGINE_ASSERT(HasComponent<ComponentType>(), "Entity does not have component.");
			_scene->_registry.remove<ComponentType>(_id);
		}

		inline entt::entity GetID() const { return _id; }
		inline const std::string& GetName() { return GetComponent<NameComponent>().name; }
		inline TransformComponent& GetTransform() { return GetComponent<TransformComponent>(); }

		operator bool() const { return _id != entt::null; }
		bool operator== (const Entity& other) const { return _id == other._id; }
		bool operator!= (const Entity& other) const { return !(*this == other); }
	private:
		entt::entity _id;
		Scene* _scene;
	};
}