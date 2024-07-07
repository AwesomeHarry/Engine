#include "DebugShapeManager.h"

#include "Rendering/Platform/Mesh.h"
#include "Rendering/Platform/Shader.h"

#include "Rendering/Platform/Buffer/VertexArrayObject.h"
#include "Rendering/Platform/Buffer/VertexBufferObject.h"

namespace Engine {
#pragma region Shaders

#pragma region PointShader
	const char* pointShader_V = R"glsl(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec4 aColor;
    out vec3 vPos;
	out vec4 vColor;
    void main() {
        vPos = aPos;
		vColor = aColor;
        gl_Position = vec4(aPos, 1.0);
    }
    )glsl";

	const char* pointShader_G = R"glsl(
    #version 330 core

    layout (points) in;
    layout (triangle_strip, max_vertices = 66) out;

    layout (std140) uniform CameraData {
        mat4 projection;
        mat4 view;
    };

	in vec3 vPos[];
	in vec4 vColor[];

    uniform float radius = 0.1;
    uniform int segments = 20;

	out vec4 color;

    const float PI = 3.1415926535897932384626433832795;

    void main() {
        // Transform the center point to view space
        vec4 centerViewSpace = view * vec4(vPos[0], 1.0);
    
        for (int i = 0; i <= segments; i++) {
            float angle = 2.0 * PI * float(i) / float(segments);
            float nextAngle = 2.0 * PI * float(i + 1) / float(segments);
        
            // Emit center vertex
            gl_Position = projection * centerViewSpace;
            color = vColor[0];
            EmitVertex();
        
            // Emit vertex on the circle's edge
            gl_Position = projection * (centerViewSpace + vec4(radius * cos(angle), radius * sin(angle), 0.0, 0.0));
            color = vColor[0];
            EmitVertex();
        
            // Emit next vertex on the circle's edge
            gl_Position = projection * (centerViewSpace + vec4(radius * cos(nextAngle), radius * sin(nextAngle), 0.0, 0.0));
            color = vColor[0];
            EmitVertex();
        
            EndPrimitive();
        }
    }
    )glsl";

	const char* pointShader_F = R"glsl(
    #version 330 core
    out vec4 FragColor;
    in vec4 color;
    void main() {
        FragColor = color;
    }
    )glsl";
#pragma endregion

#pragma region LineShader
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

#pragma endregion

	void DebugShapeManager::DrawPoint(const PointSpec& spec) {
		pointData.push_back(spec);
	}

	void DebugShapeManager::DrawLine(const LineSpec& spec) {
		lineData.push_back(spec);
	}

    void DebugShapeManager::Clear() {
		pointData.clear();
		lineData.clear();
    }

	void DebugShapeManager::OnComponentAdded(Entity& entity) {
		initialize();
	}

	void DebugShapeManager::initialize() {
        /* Points */
        {
            pointShader = std::make_shared<Shader>();
            pointShader->AttachVertexShader(pointShader_V);
            pointShader->AttachGeometeryShader(pointShader_G);
            pointShader->AttachFragmentShader(pointShader_F);
            pointShader->Link();

            pointInfoVbo = std::make_shared<VertexBufferObject>(BufferUsage::Dynamic);
            pointInfoVbo->SetData(nullptr, 0, {
                        { "aPos", Engine::LType::Float, 3 },
                        { "aColor", Engine::LType::Float, 4 }
                                  });

            pointInfoVao = std::make_shared<VertexArrayObject>();
            pointInfoVao->AddVertexBuffer(pointInfoVbo);
            pointInfoVao->Compute();
        }

        /* Lines */
        {
            lineShader = std::make_shared<Shader>();
            lineShader->AttachVertexShader(lineShader_V);
            lineShader->AttachGeometeryShader(lineShader_G);
            lineShader->AttachFragmentShader(lineShader_F);
            lineShader->Link();

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
	}
}
