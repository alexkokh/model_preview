#version 330 core

layout (location = 0) out vec4 outColor;
uniform vec4 inColor;

void main(void)
{
	outColor = vec4(1,.5,0.1,1);
}