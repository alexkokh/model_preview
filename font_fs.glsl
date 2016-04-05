#version 330 core

uniform sampler2D tex0;

in vec2 tex_coord;

layout (location = 0) out vec4 color;

void main(void)
{
	vec2 uv = vec2(0,0);
	vec4 texColor = texture(tex0, tex_coord);
	color = texColor;
}