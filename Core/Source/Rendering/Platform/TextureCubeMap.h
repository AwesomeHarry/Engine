#pragma once
#include "BaseTexture.h"
#include <vector>

#include <memory>
#include <string>

namespace Engine {
	class Texture2D;

	enum class CubeMapIndex {
		PositiveX = 0, NegativeX = 1,
		PositiveY = 2, NegativeY = 3,
		PositiveZ = 4, NegativeZ = 5,

		First = PositiveX
	};

	class TextureCubeMap : public BaseTexture {
	public:
		TextureCubeMap(const TextureSpec& spec);
		~TextureCubeMap() = default;

		void SetData(CubeMapIndex index, void* data);

		struct Utils {
			struct CubeMapPaths {
				std::string positiveX;
				std::string negativeX;
				std::string positiveY;
				std::string negativeY;
				std::string positiveZ;
				std::string negativeZ;
			};

			static std::shared_ptr<TextureCubeMap> FromFile(const CubeMapPaths& paths);

			enum class Texture2DCubemapFormat {
				Equirectangle
			};

			static std::shared_ptr<TextureCubeMap> FromTexture2D(std::shared_ptr<Texture2D> texture, Texture2DCubemapFormat format);
		};
	};
}
