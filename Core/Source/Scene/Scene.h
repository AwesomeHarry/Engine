#pragma once
#include <entt/entt.hpp>
#include "Scene/Components/Native/DebugShapeManager.h"

namespace Engine {
	class Entity;

	class Scene {
	public:
		Scene();
		~Scene() = default;

		Entity CreateEntity(const std::string& name = "");
		void DestroyEntity(Entity entity);

		Entity GetEntity(entt::entity handle);
		Entity GetEntity(const std::string& name);

		template <typename... Components>
		auto GetAllEntitiesWith() const {
			return _registry.view<Components...>();
		}

		void UpdateScene(float ts);
		void RenderScene();

		const entt::registry& GetRegistry() const { return _registry; }
		entt::registry& GetRegistry() { return _registry; }

		DebugShapeManager& GetDebugRenderer() { return _registry.get<DebugShapeManager>(_debugShapeManager); }
	private:
		entt::registry _registry;
		entt::entity _debugShapeManager;

		friend class Entity;
	};
}