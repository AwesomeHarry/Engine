#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNor;
layout(location = 2) in vec2 aTex;

layout (std140) uniform CameraData {
	mat4 projection;
	mat4 view;
};
uniform mat4 model;

out vec3 vPos;
out vec3 vNor;
out vec2 vTex;

void main()
{
	vTex = aTex;
    vNor = mat3(transpose(inverse(model))) * aNor;
    vPos = vec3(model * vec4(aPos, 1.0));

    gl_Position = projection * view * vec4(vPos, 1.0);
}