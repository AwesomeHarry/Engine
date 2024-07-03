#pragma once

#include <entt/entt.hpp>

namespace Engine {
	class Entity;

	class Scene {
	public:
		Scene() = default;
		~Scene() = default;

		Entity CreateEntity(const std::string& name = "");
		void DestroyEntity(Entity entity);

		Entity GetEntity(const std::string& name);

		template <typename... Components>
		auto GetAllEntitiesWith() const {
			return _registry.view<Components...>();
		}

		void UpdateScene(float ts);

		const entt::registry& GetRegistry() const { return _registry; }
	private:
		entt::registry _registry;

		friend class Entity;
	};
}