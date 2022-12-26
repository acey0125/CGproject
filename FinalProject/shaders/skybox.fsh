#version 330 core

in vec3 texcoord;
out vec4 fColor;

uniform samplerCube skybox;

void main()
{
    fColor = textureCube(skybox, texcoord);
}
