#include <json.hpp>
#include "Rendering/Platform/Framebuffer.h"

using namespace Engine;

namespace nlohmann {
	template<>
	struct adl_serializer<Framebuffer::FramebufferSpec> {
		static void to_json(json& j, const Framebuffer::FramebufferSpec& fbSpec) {
			j = json{
				{"width",fbSpec.width},
				{"height",fbSpec.height},
				{"attachments",json::array()},
				{"includeDepthStencil",fbSpec.includeDepthStencil}
			};
			for (const auto& attachment : fbSpec.attachments) {
				j["attachments"].push_back(attachment);
			}
		}

		static void from_json(const json& j, Framebuffer::FramebufferSpec& fbSpec) {
			j.at("width").get_to(fbSpec.width);
			j.at("height").get_to(fbSpec.height);
			j.at("includeDepthStencil").get_to(fbSpec.includeDepthStencil);
			for (const auto& attachment : j.at("attachments")) {
				fbSpec.attachments.push_back(attachment.get<ImageFormat>());
			}
		}
	};
}