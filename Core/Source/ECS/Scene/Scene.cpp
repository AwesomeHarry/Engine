#include "Scene.h"

#include "ECS/Components/Components.h"
#include "Entity.h"

using namespace Engine;

Scene::Scene() {
	/* Add Debug Shape Manager */
	{
		Entity e = CreateEntity("Debug Shape Renderer");
		e.AddComponent<DebugShapeManager>();
		_debugShapeManager = e.GetID();
	}
}

Entity Scene::CreateEntity(const std::string& name) {
	Entity entity(_registry.create(), this);

	// Add Name
	auto& nc = entity.AddComponent<NameComponent>(name);
	nc.name = name.empty() ? "Unamed Entity" : name;

	// Add Transform
	entity.AddComponent<TransformComponent>();

	return entity;
}

void Scene::DestroyEntity(Entity entity) {
	_registry.destroy(entity.GetID());
}

Entity Engine::Scene::GetEntity(entt::entity handle) {
	return Entity(handle, this);
}

Entity Scene::GetEntity(const std::string& name) {
	auto view = _registry.view<NameComponent>();
	for (auto entityUid : view) {
		const NameComponent& nc = view.get<NameComponent>(entityUid);	
		if (nc.name == name)
			return Entity(entityUid, this);
	}
	return {};
}

void Scene::UpdateScene(float ts) {

}