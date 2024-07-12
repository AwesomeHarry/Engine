#pragma once
#include "BaseTexture.h"
#include <vector>

#include <memory>
#include <string>

namespace Engine {
	class TextureCubeMap : public BaseTexture {
	public:
		TextureCubeMap(const TextureSpec& spec)
			: BaseTexture(TextureType::TexCubeMap, spec) {}
		~TextureCubeMap() = default;

		void SetData(const std::vector<uint8_t*>& data);

		struct Utils {
			static std::shared_ptr<TextureCubeMap> FromFile(const std::vector<std::string>& paths);
		};
	};
}
