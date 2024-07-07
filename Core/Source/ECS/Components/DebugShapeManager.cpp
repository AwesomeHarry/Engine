#include "DebugShapeManager.h"

#include "Rendering/Platform/Mesh.h"
#include "Rendering/Platform/Shader.h"

namespace Engine {
#pragma region Shaders
    const char* commonVertexShader = R"glsl(
    #version 330 core
    layout(location = 0) in vec3 aPos;
	uniform mat4 model;
    void main() {
        gl_Position = model * vec4(aPos, 1.0);
    }
    )glsl";

#pragma region PointShader
	const char* pointShader_G = R"glsl(
    #version 330 core

    layout (points) in;
    layout (triangle_strip, max_vertices = 66) out;

    layout (std140) uniform CameraData {
        mat4 projection;
        mat4 view;
    };

    uniform float radius = 0.1;
    uniform int segments = 20;

    const float PI = 3.1415926535897932384626433832795;

    void main() {
        // Transform the center point to view space
        vec4 centerViewSpace = view * gl_in[0].gl_Position;
    
        for (int i = 0; i <= segments; i++) {
            float angle = 2.0 * PI * float(i) / float(segments);
            float nextAngle = 2.0 * PI * float(i + 1) / float(segments);
        
            // Emit center vertex
            gl_Position = projection * centerViewSpace;
            EmitVertex();
        
            // Emit vertex on the circle's edge
            gl_Position = projection * (centerViewSpace + vec4(radius * cos(angle), radius * sin(angle), 0.0, 0.0));
            EmitVertex();
        
            // Emit next vertex on the circle's edge
            gl_Position = projection * (centerViewSpace + vec4(radius * cos(nextAngle), radius * sin(nextAngle), 0.0, 0.0));
            EmitVertex();
        
            EndPrimitive();
        }
    }
    )glsl";

	const char* pointShader_F = R"glsl(
    #version 330 core
    out vec4 FragColor;
    uniform vec4 color;
    void main() {
        FragColor = color;
    }
    )glsl";
#pragma endregion

#pragma region LineShader
    const char* lineShader_V = R"glsl(
	#version 330 core
	void main() {
		gl_Position = vec4(0.0,0.0,0.0,1.0);
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

    uniform vec3 p1;
    uniform vec3 p2;
    uniform float thickness;

    void main() {
        // Transform p1 and p2 to clip space
        vec4 p1Clip = projection * view * vec4(p1, 1.0);
        vec4 p2Clip = projection * view * vec4(p2, 1.0);
    
        // Calculate direction in clip space
        vec2 dir = normalize(p2Clip.xy / p2Clip.w - p1Clip.xy / p1Clip.w);
    
        // Calculate normal
        vec2 normal = vec2(-dir.y, dir.x) * thickness / 2.0;
    
        // Emit vertices
        gl_Position = p1Clip + vec4(normal * p1Clip.w, 0.0, 0.0);
        EmitVertex();
    
        gl_Position = p1Clip - vec4(normal * p1Clip.w, 0.0, 0.0);
        EmitVertex();
    
        gl_Position = p2Clip + vec4(normal * p2Clip.w, 0.0, 0.0);
        EmitVertex();
    
        gl_Position = p2Clip - vec4(normal * p2Clip.w, 0.0, 0.0);
        EmitVertex();
    
        EndPrimitive();
    }
    )glsl";

	const char* lineShader_F = R"glsl(
    #version 330 core
    out vec4 FragColor;
    uniform vec4 color;
    void main() {
        FragColor = color;
    }
    )glsl";

#pragma endregion

#pragma endregion

	void DebugShapeManager::DrawPoint(const PointSpec& spec) {
		points.push_back(spec);
	}

	void DebugShapeManager::DrawLine(const LineSpec& spec) {
		lines.push_back(spec);
	}

	void DebugShapeManager::OnComponentAdded(Entity& entity) {
		initialize();
	}

	void DebugShapeManager::initialize() {
		_meshTemplate = std::make_shared<Engine::Mesh>("Test");
		{
			float verts[] = { 0,0,0 };

			auto vertexArray = std::make_shared<Engine::VertexArrayObject>();
			auto vertexBuffer = std::make_shared<Engine::VertexBufferObject>();
			vertexBuffer->SetData(verts, 3, {
				{ "POSITION", Engine::LType::Float, 3 }
								  });
			vertexArray->AddVertexBuffer(vertexBuffer);
			vertexArray->Compute();
			vertexArray->SetDrawMode(Engine::DrawMode::Points);

			_meshTemplate->AddSubmesh(vertexArray);
		}

		_pointShader = std::make_shared<Shader>();
		_pointShader->AttachVertexShader(commonVertexShader);
		_pointShader->AttachGeometeryShader(pointShader_G);
		_pointShader->AttachFragmentShader(pointShader_F);
		_pointShader->Link();

		_lineShader = std::make_shared<Shader>();
        _lineShader->AttachVertexShader(lineShader_V);
        _lineShader->AttachGeometeryShader(lineShader_G);
        _lineShader->AttachFragmentShader(lineShader_F);
        _lineShader->Link();
	}
}
