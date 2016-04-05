#version 330 core

layout (location = 0) out vec4 outColor;
uniform vec4 inColor;

void main(void)
{
	outColor = inColor;
}