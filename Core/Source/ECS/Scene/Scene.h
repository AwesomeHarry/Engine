#pragma once

#include <entt/entt.hpp>

namespace Engine {
	class Entity;

	class Scene {
	public:
		Scene() {}
		~Scene() {}

		Entity CreateEntity(const std::string& name = "");
		void DestroyEntity(Entity entity);

		Entity GetEntity(const std::string& name);

		void UpdateScene(float ts);
		void RenderScene();
	private:
		entt::registry _registry;

		friend class Entity;
	};
}