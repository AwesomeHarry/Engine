#include "TextureCubemap.h"
#include <glad/glad.h>
#include <stb_image.h>
#include "Logging/Logging.h"

using namespace Engine;

TextureCubemap::TextureCubemap(const TextureSpec& spec)
	: BaseTexture(TextureType::TexCubemap, spec) {
    BindInternal();

	for (uint32_t i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, _internalFormat, _width, _height, 0, _dataFormat, _dataType, nullptr);

    glTexParameteri(_internalType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(_internalType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(_internalType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(_internalType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(_internalType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void TextureCubemap::SetData(CubemapIndex index, void* data) {
	BindInternal();
	SetDataInternal(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (int)index, data);
}

std::shared_ptr<TextureCubemap> TextureCubemap::Utils::FromFile(const CubemapPaths& paths) {
    TextureSpec spec;
    std::vector<Texture::Utils::FileTextureData> cubemapData;
    stbi_set_flip_vertically_on_load(false);

    // Load all images and perform consistency checks
    for (const auto& path : { paths.positiveX, paths.negativeX, paths.positiveY, paths.negativeY, paths.positiveZ, paths.negativeZ }) {
        auto fileData = Texture::Utils::LoadFromFile(path);

        if (cubemapData.empty()) {
            // Set initial spec from the first image
            spec.width = fileData.width;
            spec.height = fileData.height;
            spec.format = fileData.format;
        }
        else {
            // Check consistency with the first image
            if (fileData.width != spec.width || fileData.height != spec.height || fileData.format != spec.format) {
                ENGINE_ERROR("All cubemap images must have the same dimensions and format");
            }
        }

        cubemapData.push_back(std::move(fileData));
    }

    // Create texture and set data
    auto texture = std::make_shared<TextureCubemap>(spec);
    texture->BindInternal();

    for (int i = 0; i < 6; ++i) {
        texture->SetData((CubemapIndex)i, cubemapData[i].data);
    }

    // Free the image data
    for (auto& data : cubemapData) {
        stbi_image_free(data.data);
    }

    return texture;
}

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Rendering/Platform/Buffer/VertexArrayObject.h"
#include "Rendering/Platform/Material.h"
#include "Rendering/Platform/Mesh.h"
#include "Rendering/Platform/Texture2D.h"
#include "Rendering/Platform/Framebuffer.h"
#include "Rendering/RenderCommands.h"

#pragma region Shaders

const char* equiToCubeVertex = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    localPos = aPos;  
    gl_Position =  projection * view * vec4(localPos, 1.0);
}
)";

const char* equiToCubeFrag = R"(
#version 330 core
out vec4 FragColor;
in vec3 localPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(localPos)); // make sure to normalize localPos
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    FragColor = vec4(color, 1.0);
}
)";

#pragma endregion

std::shared_ptr<TextureCubemap> TextureCubemap::Utils::FromTexture2D(std::shared_ptr<Texture2D> texture, Texture2DCubemapFormat format) {
	std::shared_ptr<Engine::TextureCubemap> cubemap;

    switch (format) {
    case Texture2DCubemapFormat::Equirectangle:
    {
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] = {
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

	#pragma region Skybox Data
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
	#pragma endregion
		auto cubeVa = std::make_shared<Engine::VertexArrayObject>();
		auto skyboxVbo = std::make_shared<Engine::VertexBufferObject>(Engine::BufferUsage::Static);
		skyboxVbo->SetData(skyboxVertices, 36, {
			{ "aPos", Engine::LType::Float, 3 }
						   });
		cubeVa->AddVertexBuffer(skyboxVbo);
		cubeVa->Compute();
		auto cubeMesh = std::make_shared<Engine::Mesh>();
		cubeMesh->AddSubmesh(cubeVa);

		auto shader = std::make_shared<Engine::Shader>();
		shader->AttachShader(ShaderStage::Vertex, equiToCubeVertex);
		shader->AttachShader(ShaderStage::Fragment, equiToCubeFrag);
		shader->Link();

		auto equiToCubemapMaterial = std::make_shared<Engine::Material>(shader);
		equiToCubemapMaterial->SetTexture(texture, "equirectangularMap");
		equiToCubemapMaterial->SetUniform("projection", captureProjection);
		auto equiToCubemapMaterialInstance = equiToCubemapMaterial->CreateInstance();

		uint32_t captureSize = texture->GetHeight() / 2;

		auto textureSpec = Engine::TextureSpec{
			captureSize,
			captureSize,
			texture->GetFormat()
		};
		cubemap = std::make_shared<Engine::TextureCubemap>(textureSpec);

		auto framebufferSpec = Engine::Framebuffer::FramebufferSpec{
			captureSize,
			captureSize,
			{ texture->GetFormat() },
			true
		};
		auto captureFrambuffer = std::make_shared<Engine::Framebuffer>(framebufferSpec);

		captureFrambuffer->Bind();
		for (uint32_t i = 0; i < 6; i++) {
			//captureFrambuffer->ModifyColorAttachment(0, (Engine::TextureTarget)((int)Engine::TextureTarget::CubemapPosX + i), *cubemap);
			equiToCubemapMaterial->SetUniform("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap->GetInstanceID(), 0);

			Engine::RenderCommands::ClearBuffers(Engine::BufferBit::Color | Engine::BufferBit::Depth);
			Engine::RenderCommands::RenderMesh(*cubeMesh, *equiToCubemapMaterialInstance);
		}
		captureFrambuffer->Unbind();
    }
	break;
    default:
        ENGINE_ERROR("Invalid format to convert texture2D to cubemap!");
        return nullptr;
    }

	return cubemap;
}
