#pragma once
#include "BaseTexture.h"
#include <string>
#include <memory>

namespace Engine {
	class Texture2D : public BaseTexture {
	public:
		Texture2D(const TextureSpec& spec)
			: BaseTexture(TextureType::Tex2D, spec) {}
		~Texture2D() = default;

		void SetData(const void* data);

		struct Utils {
			static std::shared_ptr<Texture2D> FromFile(const std::string& path);
		};
	};
}