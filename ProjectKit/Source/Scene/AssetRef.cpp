#include "AssetRef.h"

namespace Engine {
    void to_json(nlohmann::json& j, const AssetRef& assetRef) {
        j = assetRef.GetId();
    }

    void from_json(const nlohmann::json& j, AssetRef& assetRef) {
        assetRef = AssetRef(j.get<uint32_t>());
    }
}