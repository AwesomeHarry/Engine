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
    layout (triangle_strip, max_vertices = 66) out;

    layout (std140) uniform CameraData {
        mat4 projection;
        mat4 view;
    };

	in vec3 vPos[];
	in float vRadius[];
	in vec4 vColor[];

    uniform int segments = 20;

	out vec4 color;

    const float PI = 3.1415926535897932384626433832795;

    void main() {
        // Transform the center point to view space
        vec4 centerViewSpace = view * vec4(vPos[0], 1.0);
        float radius = vRadius[0];
    
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
            pointShader->AttachVertexShader(pointShader_V);
            pointShader->AttachGeometeryShader(pointShader_G);
            pointShader->AttachFragmentShader(pointShader_F);
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
            lineShader->AttachVertexShader(lineShader_V);
            lineShader->AttachGeometeryShader(lineShader_G);
            lineShader->AttachFragmentShader(lineShader_F);
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
			quadShader->AttachVertexShader(quadShader_V);
			quadShader->AttachGeometeryShader(quadShader_G);
			quadShader->AttachFragmentShader(quadShader_F);
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
