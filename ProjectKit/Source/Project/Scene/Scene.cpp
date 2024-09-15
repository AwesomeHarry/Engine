#include "Scene.h"

#include "Project/Scene/Components/Native/Components.h"
#include "Entity.h"

using namespace Engine;

Scene::Scene() {
	/* Add Debug Shape Manager */
	{
		Entity e = CreateEntity("Debug Shape Renderer");
		e.AddComponent<DebugShapeManager>();
		_debugShapeManager = e.GetInstanceID();
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
	_registry.destroy(entity.GetInstanceID());
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

void Scene::RenderScene() {
	auto cameraGroup = _registry.group<Engine::TransformComponent, Engine::CameraComponent>();

	// Cameras are sorted from least priority to highest priority
	cameraGroup.sort<Engine::CameraComponent>([](const Engine::CameraComponent& a, const Engine::CameraComponent& b) {
		return a.priority < b.priority;
	});

	for (auto entity : cameraGroup) {
		auto& transform = cameraGroup.get<Engine::TransformComponent>(entity);
		auto& camera = cameraGroup.get<Engine::CameraComponent>(entity);

		Entity cameraEntity = { entity,this };

		if (!camera.renderPipeline)
			ENGINE_ERROR("[Scene::RenderScene] No IRenderPipeline set for Camera: ", cameraEntity.GetName());

		camera.renderPipeline->RenderScene(*this, cameraEntity);
	}
}
