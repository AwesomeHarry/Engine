#pragma once
#include <memory>

#include "BaseComponent.h"
#include "Project/Assets/MaterialAsset.h"

namespace Engine {
	class Entity;

	struct MeshRendererComponent : public BaseComponent, public IRenderableMaterial {
		std::shared_ptr<MaterialAsset> materialAsset;

		MeshRendererComponent() {}
		MeshRendererComponent(const std::shared_ptr<MaterialAsset>& materialAsset)
			: materialAsset(materialAsset) {}

		// Check to make sure entity also has mesh filter component
		virtual void OnComponentAdded(Entity& entity) override;

		// Material Overrides
		void SetUniform(const std::string& name, const UniformValue& value);
		std::unordered_map<std::string, UniformValue>& GetUniformOverrides() { return _uniformOverrides; }

		// IRenderableMaterial
		void Bind() const override;
		void Unbind() const override;

	private:
		std::unordered_map<std::string, UniformValue> _uniformOverrides;
	};
}
