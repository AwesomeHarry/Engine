#include "MaterialAsset.h"

using namespace Engine;

MaterialAssetInstance::MaterialAssetInstance(AssetBank& assetBank, AssetRef shaderRef)
	: Material(shaderRef.Resolve<ShaderAsset>(assetBank)->GetInstance()), _shaderRef(shaderRef), _assetBank(assetBank) {
	for (const auto& [name, _] : _textureMap)
		_textureRefs[name] = AssetRef(0);
}

void MaterialAssetInstance::SetShaderRef(const AssetRef& shaderRef) {
	_shaderRef = shaderRef;
	Material::SetShader(_shaderRef.Resolve<ShaderAsset>(_assetBank)->GetInstance());
}

void MaterialAssetInstance::SetTextureRef(const std::string& name, const AssetRef& textureRef) {
	auto it = _textureMap.find(name);
	if (it == _textureMap.end()) {
		ENGINE_ERROR("[MaterialAssetInstance::SetTextureRef] Texture '{0}' does not exist in the material.", name);
		return;
	}

	_textureRefs[name] = textureRef;

	if (textureRef.GetId() == 0) return;

	auto textureAsset = _assetBank.GetAsset<IAsset>(textureRef.GetId());
	auto textureType = textureAsset->GetType();
	if (textureType == AssetType::Texture2D) {
		auto texture = textureRef.Resolve<Texture2DAsset>(_assetBank)->GetInstance();
		SetTexture(name, texture);
	}
	else if (textureType == AssetType::TextureCubemap) {
		auto texture = textureRef.Resolve<TextureCubemapAsset>(_assetBank)->GetInstance();
		SetTexture(name, texture);
	}
}

const AssetRef& MaterialAssetInstance::GetTextureRef(const std::string& name) const {
	if (!HasTexture(name)) {
		ENGINE_ERROR("[MaterialAssetInstance::GetTextureRef] Texture '{0}' does not exist in the material.", name);
		return AssetRef(0);
	}
	return _textureRefs.at(name);
}

std::shared_ptr<MaterialOverride> MaterialAssetInstance::CreateInstance() {
	return std::make_shared<MaterialAssetInstanceOverride>(_assetBank, this);
}

MaterialAssetInstanceOverride::MaterialAssetInstanceOverride(AssetBank& assetBank, MaterialAssetInstance* materialAssetInstance)
	: MaterialOverride(materialAssetInstance), _baseMaterialAssetInstance(materialAssetInstance), _assetBank(assetBank) {}

void MaterialAssetInstanceOverride::SetShaderRef(const AssetRef& shaderRef) {
	_shaderRef = shaderRef;
	SetShader(shaderRef.Resolve<ShaderAsset>(_assetBank)->GetInstance());
}

void MaterialAssetInstanceOverride::SetTextureRef(const std::string& name, const AssetRef& textureRef) {
	_overridenTextureRefs[name] = textureRef;

	if (textureRef.GetId() == 0) {
		ENGINE_ERROR("[MaterialAssetInstanceOverride::SetTextureRef] Texture reference is invalid.");
		return;
	}

	auto textureAsset = _assetBank.GetAsset<IAsset>(textureRef.GetId());
	auto textureType = textureAsset->GetType();
	if (textureType == AssetType::Texture2D) {
		auto texture = textureRef.Resolve<Texture2DAsset>(_assetBank)->GetInstance();
		SetTexture(name, texture);
	}
	else if (textureType == AssetType::TextureCubemap) {
		auto texture = textureRef.Resolve<TextureCubemapAsset>(_assetBank)->GetInstance();
		SetTexture(name, texture);
	}
}

const AssetRef& MaterialAssetInstanceOverride::GetTextureRef(const std::string& name) const {
	if (!IsTextureOverridden(name)) {
		ENGINE_ERROR("[MaterialAssetInstanceOverride::GetTextureRef] Texture '{0}' is not overriden or does not exist.", name);
		return AssetRef(0);
	}
	return _overridenTextureRefs.at(name);
}
