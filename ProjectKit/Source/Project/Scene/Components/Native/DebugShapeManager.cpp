#include "DebugShapeManager.h"

#include "Rendering/Platform/Mesh.h"
#include "Rendering/Platform/Shader.h"

#include "Rendering/Platform/Buffer/VertexArrayObject.h"
#include "Rendering/Platform/Buffer/VertexBufferObject.h"

namespace Engine {
#pragma region Shaders

#pragma region Point Shader
    const char* pointShader_V = R"glsl(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in float aRadius;
    layout(location = 2) in vec4 aColor;
    out vec3 vPos;
    out float vRadius;
    out vec4 vColor;
    void main() {
        vPos = aPos;
        vRadius = aRadius;
        vColor = aColor;
        gl_Position = vec4(aPos, 1.0);
    }
    )glsl";


    const char* pointShader_G = R"glsl(
    #version 330 core
    layout (points) in;
    layout (triangle_strip, max_vertices = 4) out;
    layout (std140) uniform CameraData {
        mat4 projection;
        mat4 view;
    };
    uniform vec2 viewportSize;

    in vec3 vPos[];
    in float vRadius[];
    in vec4 vColor[];
    out vec2 gTexCoords;
    out vec4 gColor;
    out vec3 gViewPos;
    out float gRadius;

    void main() {
        vec4 centerViewSpace = view * vec4(vPos[0], 1.0);
        float radius = vRadius[0];
    
        // Calculate the size in screen space (pixels)
        float pixelSize = radius * 2.0; // Diameter in pixels
    
        // Calculate the size in NDC space, accounting for aspect ratio
        vec2 sizeNDC = (pixelSize / viewportSize) * 2.0;
    
        // Emit a quad
        vec2 offsets[4] = vec2[](
            vec2(-1.0, -1.0),
            vec2( 1.0, -1.0),
            vec2(-1.0,  1.0),
            vec2( 1.0,  1.0)
        );
    
        for (int i = 0; i < 4; i++) {
            // Calculate vertex position in clip space
            vec4 posClip = projection * centerViewSpace;
        
            // Convert to NDC
            vec3 posNDC = posClip.xyz / posClip.w;
        
            // Add offset in NDC space, accounting for aspect ratio
            posNDC.xy += offsets[i] * sizeNDC * 0.5;
        
            // Convert back to clip space
            gl_Position = vec4(posNDC * posClip.w, posClip.w);
        
            gTexCoords = offsets[i] * 0.5 + 0.5;
            gColor = vColor[0];
            gViewPos = centerViewSpace.xyz;
            gRadius = radius;
            EmitVertex();
        }
        EndPrimitive();
    }
    )glsl";


    const char* pointShader_F = R"glsl(
    #version 330 core
    in vec2 gTexCoords;
    in vec4 gColor;
    in vec3 gViewPos;
    in float gRadius;
    out vec4 FragColor;

    void main() {
        // Calculate the distance from the center
        vec2 centeredTexCoords = gTexCoords * 2.0 - 1.0;
        float dist = length(centeredTexCoords);
    
        // SDF for a circle
        float sdf = dist - 1.0;
    
        // Apply anti-aliasing
        float alpha = 1.0 - smoothstep(-0.01, 0.01, sdf);
    
        // Discard fragments outside the circle
        if (alpha < 0.01) discard;
    
        FragColor = vec4(gColor.rgb, gColor.a * alpha);
    }
    )glsl";
#pragma endregion

#pragma region Line Shader
	const char* lineShader_V = R"glsl(
	#version 330 core
    layout (location = 0) in vec3 aStartPos;
	layout (location = 1) in vec3 aEndPos;
    layout (location = 2) in vec4 aColor;
	
    out vec3 vStartPos;
	out vec3 vEndPos;
	out vec4 vColor;

	void main() {
		vStartPos = aStartPos;
		vEndPos = aEndPos;
		vColor = aColor;
		gl_Position = vec4(aStartPos,1.0);
	}
	)glsl";
	const char* lineShader_G = R"glsl(
	#version 330 core
    layout (points) in;
    layout (triangle_strip, max_vertices = 4) out;

    layout (std140) uniform CameraData {
        mat4 projection;
        mat4 view;
    };

    in vec3 vStartPos[];
	in vec3 vEndPos[];
	in vec4 vColor[];

    out vec4 color;

    uniform float thickness;

    void main() {
        // Transform p1 and p2 to clip space
        vec4 p1Clip = projection * view * vec4(vStartPos[0], 1.0);
        vec4 p2Clip = projection * view * vec4(vEndPos[0], 1.0);
    
        // Calculate direction and normal in clip space
        vec2 dir = normalize(p2Clip.xy / p2Clip.w - p1Clip.xy / p1Clip.w);
        vec2 normal = vec2(-dir.y, dir.x) * thickness / 2.0;
    
        // Emit vertices
        gl_Position = p1Clip + vec4(normal * p1Clip.w, 0.0, 0.0);
        color = vColor[0];
        EmitVertex();
    
        gl_Position = p1Clip - vec4(normal * p1Clip.w, 0.0, 0.0);
        color = vColor[0];
        EmitVertex();
    
        gl_Position = p2Clip + vec4(normal * p2Clip.w, 0.0, 0.0);
        color = vColor[0];
        EmitVertex();
    
        gl_Position = p2Clip - vec4(normal * p2Clip.w, 0.0, 0.0);
        color = vColor[0];
        EmitVertex();
    
        EndPrimitive();
    }
    )glsl";

	const char* lineShader_F = R"glsl(
    #version 330 core
    out vec4 FragColor;
    in vec4 color;
    void main() {
        FragColor = color;
    }
    )glsl";

#pragma endregion

#pragma region Quad Shader

    const char* quadShader_V = R"glsl(
	#version 330 core
    layout (location = 0) in vec3 aP1;
    layout (location = 1) in vec3 aP2;
    layout (location = 2) in vec3 aP3;
    layout (location = 3) in vec3 aP4;
    layout (location = 4) in vec4 aColor;
    
    out vec3 vP1;
    out vec3 vP2;
    out vec3 vP3;
    out vec3 vP4;
	out vec4 vColor;

    void main() {
		vP1 = aP1;
		vP2 = aP2;
		vP3 = aP3;
		vP4 = aP4;
		vColor = aColor;
		gl_Position = vec4(aP1, 1.0);
	}
    )glsl";

    const char* quadShader_G = R"glsl(
	#version 330 core
	layout (points) in;
	layout (triangle_strip, max_vertices = 4) out;

	layout (std140) uniform CameraData {
		mat4 projection;
		mat4 view;
	};

	in vec3 vP1[];
	in vec3 vP2[];
	in vec3 vP3[];
	in vec3 vP4[];
	in vec4 vColor[];

	out vec4 color;

	void main() {
		vec4 p1 = view * vec4(vP1[0], 1.0);
		vec4 p2 = view * vec4(vP2[0], 1.0);
		vec4 p3 = view * vec4(vP3[0], 1.0);
		vec4 p4 = view * vec4(vP4[0], 1.0);

		gl_Position = projection * p1;
		color = vColor[0];
		EmitVertex();

		gl_Position = projection * p2;
		color = vColor[0];
		EmitVertex();

		gl_Position = projection * p3;
		color = vColor[0];
		EmitVertex();

		gl_Position = projection * p4;
		color = vColor[0];
		EmitVertex();

		EndPrimitive();
	}
	)glsl";

    const char* quadShader_F = R"glsl(
    #version 330 core
	out vec4 FragColor;
	in vec4 color;
	void main() {
		FragColor = color;
	}
    )glsl";
#pragma endregion

#pragma endregion

	void DebugShapeManager::DrawPoint(const PointSpec& spec) {
		pointData.push_back(spec);
	}

	void DebugShapeManager::DrawLine(const LineSpec& spec) {
		lineData.push_back(spec);
	}

    void DebugShapeManager::DrawQuad(const QuadSpec& spec) {
        quadData.push_back(spec);
    }

    void DebugShapeManager::DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, bool wireframe) {
        glm::vec3 halfSize = size / 2.0f;
    	glm::vec3 vertices[8] = {
			{ -halfSize.x, -halfSize.y, -halfSize.z },
			{ -halfSize.x, -halfSize.y,  halfSize.z },
			{ -halfSize.x,  halfSize.y, -halfSize.z },
			{ -halfSize.x,  halfSize.y,  halfSize.z },
			{  halfSize.x, -halfSize.y,  halfSize.z },
			{  halfSize.x, -halfSize.y, -halfSize.z },
			{  halfSize.x,  halfSize.y,  halfSize.z },
			{  halfSize.x,  halfSize.y, -halfSize.z }
		};

        if (wireframe) {
            DrawLine({ position + vertices[0],position + vertices[1],color });
            DrawLine({ position + vertices[1],position + vertices[3],color });
            DrawLine({ position + vertices[3],position + vertices[2],color });
            DrawLine({ position + vertices[2],position + vertices[0],color });

            DrawLine({ position + vertices[4],position + vertices[5],color });
            DrawLine({ position + vertices[5],position + vertices[7],color });
            DrawLine({ position + vertices[7],position + vertices[6],color });
            DrawLine({ position + vertices[6],position + vertices[4],color });

            DrawLine({ position + vertices[0],position + vertices[5],color });
            DrawLine({ position + vertices[1],position + vertices[4],color });
            DrawLine({ position + vertices[2],position + vertices[7],color });
            DrawLine({ position + vertices[3],position + vertices[6],color });
        }
        else {
            DrawQuad({ position + vertices[0],position + vertices[1],position + vertices[2],position + vertices[3],color });
            DrawQuad({ position + vertices[4],position + vertices[5],position + vertices[6],position + vertices[7],color });
            DrawQuad({ position + vertices[1],position + vertices[0],position + vertices[4],position + vertices[5],color });
            DrawQuad({ position + vertices[2],position + vertices[3],position + vertices[7],position + vertices[6],color });
            DrawQuad({ position + vertices[3],position + vertices[1],position + vertices[6],position + vertices[4],color });
            DrawQuad({ position + vertices[0],position + vertices[2],position + vertices[5],position + vertices[7],color });
        }
    }

    void DebugShapeManager::Clear() {
		pointData.clear();
		lineData.clear();
        quadData.clear();
    }

	void DebugShapeManager::OnComponentAdded(Entity& entity) {
		initialize();
	}

	void DebugShapeManager::initialize() {
        /* Points */
        {
            pointShader = std::make_shared<Shader>();
            pointShader->AttachShader(ShaderStage::Vertex, pointShader_V);
            pointShader->AttachShader(ShaderStage::Geometry, pointShader_G);
            pointShader->AttachShader(ShaderStage::Fragment, pointShader_F);
            pointShader->Link();
            pointShader->BindUniformBlock("CameraData", 0);

            pointInfoVbo = std::make_shared<VertexBufferObject>(BufferUsage::Dynamic);
            pointInfoVbo->SetData(nullptr, 0, {
                        { "aPos", Engine::LType::Float, 3 },
                        { "aRadius", Engine::LType::Float, 1 },
                        { "aColor", Engine::LType::Float, 4 }
                                  });

            pointInfoVao = std::make_shared<VertexArrayObject>();
            pointInfoVao->AddVertexBuffer(pointInfoVbo);
            pointInfoVao->Compute();
        }

        /* Lines */
        {
            lineShader = std::make_shared<Shader>();
            lineShader->AttachShader(ShaderStage::Vertex, lineShader_V);
            lineShader->AttachShader(ShaderStage::Geometry, lineShader_G);
            lineShader->AttachShader(ShaderStage::Fragment, lineShader_F);
            lineShader->Link();
            lineShader->BindUniformBlock("CameraData", 0);

            lineInfoVbo = std::make_shared<VertexBufferObject>(BufferUsage::Dynamic);
            lineInfoVbo->SetData(nullptr, 0, {
                        { "aStartPos", Engine::LType::Float, 3 },
                        { "aEndPos", Engine::LType::Float, 3 },
                        { "aColor", Engine::LType::Float, 4 }
                                 });

            lineInfoVao = std::make_shared<VertexArrayObject>();
            lineInfoVao->AddVertexBuffer(lineInfoVbo);
            lineInfoVao->Compute();
        }

        /* Cubes */
        {
            quadShader = std::make_shared<Shader>();
			quadShader->AttachShader(ShaderStage::Vertex, quadShader_V);
			quadShader->AttachShader(ShaderStage::Geometry, quadShader_G);
			quadShader->AttachShader(ShaderStage::Fragment, quadShader_F);
            quadShader->Link();
            quadShader->BindUniformBlock("CameraData", 0);

			quadInfoVbo = std::make_shared<VertexBufferObject>(BufferUsage::Dynamic);
            quadInfoVbo->SetData(nullptr, 0, {
						{ "aP1", Engine::LType::Float, 3 },
						{ "aP2", Engine::LType::Float, 3 },
						{ "aP3", Engine::LType::Float, 3 },
						{ "aP4", Engine::LType::Float, 3 },
						{ "aColor", Engine::LType::Float, 4 }
								  });

			quadInfoVao = std::make_shared<VertexArrayObject>();
			quadInfoVao->AddVertexBuffer(quadInfoVbo);
			quadInfoVao->Compute();
        }
	}
}
