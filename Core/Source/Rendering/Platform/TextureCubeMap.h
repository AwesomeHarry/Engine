#pragma once
#include "BaseTexture.h"
#include <vector>

#include <memory>
#include <string>

namespace Engine {
	struct CubeMapData {
		void* positiveX;
		void* negativeX;
		void* positiveY;
		void* negativeY;
		void* positiveZ;
		void* negativeZ;

		std::vector<void*> GetArray() const {
			return { positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ };
		}
	};

	struct CubeMapPaths {
		std::string positiveX;
		std::string negativeX;
		std::string positiveY;
		std::string negativeY;
		std::string positiveZ;
		std::string negativeZ;

		std::vector<std::string> GetArray() const {
			return { positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ };
		}
	};

	class TextureCubeMap : public BaseTexture {
	public:
		TextureCubeMap(const TextureSpec& spec)
			: BaseTexture(TextureType::TexCubeMap, spec) {}
		~TextureCubeMap() = default;

		void SetData(const std::vector<void*>& data);
		void SetData(const CubeMapData& data);

		struct Utils {
			static std::shared_ptr<TextureCubeMap> FromFile(const std::vector<std::string>& paths);
		};
	};
}
