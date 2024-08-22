#include "Scene/Scene.h"

namespace Engine {
	class SceneSerializer {
	public:
		static void Serialize(const Scene& scene, const std::string& path);
	};
}