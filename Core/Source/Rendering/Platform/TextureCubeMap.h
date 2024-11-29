#pragma once
#include "BaseTexture.h"
#include <vector>

#include <memory>
#include <string>

namespace Engine {
	class Texture2D;

	enum class CubemapIndex {
		PositiveX = 0, NegativeX = 1,
		PositiveY = 2, NegativeY = 3,
		PositiveZ = 4, NegativeZ = 5,

		First = PositiveX
	};

	class TextureCubemap : public BaseTexture {
	public:
		TextureCubemap(const TextureSpec& spec);
		~TextureCubemap() = default;

		void SetData(CubemapIndex index, void* data);

		struct Utils {
			struct CubemapPaths {
				std::string positiveX;
				std::string negativeX;
				std::string positiveY;
				std::string negativeY;
				std::string positiveZ;
				std::string negativeZ;
			};

			static std::shared_ptr<TextureCubemap> FromFile(const CubemapPaths& paths);

			enum class Texture2DCubemapFormat {
				Equirectangle
			};

			static std::shared_ptr<TextureCubemap> FromTexture2D(std::shared_ptr<Texture2D> texture, Texture2DCubemapFormat format);
		};
	};
}
