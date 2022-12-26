#version 330 core

// 顶点着色器输入
layout (location = 0) in vec3 vPosition;

out vec3 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(vPosition, 1.0);  
	texcoord = vPosition;	// 坐标作为cubeMap采样坐标
}
